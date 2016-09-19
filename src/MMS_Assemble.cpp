
#include "DLL\GComp.h"
#include "Declarations.h"
#include "CommonFunctions.h"
#include "MMS.h"
#include "NameTable.h"
#include "Ordering.h"


void CMMSObject::AssembleMetadata(GCString& Dir)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	
	if( nt_rec == NULL || nt_rec->Ext != "mdp" )
		nt_rec = NameTable.Find(NT_MetadataPart);

	GCString FileName = nt_rec->PrepareFileName(Dir);
	if( !FileExist(FileName) )
	{
		nt_rec = NameTable.Find(NT_MetadataPart);
		FileName = nt_rec->PrepareFileName(Dir);
	}

	if( FileExist(FileName) )
		ParseFile(FileName);
}

void CMMSAttributes::AssembleMetadata(GCString& Dir)
{
	//на случай, если все метаданные собраны в одном файле, а не по каталогам
	CMMSObject::AssembleMetadata(Dir);
	if( GetNChildren() > 0 )
		return;

	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	if( nt_rec == NULL ) return;
	if( nt_rec->Dir.IsEmpty() ) return;
	
	GCString DirName = nt_rec->PrepareDirName(Dir);
	if( DirectoryExist(DirName) )
	{
		struct _finddata_t find_data;
		long hFind;
		GCString SubDir;

		hFind = _findfirst(DirName+"\\*", &find_data);
		if( hFind == -1 ) return;
		do
		{
			if( (find_data.attrib & _A_SUBDIR) == 0 ) continue;
			if( IsIgnoredFile(find_data.name) ) continue;

			SubDir.Format("%s\\%s", DirName, find_data.name);
			if( !FilesExist(SubDir+"\\*.mdp") ) continue; //пустые каталоги нам не нужны

			CMMSObject* Child = CreateChild(NULL);
			Child->AssembleMetadata(SubDir);
			AddChild(Child);
		} while( _findnext(hFind, &find_data) == 0 );
		_findclose(hFind);

		ImportOrdering(DirName);
	}
}

void CMMSAccountPlan::AssembleMetadata(GCString& PlanDir)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_MetadataPart);
	GCString PlanMDP = nt_rec->PrepareFileName(PlanDir);

	ParseFile(PlanMDP);

	CObjectOrdering Order(3);
	if( Order.OpenFile(PlanDir) )
	{
		GCString AccFName;
		while( Order.ReadString() )
		{
			AccFName.Format("%s\\%s", PlanDir, Order.GetValue(2));

			CMMSObject* Account = Accounts.CreateChild(NULL);
			Account->ParseFile(AccFName);
			Accounts.AddChild(Account);
		}
	}
	AddChild(&Accounts);
}

void CMMSAccountPlanList::AssembleMetadata(GCString& BuhDir)
{
	CNameTableRecord* nt_rec_plans = NameTable.Find(MMS_Plans);
	CNameTableRecord* nt_rec_plan = NameTable.Find(MMS_Plan);
	GCString PlansDir = nt_rec_plans->PrepareDirName(BuhDir);
	
	CObjectOrdering Order(3);
	if( Order.OpenFile(PlansDir) )
	{
		GCString PlanDir;
		while( Order.ReadString() )
		{
			PlanDir.Format("%s\\%s", PlansDir, Order.GetValue(2));
			
			CMMSObject* Plan = CreateChild(NULL);
			Plan->AssembleMetadata(PlanDir);
			AddChild(Plan);
		}
	}
}

void CMMSBuh::AssembleMetadata(GCString& BuhDir)
{
	CNameTableRecord* nt_rec;
	
	AccountPlans.AssembleMetadata(BuhDir);
	AddChild(&AccountPlans);

	//формы списков
	nt_rec = NameTable.Find(NT_AccListForm);
	AccListForms.ParseFile(nt_rec->PrepareFileName(BuhDir));
	AddChild(&AccListForms);

	nt_rec = NameTable.Find(NT_UnknownListForm);
	UnknownListForms.ParseFile(nt_rec->PrepareFileName(BuhDir));
	AddChild(&UnknownListForms);

	nt_rec = NameTable.Find(NT_ProvListForm);
	ProvListForms.ParseFile(nt_rec->PrepareFileName(BuhDir));
	AddChild(&ProvListForms);
}

void CMMSBuhContainer::AssembleMetadata(GCString& Dir)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Buh);
	CNameTableRecord* nt_rec_mdp = NameTable.Find(NT_MetadataPart);
	GCString BuhDir = nt_rec->PrepareDirName(Dir);

	if( GetNChildren() == 0 )
		AddChild(new CMMSBuh);

	GetChild(0)->ParseFile(nt_rec_mdp->PrepareFileName(BuhDir));
	GetChild(0)->AssembleMetadata(BuhDir);
}
