
#include <io.h>
#include "MMS.h"
#include "NameTable.h"
#include "Storage.h"
#include "CommonFunctions.h"
#include "Container.h"
#include "Ordering.h"
#include "MMS_Filter.h"
#include "DLL\GComp.h"
#include "UI.h"
#include "Rights.h"
#include "DialogForm.h"


///////////////////////////////////////////////////////////////////////
///////////  class CMMSObject methods  ////////////////////////////////
///////////////////////////////////////////////////////////////////////
void CMMSObject::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir)
{
	Msg(1, "INF: %s", sID);
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	if( nt_rec == NULL ) return;
	if( !nt_rec->Dir.IsEmpty() )
	{
		GCString Dir = nt_rec->PrepareDirName(SrcDir);
		ImportDescriptions(TypedTextStorage, Dir+"\\", true);
	}

	SendInfoMessage(InfoMsg_Progress, ++TotalObjectsProcessed);
}

void CMMSAttributes_SF::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	if( nt_rec == NULL ) return;
	if( nt_rec->Dir.IsEmpty() ) return;

	GCString Dir = nt_rec->PrepareDirName(SrcDir);
	Msg(1, "INF: %s --> %s", Dir, nt_rec->Type1C);

	ImportOrdering(Dir);
	
	//Если у нас объекты раскиданы по каталогам, то и описания нужно собирать по каталогам
	for( int i = 0; i < GetNChildren(); i++ )
	{
		CMMSObject* obj = GetChild(i);
		GCString SubDir = obj->GetObjectDir(Dir);
		obj->ImportDescriptions(TypedTextStorage, SubDir+"\\", true);
	}

	//А если всё в одном файле, то и описания будут в общей куче
	ImportDescriptions(TypedTextStorage, Dir+"\\", true);

	SendInfoMessage(InfoMsg_Progress, ++TotalObjectsProcessed);
}

void CMMSObject::ImportDescriptions(CStorage& TypedTextStor, GCString& FNamePrefix, bool StdName)
{
	if( MMS_Type == MMS_Property ) return; //Описания бывают только у объектов

	CNameTableRecord* nt_rec_obj = NameTable.Find(MMS_Type);
	CNameTableRecord* nt_rec = NameTable.Find(NT_UserHelpStor);
	CNameTableRecord* nt_rec_file = NameTable.Find(NT_UserHelp);

	MMS_Filter.Down();
	if( MMS_Filter.Test(this, nt_rec_obj) || MMS_Filter.Test(NULL, nt_rec_file) )
	{
		GCString StorName = nt_rec->PrepareStorageName(ID);
		GCString NewPrefix = FNamePrefix;
		GCString Suffix;
		
		if( ID_Type != IDT_NoID && 
			(nt_rec_obj == NULL || nt_rec_obj->Type1C == "sID") )
		{
			if( ID_Type == IDT_OnlyTextual )
				Suffix.Format("%s.", sID);
			else if( ID_Type == IDT_OnlyNumeric )
				Suffix.Format("%s.%i.", nt_rec_file->FileName, ID);
			else if( sID.IsEmpty() || !IsValidFileName(sID) )
				Suffix.Format("%s.%i.", nt_rec_file->FileName, ID);
			else
				Suffix.Format("%s.", sID);
		}

		InplaceTranslit(Suffix);
		NewPrefix += Suffix;

		if( ID_Type == IDT_BothIDs && 
			(!TypedTextStor.StorageExist(StorName) || !MMS_Filter.IsEmpty()) )
		{
			GCString FName;

			if( StdName || Suffix.IsEmpty() )
				FName.Format("%s%s.%s", FNamePrefix, nt_rec_file->FileName, nt_rec_file->Ext);
			else
				FName.Format("%s%s", NewPrefix, nt_rec_file->Ext);

			if( FileExist(FName) )
			{
				if( TypedTextStor.Create(StorName) )
				{
					Msg(2, "CPY: '%s'", FName);
					TypedTextStor.StreamFromFile(nt_rec_file->PrepareStorageName(ID), FName);
					CContainer CC;
					CC.CreateInTypedText(TypedTextStor);
					TypedTextStor.Close();
				}
			}
		}

		for( int i = 0; i < GetNChildren(); i++ )
		{
			GetChild(i)->ImportDescriptions(TypedTextStor, NewPrefix, false);
		}
	}
	MMS_Filter.Up();
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSObjectWithProps methods  ///////////////////////
///////////////////////////////////////////////////////////////////////
bool CMMSObject::ImportMoxelPages(CStorage& Storage, GCString& SrcDir, GCString& ContCont)
{
	bool HavePages = false;
	GCString FName, Page, CC;

	CObjectOrdering Order(4);

	if( !Order.OpenFile(SrcDir) ) return false;

	while( Order.ReadString() )
	{
		//Соберём имена
		FName.Format("%s\\%s", SrcDir, Order.GetValue(2));
		Page.Format("Page.%s", Order.GetValue(0));

		if( TaskParameters.NoEmptyMxl ) //Поскипаем пустые шаблоны
		{
			long size = GetFileSize(FName);
			if( size == 147 || size == 139 ) continue;
		}

		Storage.StreamFromFile(Page, FName);

		CC.Format(",{\"Moxcel.Worksheet\",\"%s\",\"%s\",\"\"}", Page, Order.GetValue(3));
		ContCont += CC;

		HavePages = true;
	}

	return HavePages;
}

void CMMSObject::ImportContainerProfile(CStorage& Storage, GCString& SrcDir)
{
	GCString ContProfName = SrcDir + "\\Container.Profile";
	GCString StreamName = "Container.Profile";

	if( FileExist(ContProfName) )
	{
		Storage.StreamFromFile(StreamName, ContProfName);
		return;
	}

	if( TaskParameters.NoProfiles ) return;

	GCString CP = "{\n\
{\"MoxelName\",\"\",\"\"},\n\
{\"MoxelPos\",\"0\",\"\"},\n\
{\"UUID\",\"D41D8CD98F00B204E9800998ECF8427E\",\"\"},\n\
{\"Entry\",\"1\",\"\"},\n\
{\"MoxelNextMode\",\"1\",\"\"}}";

	Storage.StreamFromString(StreamName, CP);
}


bool CMMSObject::ImportStreamByType(CStorage& Storage, MMSObjectType Type, GCString& SrcDir)
{
	CNameTableRecord* nt_rec = NameTable.Find(Type);
	GCString FileName = nt_rec->PrepareFileName(SrcDir);
	bool res;

	MMS_Filter.Down();
	if( MMS_Filter.Test(this, nt_rec) )
	{
		if( !FileExist(FileName) ) return false;

		CStorage SubStorage(Storage);
		res = SubStorage.StreamFromFile(
			nt_rec->PrepareStorageName(ID), 
			FileName);
	}
	MMS_Filter.Up();
	
	return res;
}

bool DialogInBeautyFormat(GCString FormFName)
{
	FILE *f = fopen(FormFName, "rt");
	char buf[16];
	fgets(buf, sizeof(buf)-1, f);
	fclose(f);
	GCString hdr = buf;
	hdr.TrimLeft();
	hdr.TrimRight();
	return ( _stricoll("Диалог:", hdr) == 0 );
}

bool CMMSObject::ImportWorkBook(CStorage& Storage, GCString& SrcDir)
{
	CNameTableRecord* nt_rec_wb = NameTable.Find(NT_Workbook);
	CNameTableRecord* nt_rec;
	GCString ContCont = "{\"Container.Contents\"";
	bool StorageNotEmpty = false;

	//Описание потока ММС для внешних отчетов
	if( TaskParameters.CompoundType == external_report )
		ContCont += ",{\"MetaDataHolderContainer\",\"Main MetaData Stream\",\"Main MetaData Stream\",\"\"}";

	if( TaskParameters.CompoundType != external_report )
		Storage.Create(nt_rec_wb->StorageName);

	//Сама форма
	bool dlg_form_imported = false;
	nt_rec = NameTable.Find(NT_DialogForm);

	MMS_Filter.Down();
	if( MMS_Filter.Test(this, nt_rec) )
	{
		GCString FormFName = nt_rec->PrepareFileName(SrcDir);
		if( FileExist(FormFName) )
		{
			if( DialogInBeautyFormat(FormFName) )
			{
				CDialogForm DlgForm(NULL);
				DlgForm.ParseFile(FormFName);
				CMMSString DlgAsString;
				DlgForm.ToString(DlgAsString);
				if( Storage.StreamFromString(nt_rec->StorageName, DlgAsString) )
				{
					dlg_form_imported = true;
					StorageNotEmpty = true;
				}
			}
			else
			{
				if( Storage.StreamFromFile(nt_rec->StorageName, FormFName) )
				{
					dlg_form_imported = true;
					StorageNotEmpty = true;
				}
			}
		}
		if( dlg_form_imported )
		{
			ContCont += ",{\"DialogEditor\",\"Dialog Stream\",\"Dialog Form\",\"\"}";
		}
	}
	MMS_Filter.Up();

	//Модуль формы
	nt_rec = NameTable.Find(NT_FormModule);
	GCString ModuleFName = nt_rec->PrepareFileName(SrcDir);
	if( dlg_form_imported || FileExist(ModuleFName) )
	{
		MMS_Filter.Down();
		if( MMS_Filter.Test(NULL, nt_rec) )
		{
			if( !Storage.StreamFromFile(nt_rec->StorageName, ModuleFName) )
			{
				Storage.StreamFromString(nt_rec->StorageName, GCString()); //создаём пустой модуль
				StorageNotEmpty = true;
			}
			ContCont += ",{\"TextDocument\",\"MD Programm text\",\"Module text\",\"\"}";
		}
		MMS_Filter.Up();
	}

	//Для внешних отчетов описание складывается прямо в контейнер. Поток назвается "Inplace description"
	if( TaskParameters.CompoundType == external_report )
	{
		if( !ImportStreamByType(Storage, NT_ErtUserHelp, SrcDir) )
		{
			nt_rec = NameTable.Find(NT_ErtUserHelp);
			Storage.StreamFromString(nt_rec->PrepareStorageName(), GCString());
			StorageNotEmpty = true;
		}
		ContCont += ",{\"MetaDataDescription\",\"Inplace description\",\"Описание\",\"\"}";
	}

	//Шаблоны таблиц
	MMS_Filter.Down();
	if( MMS_Filter.Test(GCString("ШаблоныТаблиц")) || MMS_Filter.Test(GCString("MoxelPages")) )
	{
		if( ImportMoxelPages(Storage, SrcDir, ContCont) )
			StorageNotEmpty = true;
	}
	MMS_Filter.Up();

	//Container.Contents и Container.Profile создаём только при полной компиляции
	if( MMS_Filter.IsEmpty() )
	{
		ContCont += "}\n";
		Storage.StreamFromString("Container.Contents", ContCont);
		
		//Container.Profile
		ImportContainerProfile(Storage, SrcDir);
	}
	if( TaskParameters.CompoundType != external_report )
	{
		Storage.Close();
		if( !StorageNotEmpty )
			Storage.Delete(nt_rec_wb->StorageName);
	}

	return StorageNotEmpty;
}

bool CMMSObject::ImportListForms(CFormDescrList& Forms, CStorage& Storage, 
										  GCString& SrcDir, GCString Prefix)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_ListForm);
	GCString StorName;
	bool NotEmpty = false;

	for( int i = 0; i < Forms.GetNChildren(); i++ )
	{
		CMMSObject* Form = Forms.GetChild(i);
		MMS_Filter.Down();
		if( MMS_Filter.Test(Form, nt_rec) || MMS_Filter.Test(GCString("ФормыСписков")) )
		{
			GCString FormDir;
			FormDir.Format("%s\\%s.%s", SrcDir, Translit(Form->sID), nt_rec->Ext);
			
			StorName.Format("%s%i", Prefix, Form->ID);
			Storage.Create(StorName);
			NotEmpty = ImportWorkBook(Storage, FormDir);
			Storage.Close();
			if( !NotEmpty )
				Storage.Delete(StorName);
		}
		MMS_Filter.Up();
	}

	return NotEmpty;
}

void CMMSObject::ImportOrdering(GCString& SrcDir)
{
	CObjectOrdering Order(1);
	if( !Order.OpenFile(SrcDir) ) return;
	
	int i = 0;
	while( Order.ReadString() )
	{
		CMMSObject* obj = NULL;
		char* str_id = Order.GetValue(0);
		if( isdigit(*(unsigned char*)str_id) != 0 )
			obj = FindChild(atoi(str_id), false);
		else
			obj = FindChild(GCString(str_id), false);

		if( obj != NULL ) obj->Order = i++;
	}

	SortByOrder();
}


void CMMSTaskItem::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& Dir)
{
	CMMSAttributes::Compile(Storage, TypedTextStorage, Dir);

	CNameTableRecord* nt_rec = NameTable.Find(NT_UserHelpStor);
	CNameTableRecord* nt_rec_file = NameTable.Find(NT_UserHelp);
	GCString StorName = nt_rec->PrepareStorageName(Content.ID);
	GCString FName = nt_rec_file->PrepareFileName(Dir);

	if( FileExist(FName) )
	{
		if( TypedTextStorage.Create(StorName) )
		{
			Msg(2, "CPY: '%s'", FName);
			TypedTextStorage.StreamFromFile(nt_rec_file->PrepareStorageName(Content.ID), FName);
			CContainer CC;
			CC.CreateInTypedText(TypedTextStorage);
			TypedTextStorage.Close();
		}
	}
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMS methods  //////////////////////////////////////
///////////////////////////////////////////////////////////////////////
bool CMMS::ImportStorage(CStorage& Storage, GCString& SrcDir, MMSObjectType Type)
{
	CNameTableRecord* nt_rec = NameTable.Find(Type);
	CStorage DestStor(Storage);
	bool res;

	MMS_Filter.Down();
	if( MMS_Filter.Test(NULL, nt_rec) )
	{
		res = DestStor.StorageFromDir(nt_rec->PrepareStorageName(ID), 
									  nt_rec->PrepareDirName(SrcDir));
	}
	MMS_Filter.Up();

	return res;
}

void CMMS::ImportCommonMXL(CStorage& Storage, GCString& SrcDir)
{
	CNameTableRecord* nt_rec_gd = NameTable.Find(NT_GlobalData);
	CNameTableRecord* nt_rec = NameTable.Find(NT_CommonMXL);
	
	MMS_Filter.Down();
	if( MMS_Filter.Test(NULL, nt_rec) )
	{
		Msg(1, "INF: Общие таблицы");
		CStorage GDStorage(Storage);
		GDStorage.Create(nt_rec_gd->StorageName);
		CStorage MXLStorage(GDStorage);
		MXLStorage.Create(nt_rec->StorageName);

		if( !ImportWorkBook(MXLStorage, nt_rec->PrepareDirName(SrcDir)) )
		{
			MXLStorage.Close();
			GDStorage.Delete(nt_rec->StorageName);
		}
	}
	MMS_Filter.Up();
}

void CMMS::ImportGlobalModule(CStorage& Storage, GCString& Dir)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_GlobalModuleStor);
	CStorage GMStorage(Storage);

	if( ImportStreamByType(Storage, NT_GlobalModule, Dir) && GMStorage.Open(nt_rec->StorageName) )
	{
		GMStorage.Open(nt_rec->StorageName);
		CContainer CC;
		CC.CreateInTypedText(GMStorage);
	}
}


void CMMS::ImportMMS(CStorage& Storage)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_MainMetadataStream);
	CMMSString String;

	//Изменим номер версии
	MainDataContDef.MD_ver = 10009;

	if( TaskParameters.CompoundType != external_report )
	{
		Storage.Create(GCString("Metadata"));
	}

	ToString(String);
	Storage.StreamFromString(nt_rec->StorageName, String);

	if( TaskParameters.CompoundType == external_report )
	{
		Storage.AddZeroByte(); //Во внешнем отчёте в конце ММС обязан быть '\0'
	}

	if( TaskParameters.CompoundType != external_report )
	{
		Storage.Close();
	}
}

void CMMS::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& Dir)
{
	//firstly try to read name cache
	if( MMS_Filter.IsEmpty() )
		ReadObjNameCacheFromDir(Dir);
	//Build names cache
	BuildObjNameCache(ObjNameCache_compare_str, Dir, Storage);

	if( TaskParameters.CompoundType == external_report	)
	{
		ImportWorkBook(Storage, Dir);
		ImportStorage(Storage, Dir, NT_PictureGallery);
		ImportMMS(Storage);

		CContainer CC;
		CC.Create(Storage, true);

		return;
	}
	
	SendInfoMessage(InfoMsg_Total, CountObjects());

	//Глобальный модуль
	ImportGlobalModule(TypedTextStorage, Dir);

	for( int i = 0; i < GetNChildren(); i++ )
	{
		CMMSObject *obj = GetChild(i);
		Msg(2, "%s:", obj->sID);
		obj->Compile(Storage, TypedTextStorage, Dir);
	}

	//Общие таблицы
	ImportCommonMXL(Storage, Dir);
	//Картинки
	ImportStorage(Storage, Dir, NT_PictureGallery);
	
	//Интерфейсы
	CompileUI(Storage, Dir);
	//Права
	CompileUserRights(Storage, Dir);

	//Служебные потоки
	ImportStreamByType(Storage, NT_GUIDData, Dir);
	ImportStreamByType(Storage, NT_TagStream, Dir);

	//Container.Contents в каталоге UserDef
	Storage.Open(GCString("UserDef"));
	GCString CC = "{\"Container.Contents\",{\"WorkPlaceType\",\"Page.1\",\"\",\"\"},{\"RigthType\",\"Page.2\",\"\",\"\"}}";
	Storage.StreamFromString("Container.Contents", CC);
	Storage.Close();

	//Самое главное - Main Metadata Stream
	Msg(1, "INF: Создание потока MainMetadataStream");
	if( MMS_Filter.IsEmpty() )
		ImportMMS(Storage);

	//Рекурсивный обход всех storage и создание Container.Contents там где его нет.
	Msg(1, "INF: Создание потоков Container.Contents");

	CContainer ContainerContent;
	ContainerContent.Create(Storage, true);
	ContainerContent.Create(TypedTextStorage, true);
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSReport methods  ////////////////////////////////
///////////////////////////////////////////////////////////////////////
void CMMSReport::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir, GCString& SubStroragePrefix)
{
	MMS_Filter.Down();
	if( MMS_Filter.Test(this, NULL) )
	{
		GCString RptDir = GetObjectDir(SrcDir);
		GCString StorName;

		Msg(1, "INF: Отчёт.%s", sID);

		StorName.Format("%s%i", SubStroragePrefix, ID);
		Storage.Create(StorName);
		bool NotEmpty = ImportWorkBook(Storage, RptDir);
		Storage.Close();
		if( !NotEmpty )
			Storage.Delete(StorName);

		ImportDescriptions(TypedTextStorage, RptDir+"\\", true);
	}
	MMS_Filter.Up();

	SendInfoMessage(InfoMsg_Progress, ++TotalObjectsProcessed);
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSReportList methods  /////////////////////////////
///////////////////////////////////////////////////////////////////////
void CMMSReportList::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir)
{
	CNameTableRecord* nt_rec = NameTable.Find(sID);
	GCString ReportsDir = nt_rec->PrepareDirName(SrcDir);

	MMS_Filter.Down();
	if( MMS_Filter.Test(this, nt_rec) )
	{
		ImportOrdering(ReportsDir);

		Storage.Create(nt_rec->StorageName);
		GCString ReportStorName = nt_rec->StorageName + "_Number";
		for( int i = 0; i < GetNChildren(); i++ )
		{
			((CMMSReport*)GetChild(i))->Compile(Storage, TypedTextStorage, ReportsDir, ReportStorName);
		}
		Storage.Close();
	}
	MMS_Filter.Up();
}
