
#include "DLL\GComp.h"
#include "CommonFunctions.h"
#include "Strings.h"
#include "NameTable.h"
#include "MMS.h"
#include "MMS_Subconto.h"
#include "MMS_Document.h"
#include "MMS_Buh.h"
#include "MMS_Calculation.h"
#include "Lexer.h"
#include "Ordering.h"


///////////////////////////////////////////////////////////////////////
///////////  class CMMSObject methods  ////////////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSObject::CMMSObject()
{
	MMS_Type = MMS_Object;
	ID = 0;
	ID_Type = IDT_NoID;
	//Used = false;
	Order = 0;
	ToDestroyChildren = true;
}

CMMSObject::CMMSObject(CMMSObject& src)
{
	Children.Copy(src.Children);
	src.ToDestroyChildren = false;
}

CMMSObject::~CMMSObject()
{
	if( ToDestroyChildren )
		DestroyObjects();
}

void CMMSObject::DestroyObjects()
{
	int nProps = GetNChildren();
	while( nProps > 0 )
	{
		CMMSObject* obj = GetChild(--nProps);
		if( obj != NULL ) delete obj;
	}
}

void CMMSObject::DelChild(GCString& sID)
{
	int n = GetNChildren();
	for( int i = 0; i < n; i++ )
	{
		CMMSObject* obj = GetChild(i);
		if( obj->sID == sID )
		{
			DelChild(i);
			return;
		}
	}
}

CMMSObject* CMMSObject::FindChild(int ID, bool recursive, CPtrList* pList)
{
	int n = GetNChildren();
	int i;
	for( i = 0; i < n; i++ )
	{
		CMMSObject* obj = GetChild(i);
		if( obj->ID == ID )
		{
			if( pList != NULL ) pList->AddTail(obj);
			return obj;
		}
	}

	if( !recursive )
		return NULL;

	for( i = 0; i < n; i++ )
	{
		CMMSObject* obj = GetChild(i);
		if( obj->MMS_Type == MMS_Property ) continue;
		CMMSObject* found = obj->FindChild(ID, true, pList);
		if( found != NULL )
		{
			if( pList != NULL ) pList->AddHead(obj);
			return found;
		}
	}

	return NULL;
}

CMMSObject* CMMSObject::FindChild(GCString& sID, bool recursive)
{
	int n = GetNChildren();
	for( int i = 0; i < n; i++ )
	{
		CMMSObject* obj = GetChild(i);
		if( obj->MMS_Type != MMS_Property && obj->sID == sID )
		{
			return obj;
		}
	}

	if( recursive )
	{
		for( i = 0; i < n; i++ )
		{
			CMMSObject* obj = GetChild(i);
			if( obj->MMS_Type == MMS_Property ) continue;
			CMMSObject* found = obj->FindChild(sID, true);
			if( found != NULL )
			{
				return found;
			}
		}
	}

	return NULL;
}


static GCString _BuhName("Бухгалтерия");
void CMMSObject::BuildObjNameCache(CObjNameCache& Cache, GCString& NamePrefix, int level)
{
	int n = GetNChildren();
	int i;
	TObjNameCacheRecord new_entry;

	for( i = 0; i < n; i++ )
	{
		CMMSObject* obj = GetChild(i);
		if( obj->MMS_Type == MMS_Property ) continue;
		
		CNameTableRecord* nt_rec = NameTable.Find(obj->MMS_Type);
		GCString* CurObjName = NULL;

		new_entry.ID = obj->ID;
		new_entry.Name = NamePrefix;

		if( obj->sID == "AccSbCnts" ) //Ещё одно извращение. Эти объекты могут содержать неуникальные IDы - мы их пропустим
			continue;
		if( obj->MMS_Type == MMS_Holydays ) continue; //календарь праздников имеет фиксированный ID, и он нам в кэше не нужен

		if( obj->MMS_Type == MMS_BuhContainer ) //Вот такое вот извращение. Бухгалтерия у нас  сделана крайне нестандартно
			CurObjName = NULL;
		else if( nt_rec == NULL )
			CurObjName = &(obj->sID);
		else if( nt_rec->Type1C.IsEmpty() )
			CurObjName = NULL;
		else if( nt_rec->Type1C != "sID" )
			CurObjName = &(nt_rec->Type1C);
		else if( obj->MMS_Type == MMS_Account ) //Вот такое вот извращение. Бухгалтерия у нас  сделана крайне нестандартно
			CurObjName = &( ((CMMSAccount*)obj)->Code);
		else
			CurObjName = &(obj->sID);

		if( CurObjName != NULL && !CurObjName->IsEmpty() )
		{
			if( !NamePrefix.IsEmpty() )
				new_entry.Name += ".";
			new_entry.Name += *CurObjName;
		}


		if( obj->ID_Type == IDT_BothIDs && CurObjName != NULL && !CurObjName->IsEmpty() )
		{
			if( TaskType == TaskType_Compile )
			{
				//Если в кэше найдётся нужная запись, то возьмём ID объекта из кэша. А если не найдётся, 
				//то поместим в кэш запись с нулевым ID, и при проверке выдадим ругательство.
				TObjNameCacheRecord* cache_entry = 
					(TObjNameCacheRecord*) bsearch(&new_entry, Cache.GetData(), 
					Cache.GetSize(), sizeof(TObjNameCacheRecord),
					ObjNameCache_compare_str);
				if( cache_entry != NULL )
				{
					obj->ID = cache_entry->ID;
				}
				else
				{
					Cache.Add(new_entry);
					qsort(Cache.GetData(), Cache.GetSize(), 
						sizeof(TObjNameCacheRecord), 
						ObjNameCache_compare_str);
				}
			}
			else
			{
				Cache.Add(new_entry);
			}
		}

		obj->BuildObjNameCache(Cache, new_entry.Name, level+1);
	}
}



/*
void CMMSObject::MarkUsed(int ID)
{
	CMMSNodeList List;
	List.DestroyObjects = false;
	if( GetProperty(ID, true, &List) == NULL ) return;

	POSITION pos = List.GetHeadPosition();
	while( pos != NULL )
	{
		CMMSObject* SubNode = List.GetNext(pos);
		SubNode->Used = true;
	}
	
}

void CMMSObject::DeleteUnused()
{
	int i = GetNProps() - 1;
	while( i >= 0 )
	{
		CMMSObject* prop = GetPropByNum(i);
		if( prop->Used )
			prop->DeleteUnused();
		else if( prop->Type != MMS_Property )
			DelProperty(i);

		i--;
	}
}
*/

int PropsCompareByOrder( const void *arg1, const void *arg2 )
{
	CMMSObject* obj1 = *(CMMSObject**)arg1;
	CMMSObject* obj2 = *(CMMSObject**)arg2;

	if( obj1->Order < obj2->Order ) return -1;
	if( obj1->Order > obj2->Order ) return 1;
	return 0;
}

void CMMSObject::SortByOrder()
{
	qsort(Children.GetData(), GetNChildren(), sizeof(CMMSObject*), PropsCompareByOrder);
}

GCString CMMSObject::GetObjectDir(GCString& ParentDir)
{
	GCString Result(ParentDir);
	if( !sID.IsEmpty() )
	{
		Result += "\\";
		Result += Translit(sID);
	}
	return Result;
}

void CMMSObject::ToString(CMMSString& String)
{
	if( sID.IsEmpty() )
	{
		CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
		if( nt_rec != NULL )
			sID = nt_rec->sID;
	}

	if( String.IsEmpty() )
		String += "{";
	else
		String += "\r\n{";
	CStoredObject::ToString(String);
	String += "}";
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSAttributes methods  ////////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSAttributes::CMMSAttributes()
{
	ID_Type = IDT_OnlyTextual;
}

bool CMMSAttributes::SetValue(int MMS_Index, GCString& Value)
{
	if( MMS_Index == 0 )
		sID = Value;
	return true;
}

void CMMSAttributes::BeautyString(CBeautyString& String, int level)
{
	if( GetNChildren() == 0 && GetPropSet() == NULL )
		return;

	CBeautyString str;

	str.add_parameter(level, BeautyName(), "", "", true);
	str.add_brace(level, '{');

	if( GetPropSet() != NULL )
		CStoredObject::BeautyString(str, level+1);

	int n = GetNChildren();
	for( int i = 0; i < n; i++ )
	{
		CMMSObject* attr = GetChild(i);
		attr->BeautyString(str, level+1);
	}
	
	str.add_brace(level, '}');
	String += str;
}

const char* CMMSAttributes::MMSName()
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	if( nt_rec != NULL )
		return nt_rec->sID;
	return sID;
}

void CMMSAttributes::ToString(CMMSString& String)
{
	CMMSString str;
	bool AddComma = true;
	if( String.IsEmpty() )
		str += "{";
	else
		str += "\r\n{";

	if( MMSName() != NULL )
		str.add_quoted(MMSName(), false);
	else
		AddComma = false;

	if( GetPropSet() != NULL )
	{
		if( AddComma )
			str += ",";
		CStoredObject::ToString(str);
		AddComma = true;
	}

	int n = GetNChildren();
	for( int i = 0; i < n; i++ )
	{
		if( AddComma )
			str += ",";
		else
			AddComma = true;

		CMMSObject* attr = GetChild(i);
		attr->ToString(str);
	}
	
	str += "}";
	String += str;
}

int CMMSAttributes_SF::CountObjects()
{
	return 1;
}
////////////////////////////////////////////////////////////
//// Типизированный объект  /////////////////////////////////
////////////////////////////////////////////////////////////
void CMMSTypedObject::BeautyString(CBeautyString& String, int level)
{
	String.add_parameter(level, BeautyName(), "", NULL, true);
	String.add_brace(level, '{');

	CMMSObject::BeautyString(String, level+1);
	
	String.add_brace(level, '}');
}

////////////////////////////////////////////////////////////
//// Параметра атрибута объекта  ///////////////////////////
////////////////////////////////////////////////////////////
void CMMSObjectAttribute::BeautyString(CBeautyString& String, int level, bool NoNumericID)
{
	if( NoNumericID )
		String.add_parameter(level, sID, "", "", true);
	else
		String.add_parameter(level, sID, ID, 0, true);
	String.add_brace(level, '{');

	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet != NULL )
	{
		for( int i = 2; i < pPropSet->nProps; i++ ) //пропускаем ID и sID
			pPropSet->BeautyString(String, level+1, this, i);
	}
	
	String.add_brace(level, '}');
}

void CMMSObjectAttribute::BeautyString(CBeautyString& String, int level)
{
	BeautyString(String, level, true);
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSRefers tables and methods  /////////////////////
///////////////////////////////////////////////////////////////////////
CMMSReferList::~CMMSReferList()
{
	int i = Refers.GetSize() - 1;
	while( i >= 0 )
	{
		CMMSObjRef* ref = (CMMSObjRef*)Refers.GetAt(i);
		delete ref;
		i--;
	}
}

bool CMMSReferList::SetValue(int MMS_Index, GCString& Value)
{
	if( MMS_Index == 0 )
		sID = Value;
	else
	{
		CMMSObjRef* ref = new CMMSObjRef;
		ref->SetValue(Value);
		Refers.Add((void*)ref);
	}

	return true;
}

bool CMMSReferList::SetValue(GCString& Name, GCString& Value)
{
	if( Name.IsEmpty() )
	{
		CMMSObjRef* ref = new CMMSObjRef;
		ref->SetValue(Value);
		Refers.Add((void*)ref);
	}
	return true;
}

void CMMSReferList::BeautyString(CBeautyString& String, int level)
{
	String.add_parameter(level, BeautyName(), "", "", true);
	String.add_brace(level, '{');

	for( int i = 0; i < Refers.GetSize(); i++ )
	{
		CMMSObjRef* ref = (CMMSObjRef*)Refers.GetAt(i);
		String.add_offset(level+1);
		String += ref->BeautyString();
		String += "\r\n";
	}

	String.add_brace(level, '}');
}

void CMMSReferList::ToString(CMMSString& String)
{
	String += "\r\n{\"Refers\"";
	for( int i = 0; i < Refers.GetSize(); i++ )
	{
		CMMSObjRef* ref = (CMMSObjRef*)Refers.GetAt(i);
		String += ",\r\n{";
		String.add_quoted(ref->MMSString(), false);
		String += "}";
	}
	String += "}";
}

CMMSRefers* CMMSRefers::DefVal = new CMMSRefers;
CObjectProperty CMMSRefers::Props[] = {
	{"ID",                0, CObjectProperty::t_int, VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",     1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",       2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",           3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Ссылки",            4, CObjectProperty::t_object, VAR_OFFSET(DefVal, Refers)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSRefers::PropSet(Props, DefVal);

CMMSObject* CMMSRefers::CreateChild(const char* Name, const char* Value)
{
	Refers.ID = atoi(Value);
	return &Refers;
}

void CMMSRefers::BeautyString(CBeautyString& String, int level)
{
	if( !TaskParameters.NoDefaults )
		ID = 0;

	if( Refers.Refers.GetSize() == 0 && ID == 0 )
		return;

	if( ID == 0 )
		String.add_parameter(level, BeautyName(), "", "", true);
	else
		String.add_parameter(level, BeautyName(), ID, 0, true);
	String.add_brace(level, '{');

	Refers.BeautyString(String, level + 1);

	String.add_brace(level, '}');
}

void CMMSRefers::ToString(CMMSString& String)
{
	sID = "";
	CMMSObjectAttribute::ToString(String);
}


///////////////////////////////////////////////////////////////////////
///////////  class CMMSNamedList methods  /////////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSNamedList::CMMSNamedList()
{
	MMS_Type = MMS_Property;
	ID_Type = IDT_OnlyTextual;
}

CMMSNamedList::~CMMSNamedList()
{
	int i = GetNChildren() - 1;
	while( i >= 0 )
	{
		CStoredProperty* prop = (CStoredProperty*)PropArray.GetAt(i);
		delete prop;
		i--;
	}
}

bool CMMSNamedList::SetValue(int MMS_Index, GCString& Value)
{
	if( MMS_Index == 0 )
		sID = Value;
	else
	{
		CStoredProperty* prop = CreateProperty();
		prop->SetValue(Value);
		PropArray.Add(prop);
	}
	return true;
}

void CMMSNamedList::BeautyString(CBeautyString& String, int level)
{
	if( GetNChildren() == 0 )
		return;

	String.add_parameter(level, BeautyName(), "", "", true);
	String.add_brace(level, '{');

	int level1 = level + 1;
	for( int i = 0; i < GetNChildren(); i++ )
	{
		CStoredProperty* prop = (CStoredProperty*)(PropArray.GetAt(i));
		String.add_offset(level1);
		String += prop->BeautyString();
		String += "\r\n";
	}

	String.add_brace(level, '}');
}

void CMMSNamedList::ToString(CMMSString& String)
{
	String += "\r\n{";
	String.add_quoted(MMSName(), false);
	for( int i = 0; i < GetNChildren(); i++ )
	{
		CStoredProperty* prop = (CStoredProperty*)PropArray.GetAt(i);
		String.add_quoted(prop->MMSString(), true);
	}
	String += "}";
}

///////////////////////////////////////////////////////////////////////
///////////  class CFormDescr methods  ////////////////////////////////
///////////////////////////////////////////////////////////////////////
CFormDescr* CFormDescr::DefVal = new CFormDescr;
CObjectProperty CFormDescr::Props[] = {
	{"ID",            0, CObjectProperty::t_int, VAR_OFFSET(DefVal, ID)},
	{"Идентификатор", 1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",   2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",       3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CFormDescr::PropSet(Props, DefVal);


///////////////////////////////////////////////////////////////////////
///////////  class CMigrationRules tables and methods  ////////////////
///////////////////////////////////////////////////////////////////////
CMigrationRules* CMigrationRules::DefVal = new CMigrationRules;
CObjectProperty CMigrationRules::Props[] = {
	{"Область распространения",	1, CObjectProperty::t_prop, VAR_OFFSET(DefVal, Area)},
	{"Коды дополнительных ИБ",	2, CObjectProperty::t_string, VAR_OFFSET(DefVal, OtherBases)},
	{"Автоматическая регистрация изменений", 3, CObjectProperty::t_prop, VAR_OFFSET(DefVal, AutoRegisterChanges)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMigrationRules::PropSet(Props, DefVal);

CMigrationRules::CMigrationRules()
{
	MMS_Type = MMS_DistributionRuler;
	AutoRegisterChanges = true;
}

CMigrationRules& CMigrationRules::operator=(CMigrationRules& src)
{
	MMS_Type = src.MMS_Type;
	Area = src.Area;
	OtherBases = src.OtherBases;
	AutoRegisterChanges = src.AutoRegisterChanges;
	return *this;
}

bool CMigrationRules::operator==(CMigrationRules& Rules)
{
	return (Area.Letter == Rules.Area.Letter) && 
		(OtherBases == Rules.OtherBases) && 
		(AutoRegisterChanges == Rules.AutoRegisterChanges);
}

void CMigrationRules::BeautyString(CBeautyString& String, int level)
{
	if( (*this) == (*DefVal) )
		return;

	String.add_parameter(level, BeautyName(), "", "", true);
	String.add_brace(level, '{');

	CStoredObject::BeautyString(String, level+1);
	
	String.add_brace(level, '}');
}

void CMigrationRules::ToString(CMMSString& String)
{
	if( (*this) == (*DefVal) )
	{
		if( String[String.GetLength()-1] == ',' )
			String.Delete(String.GetLength()-1); //уберём запятую, раз сам объект не выводится
		return;
	}

	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	String += "\r\n{";
	String.add_quoted(nt_rec->sID, false);
	String += ",";
	CStoredObject::ToString(String);
	String += "}";
}

///////////////////////////////////////////////////////////////////////
///////////  class CConstant tables and methods  //////////////////////
///////////////////////////////////////////////////////////////////////
CConstant* CConstant::DefVal = new CConstant;
CObjectProperty CConstant::Props[] = {
	{"ID",                0, CObjectProperty::t_int, VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",     1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",       2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",           3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",               4, CObjectProperty::t_subobject, VAR_OFFSET(DefVal, Type)},
	{"Неотрицательный",   8, CObjectProperty::t_prop, VAR_OFFSET(DefVal, NonNegative)},
	{"Разделять триады",  9, CObjectProperty::t_prop, VAR_OFFSET(DefVal, DivideTriades)},
	{"Периодическая",    10, CObjectProperty::t_prop, VAR_OFFSET(DefVal, Periodical)},
	{"Правила миграции", 11, CObjectProperty::t_object, VAR_OFFSET(DefVal, MigrationRules)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CConstant::PropSet(Props, DefVal);

CConstant::CConstant()
{
	MMS_Type = MMS_Const;
	ID_Type = IDT_BothIDs;
	NonNegative = false;
	DivideTriades = false;
	Periodical = false;

	ToDestroyChildren = false;
}

CMMSObject* CConstant::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, MigrationRules.BeautyName()) == 0 )
		return &MigrationRules;
	return NULL;
}

CMMSObject* CConstant::CreateChild(const char* Name, int MMS_Index)
{
	if( MMS_Index == 11 )
		return &MigrationRules;
	return NULL;
}

CConstantList::CConstantList()
{
	MMS_Type = MMS_Consts;
	ID_Type = IDT_OnlyTextual;
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSRegisterList methods  //////////////////////////
///////////////////////////////////////////////////////////////////////
CRegisterProp* CRegisterProp::DefVal = new CRegisterProp;
CObjectProperty CRegisterProp::Props[] = {
	{"ID",               0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",    1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",      2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",          3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",              4, CObjectProperty::t_subobject, VAR_OFFSET(DefVal, Type)},
	{"Неотрицательный",  8, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, NonNegative)},
	{"Разделять триады", 9, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, DivideTriades)},
	{"Отбор движений",  10, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, SelectMovement)},
	{"Отбор итогов",    11, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, SelectSummaries)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CRegisterProp::PropSet(Props, DefVal);

//-----------------------------
CRegisterFigure* CRegisterFigure::DefVal = new CRegisterFigure;
CObjectProperty CRegisterFigure::Props[] = {
	{"ID",               0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",    1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",      2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",          3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",              4, CObjectProperty::t_subobject, VAR_OFFSET(DefVal, Type)},
	{"Неотрицательный",  8, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, NonNegative)},
	{"Разделять триады", 9, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, DivideTriades)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CRegisterFigure::PropSet(Props, DefVal);

//-----------------------------
CRegisterFld* CRegisterFld::DefVal = new CRegisterFld;
CObjectProperty CRegisterFld::Props[] = {
	{"ID",               0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",    1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",      2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",          3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",              4, CObjectProperty::t_subobject, VAR_OFFSET(DefVal, Type)},
	{"Неотрицательный",  8, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, NonNegative)},
	{"Разделять триады", 9, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, DivideTriades)},
	{"Отбор движений",  10, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, SelectMovement)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CRegisterFld::PropSet(Props, DefVal);

//-----------------------------
CMMSRegister* CMMSRegister::DefVal = new CMMSRegister;
CObjectProperty CMMSRegister::Props[] = {
	{"ID",             0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",  1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",    2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",        3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Оборотный",      4, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Rotation)},
	{"Периодичность",  5, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Period)},
	{"Быстрая обработка движений", 6, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, FastMovement)},
	{"Миграция движений", 7, CObjectProperty::t_prop,VAR_OFFSET(DefVal, Migration)},
	{"Измерения",      8, CObjectProperty::t_object, VAR_OFFSET(DefVal, RegProps)},
	{"Ресурсы",        9, CObjectProperty::t_object, VAR_OFFSET(DefVal, Figures)},
	{"Реквизиты",     10, CObjectProperty::t_object, VAR_OFFSET(DefVal, Flds)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSRegister::PropSet(Props, DefVal);

CMMSRegister::CMMSRegister()
{
	MMS_Type = MMS_Register;
	ToDestroyChildren = false;
}

CMMSObject* CMMSRegister::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, RegProps.BeautyName()) == 0 )
		return &RegProps;
	if( _stricoll(Name, Figures.BeautyName()) == 0 )
		return &Figures;
	if( _stricoll(Name, Flds.BeautyName()) == 0 )
		return &Flds;
	return NULL;
}

CMMSObject* CMMSRegister::CreateChild(const char* Name, int MMS_Index)
{
	switch( MMS_Index )
	{
	case 8:
		return &RegProps;
	case 9:
		return &Figures;
	case 10:
		return &Flds;
	}
	return NULL;
}


///////////////////////////////////////////////////////////////////////
///////////  class CMMSEnum methods  //////////////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSEnumVal* CMMSEnumVal::DefVal = new CMMSEnumVal;
CObjectProperty CMMSEnumVal::Props[] = {
	{"ID",            0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор", 1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",   2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",       3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Представление", 4, CObjectProperty::t_string, VAR_OFFSET(DefVal, Representation)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSEnumVal::PropSet(Props, DefVal);

CMMSEnum* CMMSEnum::DefVal = new CMMSEnum;
CObjectProperty CMMSEnum::Props[] = {
	{"ID",            0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор", 1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",   2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",       3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Значения",      4, CObjectProperty::t_object, VAR_OFFSET(DefVal, ValList)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSEnum::PropSet(Props, DefVal);
CMMSEnum::CMMSEnum()
{
	MMS_Type = MMS_Enum;
	ToDestroyChildren = false;
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSReport methods  ////////////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSReport* CMMSReport::DefVal = new CMMSReport;
CObjectProperty CMMSReport::Props[] = {
	{"ID",                      0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",           1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",             2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",                 3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSReport::PropSet(Props, DefVal);
