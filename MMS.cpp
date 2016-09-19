
#include "CommonFunctions.h"
#include "Strings.h"
#include "NameTable.h"
#include "MMS.h"
#include "MMS_Subconto.h"
#include "MMS_Document.h"
#include "MMS_Buh.h"
#include "MMS_Calculation.h"


//Сортировка таблицы соответствия идентификаторв и имён объектов (по ID)
int __cdecl ObjNameCache_compare_id(const void *elem1, const void *elem2)
{
	TObjNameCacheRecord* rec1 = (TObjNameCacheRecord*)elem1;
	TObjNameCacheRecord* rec2 = (TObjNameCacheRecord*)elem2;
	int id1 = rec1->ID;
	int id2 = rec2->ID;
	if( id1 < id2 ) return -1;
	if( id1 > id2 ) return 1;
	return 0;
}

//Сортировка таблицы соответствия идентификаторв и имён объектов (по текстовому ID)
int __cdecl ObjNameCache_compare_str(const void *elem1, const void *elem2)
{
	TObjNameCacheRecord* rec1 = (TObjNameCacheRecord*)elem1;
	TObjNameCacheRecord* rec2 = (TObjNameCacheRecord*)elem2;
	//return rec1->Name.Collate(rec2->Name);
	return strcmp(rec1->Name, rec2->Name);
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSMainDataContDef tables and methods  ////////////
///////////////////////////////////////////////////////////////////////
CMMSMainDataContDef::CMMSMainDataContDef()
{
	MMS_Type = MMS_MainDataContDef;
	ID_Type = IDT_OnlyTextual;

	sID = BeautyName();
	ID = 0;
	FirstAvailableID = 0;
	MD_ver = 0;
	UnknownID = 0;
}


CMMSMainDataContDef* CMMSMainDataContDef::DefVal = new CMMSMainDataContDef;
CObjectProperty CMMSMainDataContDef::Props[] = {
	{"Идентификатор", 0, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"ID",            -1, CObjectProperty::t_int, VAR_OFFSET(DefVal, ID)},
	{"Next ID",       1, CObjectProperty::t_int, VAR_OFFSET(DefVal, FirstAvailableID)},
	{"Версия",        2, CObjectProperty::t_int, VAR_OFFSET(DefVal, MD_ver)},
	{"Unknown ID",    3, CObjectProperty::t_int, VAR_OFFSET(DefVal, UnknownID)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSMainDataContDef::PropSet(Props, DefVal);

///////////////////////////////////////////////////////////////////////
///////////  class CMMSTaskItem tables and methods  ///////////////////
///////////////////////////////////////////////////////////////////////
CMMSTaskItemContent* CMMSTaskItemContent::DefVal = new CMMSTaskItemContent;
CObjectProperty CMMSTaskItemContent::Props[] = {
	{"ID",            0, CObjectProperty::t_int, VAR_OFFSET(DefVal, ID)},
	{"Идентификатор", 1, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",   2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",       3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Пароль",        4, CObjectProperty::t_string, VAR_OFFSET(DefVal, Password)},
	{"Основной язык", 5, CObjectProperty::t_prop, VAR_OFFSET(DefVal, BasicLanguage)},
	{"Param7",        6, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param7)},
	{"Непосредственное удаление", 7, CObjectProperty::t_prop, VAR_OFFSET(DefVal, ImmediateDeletion)},
	{"Способ округления", 8, CObjectProperty::t_prop, VAR_OFFSET(DefVal, RoundType)},
	{"Param10",        9, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param10)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSTaskItemContent::PropSet(Props, DefVal);

static CStoredSet::TSetPair setBasicLanguage[] = {
	{0, "Английский"},
	{1, "Русский"},
	{0, NULL}
};
static CStoredSet::TSetPair setRoundType[] = {
	{0, "Округлять 1.5 до 2"},
	{1, "Округлять 1.5 до 1"},
	{0, NULL}
};

CMMSTaskItemContent::CMMSTaskItemContent()
{
	MMS_Type = MMS_TaskItemContent;
	ID_Type = IDT_BothIDs;

	BasicLanguage = 1;
	ImmediateDeletion = false;
	RoundType = 0;

	BasicLanguage.SetTable(setBasicLanguage);
	RoundType.SetTable(setRoundType);
}

void CMMSTaskItemContent::BeautyString(CBeautyString& String, int level)
{
	String.add_brace(level, '{');

	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet != NULL )
	{
		for( int i = 1; i < pPropSet->nProps; i++ ) //пропускаем ID
			pPropSet->BeautyString(String, level+1, this, i);
	}
	
	String.add_brace(level, '}');
}

////////////////////////////////////////////////////////////////////////
CMMSTaskItem::CMMSTaskItem()
{
	MMS_Type = MMS_TaskItem;
	ID_Type = IDT_OnlyTextual;
	ToDestroyChildren = false;
	CMMSAttributes::AddChild(&Content);
}



///////////////////////////////////////////////////////////////////////
///////////  class CMMS methods  //////////////////////////////////////
///////////////////////////////////////////////////////////////////////
CMMS* CMMS::DefVal = new CMMS;
CObjectProperty CMMS::Props[] = {
	{"MainDataContDef",		 0, CObjectProperty::t_object, VAR_OFFSET(DefVal, MainDataContDef)},
	{"TaskItem",			 1, CObjectProperty::t_object, VAR_OFFSET(DefVal, TaskItem)},
	{"GenJrnlFldList",		 2, CObjectProperty::t_object, VAR_OFFSET(DefVal, GenJrnlFldList)},
	{"DocSelRefObjList",	 3, CObjectProperty::t_object, VAR_OFFSET(DefVal, DocSelRefObjList)},
	{"DocNumDefList",		 4, CObjectProperty::t_object, VAR_OFFSET(DefVal, DocNumDefList)},
	{"ConstantList",		 5, CObjectProperty::t_object, VAR_OFFSET(DefVal, ConstantList)},
	{"SbCnts",				 6, CObjectProperty::t_object, VAR_OFFSET(DefVal, SbCnts)},
	{"RegisterList",		 7, CObjectProperty::t_object, VAR_OFFSET(DefVal, RegisterList)},
	{"Documents",			 8, CObjectProperty::t_object, VAR_OFFSET(DefVal, Documents)},
	{"JournalList",			 9, CObjectProperty::t_object, VAR_OFFSET(DefVal, JournalList)},
	{"EnumList",			10, CObjectProperty::t_object, VAR_OFFSET(DefVal, EnumList)},
	{"ReportList",			11, CObjectProperty::t_object, VAR_OFFSET(DefVal, ReportList)},
	{"CalcJournalList",		12, CObjectProperty::t_object, VAR_OFFSET(DefVal, CalcJournalList)},
	{"CalendarList",		13, CObjectProperty::t_object, VAR_OFFSET(DefVal, CalendarList)},
	{"CaclAlgorithmList",	14, CObjectProperty::t_object, VAR_OFFSET(DefVal, CaclAlgorithmList)},
	{"RecalcRuleList",		15, CObjectProperty::t_object, VAR_OFFSET(DefVal, RecalcRuleList)},
	{"CalcVarList",			16, CObjectProperty::t_object, VAR_OFFSET(DefVal, CalcVarList)},
	{"CaclAlgGroupList",	17, CObjectProperty::t_object, VAR_OFFSET(DefVal, CaclAlgGroupList)},
	{"DocStreamList",		18, CObjectProperty::t_object, VAR_OFFSET(DefVal, DocStreamList)},
	{"BuhContainer",		19, CObjectProperty::t_object, VAR_OFFSET(DefVal, BuhContainer)},
	{"CRC",					20, CObjectProperty::t_object, VAR_OFFSET(DefVal, CRC)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMS::PropSet(Props, DefVal);

CMMS::CMMS()
{
	sID = "MainMetadataStream";
	ToDestroyChildren = false;

	//добавим всё в список потомков чтобы можно было построить ObjNameCache
	CMMSAttributes::AddChild(&MainDataContDef);
	CMMSAttributes::AddChild(&TaskItem);
	CMMSAttributes::AddChild(&GenJrnlFldList);
	CMMSAttributes::AddChild(&DocSelRefObjList);
	CMMSAttributes::AddChild(&DocNumDefList);
	CMMSAttributes::AddChild(&ConstantList);
	CMMSAttributes::AddChild(&SbCnts);
	CMMSAttributes::AddChild(&RegisterList);
	CMMSAttributes::AddChild(&Documents);
	CMMSAttributes::AddChild(&JournalList);
	CMMSAttributes::AddChild(&EnumList);
	CMMSAttributes::AddChild(&ReportList);
	CMMSAttributes::AddChild(&CalcJournalList);
	CMMSAttributes::AddChild(&CalendarList);
	CMMSAttributes::AddChild(&CaclAlgorithmList);
	CMMSAttributes::AddChild(&RecalcRuleList);
	CMMSAttributes::AddChild(&CalcVarList);
	CMMSAttributes::AddChild(&CaclAlgGroupList);
	CMMSAttributes::AddChild(&DocStreamList);
	CMMSAttributes::AddChild(&BuhContainer);
}

CMMSObject* CMMS::CreateChild(const char* Name, int MMS_Index)
{
	if( Name != NULL )
	{
		if( strcmp(Name, "MainDataContDef") == 0 )
			return &MainDataContDef;
		if( strcmp(Name, TaskItem.MMSName()) == 0 )
			return &TaskItem;
		if( strcmp(Name, GenJrnlFldList.MMSName()) == 0 )
			return &GenJrnlFldList;
		if( strcmp(Name, DocSelRefObjList.MMSName()) == 0 )
			return &DocSelRefObjList;
		if( strcmp(Name, DocNumDefList.MMSName()) == 0 )
			return &DocNumDefList;
		if( strcmp(Name, ConstantList.MMSName()) == 0 )
			return &ConstantList;
		if( strcmp(Name, SbCnts.MMSName()) == 0 )
			return &SbCnts;
		if( strcmp(Name, RegisterList.MMSName()) == 0 )
			return &RegisterList;
		if( strcmp(Name, Documents.MMSName()) == 0 )
			return &Documents;
		if( strcmp(Name, JournalList.MMSName()) == 0 )
			return &JournalList;
		if( strcmp(Name, EnumList.MMSName()) == 0 )
			return &EnumList;
		if( strcmp(Name, ReportList.MMSName()) == 0 )
			return &ReportList;
		if( strcmp(Name, CalcJournalList.MMSName()) == 0 )
			return &CalcJournalList;
		if( strcmp(Name, CalendarList.MMSName()) == 0 )
			return &CalendarList;
		if( strcmp(Name, CaclAlgorithmList.MMSName()) == 0 )
			return &CaclAlgorithmList;
		if( strcmp(Name, RecalcRuleList.MMSName()) == 0 )
			return &RecalcRuleList;
		if( strcmp(Name, CalcVarList.MMSName()) == 0 )
			return &CalcVarList;
		if( strcmp(Name, CaclAlgGroupList.MMSName()) == 0 )
			return &CaclAlgGroupList;
		if( strcmp(Name, DocStreamList.MMSName()) == 0 )
			return &DocStreamList;
		if( strcmp(Name, BuhContainer.MMSName()) == 0 )
			return &BuhContainer;
		if( strcmp(Name, "CRC") == 0 )
			return &CRC;
	}

	switch( MMS_Index )
	{
	case 0:
		return &MainDataContDef;
	case 1:
		return &TaskItem;
	case 2:
		return &GenJrnlFldList;
	case 3:
		return &DocSelRefObjList;
	case 4:
		return &DocNumDefList;
	case 5:
		return &ConstantList;
	case 6:
		return &SbCnts;
	case 7:
		return &RegisterList;
	case 8:
		return &Documents;
	case 9:
		return &JournalList;
	case 10:
		return &EnumList;
	case 11:
		return &ReportList;
	case 12:
		return &CalcJournalList;
	case 13:
		return &CalendarList;
	case 14:
		return &CaclAlgorithmList;
	case 15:
		return &RecalcRuleList;
	case 16:
		return &CalcVarList;
	case 17:
		return &CaclAlgGroupList;
	case 18:
		return &DocStreamList;
	case 19:
		return &BuhContainer;
	case 20:
		return &CRC;
	}
	return NULL;
}

CMMSObject* CMMS::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, MainDataContDef.BeautyName()) == 0 )
		return &MainDataContDef;
	if( _stricoll(Name, TaskItem.BeautyName()) == 0 )
		return &TaskItem;
	if( _stricoll(Name, GenJrnlFldList.BeautyName()) == 0 )
		return &GenJrnlFldList;
	if( _stricoll(Name, DocSelRefObjList.BeautyName()) == 0 )
		return &DocSelRefObjList;
	if( _stricoll(Name, DocNumDefList.BeautyName()) == 0 )
		return &DocNumDefList;
	if( _stricoll(Name, ConstantList.BeautyName()) == 0 )
		return &ConstantList;
	if( _stricoll(Name, SbCnts.BeautyName()) == 0 )
		return &SbCnts;
	if( _stricoll(Name, RegisterList.BeautyName()) == 0 )
		return &RegisterList;
	if( _stricoll(Name, Documents.BeautyName()) == 0 )
		return &Documents;
	if( _stricoll(Name, JournalList.BeautyName()) == 0 )
		return &JournalList;
	if( _stricoll(Name, EnumList.BeautyName()) == 0 )
		return &EnumList;
	if( _stricoll(Name, ReportList.BeautyName()) == 0 )
		return &ReportList;
	if( _stricoll(Name, CalcJournalList.BeautyName()) == 0 )
		return &CalcJournalList;
	if( _stricoll(Name, CalendarList.BeautyName()) == 0 )
		return &CalendarList;
	if( _stricoll(Name, CaclAlgorithmList.BeautyName()) == 0 )
		return &CaclAlgorithmList;
	if( _stricoll(Name, RecalcRuleList.BeautyName()) == 0 )
		return &RecalcRuleList;
	if( _stricoll(Name, CalcVarList.BeautyName()) == 0 )
		return &CalcVarList;
	if( _stricoll(Name, CaclAlgGroupList.BeautyName()) == 0 )
		return &CaclAlgGroupList;
	if( _stricoll(Name, DocStreamList.BeautyName()) == 0 )
		return &DocStreamList;
	if( _stricoll(Name, BuhContainer.BeautyName()) == 0 )
		return &BuhContainer;

	return NULL;
}

CMMSObject* CMMS::AddChild(CMMSObject* Child)
{
	return NULL; //искуственная мера - чтобы при парсинге объекты не добавлялись повторно
}

bool CMMS::ObjectExist(int ID)
{
	TObjNameCacheRecord* cache_entry = 
		(TObjNameCacheRecord*) bsearch(&ID, ObjNameCache.GetData(), 
		ObjNameCache.GetSize(), sizeof(TObjNameCacheRecord),
		ObjNameCache_compare_id);
	return cache_entry != NULL;
}

GCString& CMMS::ObjectIDtoName(int ID)
{
	if( ID == 0 ) return EmptyObjectName;

	TObjNameCacheRecord* cache_entry = 
		(TObjNameCacheRecord*) bsearch(&ID, ObjNameCache.GetData(), 
		ObjNameCache.GetSize(), sizeof(TObjNameCacheRecord),
		ObjNameCache_compare_id);
	if( cache_entry != NULL )
		return cache_entry->Name;
	else
	{
		//Msg(0, "ERR: Не найден объект с идентификатором %i", ID);
		return EmptyObjectName;
		//throw NULL;
	}
}

int CMMS::ObjectNameToID(GCString& Name)
{
	if( Name.IsEmpty() ) return 0;

	TObjNameCacheRecord record;
	record.Name = Name;

	TObjNameCacheRecord* cache_entry = 
		(TObjNameCacheRecord*) bsearch(&record, ObjNameCache.GetData(), 
		ObjNameCache.GetSize(), sizeof(TObjNameCacheRecord),
		ObjNameCache_compare_str);
	if( cache_entry != NULL )
		return cache_entry->ID;
	else
	{
		Msg(0, "WARN: Не найден объект с идентификатором %s", Name);
		return 0;
		//throw NULL;
	}
}

void CMMS::AssembleMetadata(GCString& Dir)
{
	if( TaskParameters.CompoundType == external_report )
	{
		if( TaskParameters.TruncateMMS )
		{
			TaskItem.Content.ID = 1;
			BuhContainer.Buh.ID = 2;
			return;
		}

		CNameTableRecord* nt_rec = NameTable.Find(NT_MetadataPart);
		GCString FName = nt_rec->PrepareFileName(Dir);
		CLexer Lexer(FName);
		
		if( !Lexer.IsValid() ) //метаданных нет, значит декомпилировали с ключом --truncate-mms
		{
			TaskItem.Content.ID = 1;
			BuhContainer.Buh.ID = 2;
			return;
		}

		ParseFile(Lexer);
		return;
	}
	else
	{
		for( int i = 0; i < GetNChildren(); i++ )
			GetChild(i)->AssembleMetadata(Dir);
	}
}

int CMMS::CountObjects()
{
	int counter = 0;
	for( int i = 0; i < GetNChildren(); i++ )
		counter += GetChild(i)->CountObjects();
	return counter;
}