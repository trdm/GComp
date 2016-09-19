
#include "Declarations.h"
#include "DLL\GComp.h"
#include "MMS_BasicTypes.h"
#include "CommonFunctions.h"
#include "MMS.h"


bool CStoredObject::SetValue(int MMS_Index, GCString& Value)
{
	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet == NULL ) return false;

	return pPropSet->SetByMMSIndex(this, MMS_Index, Value);
}

bool CStoredObject::SetValue(GCString& Name, GCString& Value)
{
	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet == NULL ) return false;
	int i = pPropSet->FindByName(Name);
	if( i >= 0 )
	{
		pPropSet->SetValue(this, i, Value);
		return true;
	}

	CPropertySet& PropSet = *pPropSet;
	for( i = 0; PropSet[i].Name != NULL; i++ )
	{
		if( PropSet[i].Type == CObjectProperty::t_subobject )
		{
			CStoredObject* SubObj = (CStoredObject*)PropSet.GetObjProp(this, i);
			if( SubObj->SetValue(Name, Value) )
				return true;
		}
	}

	return false;
}

void CStoredObject::BeautyString(CBeautyString& String, int level)
{
	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet == NULL ) return;

	CPropertySet& PropSet = *pPropSet;
	for( int i = 0; i < PropSet.nProps; i++ )
		PropSet.BeautyString(String, level, this, i);
}

void CStoredObject::ToString(CMMSString& String)
{
	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet == NULL ) return;

	bool AddComma = false;
	for( int MMS_Index = 0; MMS_Index <= pPropSet->MaxMMS_Index; MMS_Index++ )
	{
		int i = pPropSet->FindByMMSIndex(MMS_Index);
		if( i >= 0 )
		{
			pPropSet->ToString(String, this, i, AddComma);
			AddComma = true;
		}
	}
}

void CPropertySet::add_flags(CBeautyString& String, int level, const char* id, unsigned int Flags, char* FlagNames[])
{
	GCString flags;
	bool delimit = false;
	int bit = 0;
	unsigned int bitmask = 1;

	while( bit < 32 )
	{
		if( Flags & bitmask )
		{
			if( delimit )
				flags += ", ";
			flags += FlagNames[bit];
			delimit = true;
		}
		bit++;
		bitmask <<= 1;
	}

	String.add_parameter(level, id, flags, "");
}

/////////////////////////////////////////////////////////////////
//////  CPropertySet  //////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
CPropertySet::CPropertySet(CObjectProperty* Props, CStoredObject* Defaults)
{
	Propertys = Props;
	DefaultValues = Defaults;

	//рассчитаем смещения
	nProps = 0;
	MaxMMS_Index = 0;
	CObjectProperty* prop = Props;
	while( prop->Name != NULL )
	{
		MaxMMS_Index = max(MaxMMS_Index, prop->MMS_Index);
		prop++;
		nProps++;
	}
}

CPropertySet::~CPropertySet()
{
	if( DefaultValues != NULL ) delete DefaultValues;
}

void* CPropertySet::GetObjProp(void* Object, int i)
{
	CObjectProperty& prop = Propertys[i];
	return (char*)Object + prop.Offset;
}

int CPropertySet::FindByName(const char* Name)
{
	CObjectProperty* prop = Propertys;
	int i = 0;
	while( prop->Name != NULL )
	{
		if( _stricoll(prop->Name, Name) == 0 )
			return i;
		prop++;
		i++;
	}

	return -1;
}

int CPropertySet::FindByMMSIndex(int Index)
{
	CObjectProperty* prop = Propertys;
	int i = 0;
	while( prop->Name != NULL )
	{
		if( prop->MMS_Index == Index )
			return i;
		prop++;
		i++;
	}
	return -1;
}

bool CPropertySet::SetByMMSIndex(void* Object, int MMS_Index, GCString& Value, bool Unquote)
{
	CObjectProperty* prop = Propertys;
	int i = 0;
	while( prop->Name != NULL )
	{
		if( prop->Type == CObjectProperty::t_subobject )
		{
			CStoredObject* obj = (CStoredObject*) ((char*)Object + prop->Offset);
			CPropertySet* PropSet = obj->GetPropSet();
			if( PropSet != NULL )
			{
				bool res = PropSet->SetByMMSIndex(obj, MMS_Index - prop->MMS_Index, Value, Unquote);
				if( res )
					return true;
			}
		}
		else
		{
			if( prop->MMS_Index == MMS_Index )
			{
				SetValue(Object, i, Value, Unquote);
				return true;
			}
		}
		prop++;
		i++;
	}
	return false;
}

void CPropertySet::SetValue(void* Object, int i, GCString& Value, bool Unquote)
{
	CObjectProperty& prop = Propertys[i];

	switch( prop.Type )
	{
	case CObjectProperty::t_quotstring:
		{
		GCString* str_prop_val = (GCString*) ((char*)Object + prop.Offset );
		*str_prop_val = Value;
		}
		break;
	case CObjectProperty::t_string:
		{
		GCString* str_prop_val = (GCString*) ((char*)Object + prop.Offset );
		Value.TrimRight();
		*str_prop_val = Value;
		}
		break;
	case CObjectProperty::t_int:
		{
		int* int_prop_val = (int*) ((char*)Object + prop.Offset);
		*int_prop_val = atoi(Value);
		}
		break;
	case CObjectProperty::t_uint:
		{
		unsigned int* uint_prop_val = (unsigned int*) ((char*)Object + prop.Offset);
		char *stopstring;
		*uint_prop_val = strtol(Value, &stopstring, 0); //Undersatnd 0xNNNN format
		}
		break;
	case CObjectProperty::t_bool:
		{
		bool* bool_prop_val = (bool*) ((char*)Object + prop.Offset);
		*bool_prop_val = StrToBool(Value);
		}
		break;
	case CObjectProperty::t_subobject:
	case CObjectProperty::t_object:
		{
			CStoredObject* obj = (CStoredObject*) ((char*)Object + prop.Offset);
			obj->SetValue(GCString(prop.Name), Value);
		}
		break;
	case CObjectProperty::t_prop:
		{
			CStoredProperty* obj = (CStoredProperty*) ((char*)Object + prop.Offset);
			obj->SetValue(Value);
		}
		break;
	default:
		Msg(0, "ERR: Непредусмотренный тип свойства %i (%s)!", (int)prop.Type, (LPCSTR)prop.Name);
		throw NULL;
		break;
	}
}

void CPropertySet::BeautyString(CBeautyString& String, int level, const void* Object, int i)
{
	CObjectProperty& prop = Propertys[i];
	
	switch( prop.Type )
	{
	case CObjectProperty::t_quotstring:
		{
		GCString* prop_val = (GCString*) ((char*)Object + prop.Offset );
		GCString* def_val  = (GCString*) ((char*)DefaultValues + prop.Offset );
		GCString quoted_val, quoted_defval;
		quoted_val.Format("\"%s\"", *prop_val);
		quoted_val.quote_linefeeds();
		quoted_defval.Format("\"%s\"", *def_val);
		String.add_parameter(level, prop.Name, quoted_val, quoted_defval, TaskParameters.NoDefaults);
		}
		break;
	case CObjectProperty::t_string:
		{
		GCString* prop_val = (GCString*) ((char*)Object + prop.Offset );
		GCString* def_val  = (GCString*) ((char*)DefaultValues + prop.Offset );
		prop_val->quote_linefeeds();
		String.add_parameter(level, prop.Name, *prop_val, *def_val, TaskParameters.NoDefaults);
		}
		break;
	case CObjectProperty::t_int:
		{
		int* prop_val = (int*) ((char*)Object + prop.Offset);
		int* def_val  = (int*) ((char*)DefaultValues + prop.Offset );
		String.add_parameter(level, prop.Name, *prop_val, *def_val, TaskParameters.NoDefaults);
		}
		break;
	case CObjectProperty::t_uint:
		{
		unsigned int* prop_val = (unsigned int*) ((char*)Object + prop.Offset);
		unsigned int* def_val  = (unsigned int*) ((char*)DefaultValues + prop.Offset );
		String.add_parameter(level, prop.Name, *prop_val, *def_val, TaskParameters.NoDefaults);
		}
		break;
	case CObjectProperty::t_bool:
		{
		bool* prop_val = (bool*) ((char*)Object + prop.Offset);
		bool* def_val  = (bool*) ((char*)DefaultValues + prop.Offset );
		String.add_parameter(level, prop.Name, *prop_val, *def_val, TaskParameters.NoDefaults);
		}
		break;
	case CObjectProperty::t_subobject:
	case CObjectProperty::t_object:
		{
			CStoredObject* obj = (CStoredObject*) ((char*)Object + prop.Offset);
			obj->BeautyString(String, level);
		}
		break;
	case CObjectProperty::t_prop:
		{
			CStoredProperty* val = (CStoredProperty*) ((char*)Object + prop.Offset);
			CStoredProperty* def_val  = (CStoredProperty*) ((char*)DefaultValues + prop.Offset );
			GCString str = val->BeautyString();
			if( TaskParameters.NoDefaults)
				String.add_parameter(level, prop.Name, val->BeautyString(), NULL, true);
			else
				String.add_parameter(level, prop.Name, val->BeautyString(), def_val->BeautyString(), false);
		}
		break;
	default:
		Msg(0, "ERR: Непредусмотренный тип свойства %i (%s)!", (int)prop.Type, prop.Name);
		throw NULL;
		break;
	}
}

void CPropertySet::ToString(CMMSString& String, const void* Object, int i, bool AddComma)
{
	CObjectProperty& prop = Propertys[i];
	switch( prop.Type )
	{
	case CObjectProperty::t_quotstring:
		{
		GCString* prop_val = (GCString*) ((char*)Object + prop.Offset );
		prop_val->unquote_linefeeds();
		if( prop_val->GetLength() >= 2 )
			String.add_quoted(prop_val->Mid(1, prop_val->GetLength()-2), AddComma);
		else
			String.add_quoted(*prop_val, AddComma);
		}
		break;
	case CObjectProperty::t_string:
		{
		GCString* prop_val = (GCString*) ((char*)Object + prop.Offset );
		prop_val->unquote_linefeeds();
		String.add_quoted(*prop_val, AddComma);
		}
		break;
	case CObjectProperty::t_int:
		{
		int* prop_val = (int*) ((char*)Object + prop.Offset);
		String.add_quoted(*prop_val, AddComma);
		}
		break;
	case CObjectProperty::t_uint:
		{
		unsigned int* prop_val = (unsigned int*) ((char*)Object + prop.Offset);
		String.add_quoted(*prop_val, AddComma);
		}
		break;
	case CObjectProperty::t_bool:
		{
		bool* prop_val = (bool*) ((char*)Object + prop.Offset);
		String.add_quoted(*prop_val, AddComma);
		}
		break;
	case CObjectProperty::t_subobject:
	case CObjectProperty::t_object:
		{
			CStoredObject* obj = (CStoredObject*) ((char*)Object + prop.Offset);
			if( AddComma )
				String += ",";
			obj->ToString(String);
		}
		break;
	case CObjectProperty::t_prop:
		{
			CStoredProperty* val = (CStoredProperty*) ((char*)Object + prop.Offset);
			String.add_quoted(val->MMSString(), AddComma);
		}
		break;
	default:
		Msg(0, "ERR: Непредусмотренный тип свойства %i (%s)!", (int)prop.Type, (LPCSTR)prop.Name);
		throw NULL;
		break;
	}
}


//===================================================================================
//======    CStoredBool  ============================================================
//===================================================================================
void CStoredBool::SetValue(GCString& NewValue)
{
	if( NewValue.IsNumber() )
		Value = atoi(NewValue) != 0;
	else
	{
		if( NewValue.CollateNoCase("Да") == 0 )
			Value = true;
		else if( NewValue.CollateNoCase("Yes") == 0 )
			Value = true;
		else
			Value = false;
	}
}

GCString CStoredBool::BeautyString()
{
	if( Value )
		return "Да";
	else
		return "Нет";
}

GCString CStoredBool::MMSString()
{
	if( Value )
		return "1";
	else
		return "0";
}

//===================================================================================
//======    CFontBold  ==============================================================
//===================================================================================
void CFontBold::SetValue(GCString& NewValue)
{
	if( (NewValue.CollateNoCase("700") == 0) || 
		(NewValue.CollateNoCase("Да") == 0) || 
		NewValue.CollateNoCase("Yes") == 0 )
	{
		Value = true;
	}
	else
	{
		Value = false;
	}
}

GCString CFontBold::BeautyString()
{
	if( Value )
		return "Да";
	else
		return "Нет";
}

GCString CFontBold::MMSString()
{
	if( !Needed )
		return "0";

	if( Value )
		return "700";
	else
		return "400";
}


//===================================================================================
//======  CStoredBitSet  ============================================================
//===================================================================================
int CStoredBitSet::BitByName(const char* BitName)
{
	int i = 0;
	while( i < 32 )
	{
		if( BitNames[i] != NULL && _stricoll(BitName, BitNames[i]) == 0 )
				return i;
		i++;
	}
	return -1;
}

bool CStoredBitSet::BitValue(int i)
{
	return (Value & (1 << i)) != 0;
}

void CStoredBitSet::SetValue(GCString& NewValue)
{
	if( NewValue.IsNumber() )
	{
		Value = atoi(NewValue);
	}
	else
	{
		Value = 0;
		while( !NewValue.IsEmpty() )
		{
			GCString Name = NewValue.SpanExcluding(", \t");
			if( !Name.IsEmpty() )
			{
				int bit = BitByName(Name);
				if( bit < 0 )
				{
					Msg(0, "ERR: Неизвестный флаг '%s'", Name);
					throw NULL;
				}
				Value |= 1 << bit;
			}
			else
			{
				Name = NewValue.SpanIncluding(", \t");
			}
			NewValue.Delete(0, Name.GetLength());
		}
	}
}

GCString CStoredBitSet::BeautyString()
{
	GCString flags;
	bool delimit = false;
	int bit = 0;
	unsigned int bitmask = 1;

	while( bit < 32 )
	{
		if( Value & bitmask )
		{
			if( delimit )
				flags += ", ";
			flags += BitNames[bit];
			delimit = true;
		}
		bit++;
		bitmask <<= 1;
	}

	return flags;
}

GCString CStoredBitSet::MMSString()
{
	return GCString(Value);
}

//===================================================================================
//======  CStoredSet  ===============================================================
//===================================================================================
void CStoredSet::SetValue(GCString& NewValue)
{
	if( NewValue.IsNumber() )
	{
		Value = atoi(NewValue);
	}
	else
	{
		TSetPair* Set = ValTable;
		while( Set->representation != NULL )
		{
			if( NewValue.CollateNoCase(Set->representation) == 0 )
			{
				Value = Set->value;
				break;
			}
			Set++;
		}
	}
}

GCString CStoredSet::BeautyString()
{
	TSetPair* Set = ValTable;
	while( Set->representation != NULL )
	{
		if( Set->value == Value )
		{
			return Set->representation;
		}
		Set++;
	}
	return GCString(Value);
}

GCString CStoredSet::MMSString()
{
	return GCString(Value);
}

//===================================================================================
//======    CMMSObjRef   ============================================================
//===================================================================================
void CMMSObjRef::SetValue(GCString& Value)
{
	sTypeObjectID = Value;

	if( Value.IsNumber() )
		iTypeObjectID = atoi(Value);
	else
		iTypeObjectID = 0;
}

GCString CMMSObjRef::BeautyString()
{
	if( iTypeObjectID != 0 )
	{
		sTypeObjectID = MainMetadataStream->ObjectIDtoName(iTypeObjectID);
		if( sTypeObjectID.IsEmpty() )
		{
			//раз не нашлось в метаданных, то просто вернём циферки как есть
			char str[32];
			sTypeObjectID = itoa(iTypeObjectID, str, 10);
		}
	}
	else
		sTypeObjectID = "";
	return sTypeObjectID;
}

GCString CMMSObjRef::MMSString()
{
	if( sTypeObjectID.Find('.') >= 0 )
		iTypeObjectID = MainMetadataStream->ObjectNameToID(sTypeObjectID);
	else
		iTypeObjectID = atoi(sTypeObjectID);
	return iTypeObjectID;
}

//===================================================
//======  CMMSTypeLetter  ===========================
//===================================================
CMMSTypeLetter::TTypeTable CMMSTypeLetter::type_table[] = {
	{'U', "Неопределенный"},
	{ 0,  "Неопределенный"},
	{'N', "Число"},
	{ 1,  "Число"},
	{'S', "Строка"},
	{ 2,  "Строка"},
	{'D', "Дата"},
	{ 3,  "Дата"},
	{'E', "Перечисление"},
	{ 10, "Перечисление"},
	{'B', "Справочник"},
	{ 11, "Справочник"},
	{'O', "Документ"},
	{ 12, "Документ"},
	{'C', "Календарь"},
	{ 13, "Календарь"},
	{'A', "ВидРасчета"},
	{ 14, "ВидРасчета"},
	{'T', "Счет"},
	{ 15, "Счет"},
	{'K', "ВидСубконто"},
	{ 16, "ВидСубконто"},
	{'P', "ПланСчетов"},
	{ 17, "ПланСчетов"},
	{0, NULL}
};

void CMMSTypeLetter::SetValue(GCString& Value)
{
	TypeLetter = Value;
}

GCString CMMSTypeLetter::BeautyString()
{
	struct TTypeTable* table = type_table;
	char letter;
	if( TypeLetter.IsNumber() )
		letter = atoi(TypeLetter);
	else
		letter = TypeLetter[0];

	while( table->type != NULL )
	{
		if( table->letter == letter )
			return table->type;
		table++;
	}
	return TypeLetter;
}

GCString CMMSTypeLetter::MMSString()
{
	if( Numeric )
		return AsNumber();

	struct TTypeTable* table;
	for( table = type_table; table->type != NULL; table++ )
	{
		if( TypeLetter == table->type )
			return table->letter;
		if( table->letter == TypeLetter[0] )
			return table->letter;
	}
	return TypeLetter;
}

GCString CMMSTypeLetter::AsNumber()
{
	struct TTypeTable* table;
	for( table = type_table; table->type != NULL; table++ )
	{
		if( table->letter > 17 ) continue;

		if( TypeLetter == table->type )
			return GCString((int)(table->letter));
		if( table->letter == TypeLetter[0] )
			return GCString((int)(table->letter));
	}
	return "0";
}

static CMMSAttributeType* DefAttrType = new CMMSAttributeType;
CObjectProperty CMMSAttributeType::Props[] = {
	{"Тип",            0, CObjectProperty::t_prop, VAR_OFFSET(DefAttrType, TypeLetter)},
	{"Длина",          1, CObjectProperty::t_int,  VAR_OFFSET(DefAttrType, Length)},
	{"Точность",       2, CObjectProperty::t_int,  VAR_OFFSET(DefAttrType, Precision)},
	{"Агрегатный тип", 3, CObjectProperty::t_prop, VAR_OFFSET(DefAttrType, TypeObjectID)},
	{NULL, -1, CObjectProperty::t_int, 0}
};

CPropertySet CMMSAttributeType::PropSet(Props, DefAttrType);

void CMMSAttributeType::BeautyString(CBeautyString& String, int level)
{
	if( NeedType )
		CStoredObject::BeautyString(String, level);
}


///////////////////////////////////////////////////
void CMigrationArea::SetValue(GCString& Value)
{
	if( Value.GetLength() == 1 )
	{
		Letter = Value;
	}
	else
	{
		if( Value.CollateNoCase("все информационные базы") == 0 )
			Letter = 'W';
		else if( Value.CollateNoCase("место создания") == 0 )
			Letter = 'L';
		else if( Value.CollateNoCase("место создания и центр") == 0 )
			Letter = 'M';
		else
		{
			Msg(0, "Неизвестный тип миграции: '%s'", (LPCSTR)Value);
			throw NULL;
		}
	}
}

GCString CMigrationArea::BeautyString()
{
	switch( Letter[0] )
	{
	case 'W':
		return "Все информационные базы";
	case 'L':
		return "Место создания";
	case 'M':
		return "Место создания и центр";
	}

	Msg(0, "Неизвестный тип миграции: '%s'", (LPCSTR)Letter);
	throw NULL;
}

GCString CMigrationArea::MMSString()
{
	return Letter;
}

///////////////////////////////////////////////////
void CPeriod::SetValue(GCString& Value)
{
	if( Value.GetLength() == 1 )
	{
		PeriodLetter = Value;
	}
	else
	{
		if( Value.IsEmpty() )
			PeriodLetter = ' ';
		else if( Value.CollateNoCase("год") == 0 )
			PeriodLetter = 'Y';
		else if( Value.CollateNoCase("квартал") == 0 )
			PeriodLetter = 'Q';
		else if( Value.CollateNoCase("месяц") == 0 )
			PeriodLetter = 'M';
		else if( Value.CollateNoCase("декада") == 0 )
			PeriodLetter = 'C';
		else if( Value.CollateNoCase("неделя") == 0 )
			PeriodLetter = 'W';
		else if( Value.CollateNoCase("день") == 0 )
			PeriodLetter = 'D';
		else
		{
			Msg(0, "Неизвестный тип периода: '%s'", (LPCSTR)Value);
			throw NULL;
		}
	}
}

GCString CPeriod::BeautyString()
{
	switch( PeriodLetter[0] )
	{
	case ' ':
		return "";
	case 'Y':
		return "Год";
	case 'Q':
		return "Квартал";
	case 'M':
		return "Месяц";
	case 'C':
		return "Декада";
	case 'W':
		return "Неделя";
	case 'D':
		return "День";
	}

	Msg(0, "Неизвестный тип периода: '%s'", (LPCSTR)PeriodLetter);
	throw NULL;
}

GCString CPeriod::MMSString()
{
	return PeriodLetter;
}
