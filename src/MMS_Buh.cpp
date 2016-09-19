
#include "Declarations.h"
#include "DLL\GComp.h"
#include "CommonFunctions.h"
#include "Storage.h"
#include "MMS_Filter.h"
#include "NameTable.h"
#include "Ordering.h"
#include "MMS_Buh.h"
#include "MMS.h"

///////////////////////////////////////////////////////////////////////
///////////  class CMMSAccount methods  ///////////////////////////////
///////////////////////////////////////////////////////////////////////
CAccountSubconto* CAccountSubconto::DefVal = new CAccountSubconto;
CObjectProperty CAccountSubconto::Props[] = {
	{"ID",                0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",     1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",       2, CObjectProperty::t_string, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",           3, CObjectProperty::t_string, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",               4, CObjectProperty::t_subobject,    VAR_OFFSET(DefVal, Type)},
	{"Param9",            8, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, Param9)},
	{"Param10",           9, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, Param10)},
	{"Вид субконто",     10, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SbKind)},
	{"Только обороты",   11, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, OnlyMovement)},
	{"Учёт по сумме",    12, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, BySum)},
	{"Учёт по валютной сумме", 13, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, ByCurrencySum)},
	{"Учёт по количеству", 14, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, ByAmount)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CAccountSubconto::PropSet(Props, DefVal);

void CAccountSubconto::BeautyString(CBeautyString& String, int level)
{
	CMMSObjectAttribute::BeautyString(String, level, false);
}

//--------------------------------------------------------------
CMMSAccount* CMMSAccount::DefVal = new CMMSAccount;
CObjectProperty CMMSAccount::Props[] = {
	{"ID",             0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Код",            4, CObjectProperty::t_string, VAR_OFFSET(DefVal, Code)},
	{"Идентификатор",  1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Наименование",   2, CObjectProperty::t_string, VAR_OFFSET(DefVal, Name)},
	{"Param2",         3, CObjectProperty::t_string, VAR_OFFSET(DefVal, Param2)},
	{"Валютный",       5, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, IsCurrency)},
	{"Количественный", 6, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, IsAmount)},
	{"Забалансовый",   7, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, IsSingle)},
	{"Это группа",     8, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, IsGroup)},
	{"Активный",       9, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, IsActive)},
	{"Субконто",      10, CObjectProperty::t_object,   VAR_OFFSET(DefVal, SubcontoList)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSAccount::PropSet(Props, DefVal);

static CStoredSet::TSetPair Account_IsActive[] = {
	{1, "Активный"},
	{2, "Пассивный"},
	{3, "Активно-пассивный"},
	{0, NULL}
};

CMMSAccount::CMMSAccount()
{
	MMS_Type = MMS_Account;
	ID_Type = IDT_BothIDs;

	IsActive = 0;
	IsActive.SetTable(Account_IsActive);

	ToDestroyChildren = false;
}

void CMMSAccount::BeautyString(CBeautyString& String, int level)
{
	String.add_parameter(level, Code, "", "", true);
	String.add_brace(level, '{');

	CPropertySet* pPropSet = GetPropSet();
	if( pPropSet != NULL )
	{
		for( int i = 2; i < pPropSet->nProps; i++ ) //пропускаем ID и Code
			pPropSet->BeautyString(String, level+1, this, i);
	}
	
	String.add_brace(level, '}');
}

void CMMSAccount::SetIDs(const char* Name, const char* Value)
{
	Code = Name;
	ID = atoi(Value);
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSAccountPlan methods  ///////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSAccountPlan* CMMSAccountPlan::DefVal = new CMMSAccountPlan;
CObjectProperty CMMSAccountPlan::Props[] = {
	{"ID",            0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор", 1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",   2, CObjectProperty::t_string, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",       3, CObjectProperty::t_string, VAR_OFFSET(DefVal, Synonym)},
	{"Маска счёта 1",   4, CObjectProperty::t_string, VAR_OFFSET(DefVal, AccountMask1)},
	{"Маска счёта", 5, CObjectProperty::t_string, VAR_OFFSET(DefVal, AccountMask2)},
	{"Accounts",      6, CObjectProperty::t_object, VAR_OFFSET(DefVal, Accounts)},
	{"MigrationRules", 7, CObjectProperty::t_object, VAR_OFFSET(DefVal, Migration)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSAccountPlan::PropSet(Props, DefVal);

CMMSObject* CMMSAccountPlan::CreateChild(const char* Name, int MMS_Index)
{
	if( Name != NULL )
	{
		if( _stricoll(Name, Accounts.BeautyName()) == 0 )
			return &Accounts;
		if( _stricoll(Name, Migration.BeautyName()) == 0 )
			return &Migration;
	}

	if( AccMaskAmount == 0 )
	{
		if( MMS_Index == 5 )
			AccMaskAmount = 1;
		else
			AccMaskAmount = 2;
	}

	if( AccMaskAmount == 1 )
	{
		switch( MMS_Index )
		{
			case 5: return &Accounts;
			case 6: return &Migration;
		}
	}
	else
	{
		switch( MMS_Index )
		{
			case 6: return &Accounts;
			case 7: return &Migration;
		}
	}

	return NULL;
}

CMMSAccountPlan::CMMSAccountPlan()
{
	MMS_Type = MMS_Plan;
	ID_Type = IDT_BothIDs;

	AccMaskAmount = 0;

	ToDestroyChildren = false;
}

void CMMSAccountPlan::BeautyString(CBeautyString& String, int level)
{
	if( AccountMask2.IsEmpty() ) AccountMask2 = AccountMask1;

	CPropertySet* pPropSet = GetPropSet();

	String.add_parameter(level, sID, "", "", true);
	String.add_brace(level, '{');
	CPropertySet& PropSet = *pPropSet;
	for( int i = 2; i < PropSet.nProps; i++ )
	{
		CObjectProperty& Prop = PropSet[i];
		if( TaskParameters.CompoundType != external_report && Prop.MMS_Index == 6 )
			continue; //не выводим счета, если это МДшник - они будут в отдельном каталоге
		else if( Prop.MMS_Index == 4 )
			continue; //Пока у нас ММС приводится к формату 10009 будем насильно оставлять первую маску пустой

		PropSet.BeautyString(String, level+1, this, i);
	}
	String.add_brace(level, '}');
}

void CMMSAccountPlan::ToString(CMMSString& String)
{
	if( AccountMask2.IsEmpty() ) AccountMask2 = AccountMask1;

	String += "\r\n{";

	CPropertySet& PropSet = *GetPropSet();
	bool AddComma = false;
	for( int MMS_Index = 0; MMS_Index <= PropSet.MaxMMS_Index; MMS_Index++ )
	{
		int i = PropSet.FindByMMSIndex(MMS_Index);
		if( i >= 0 )
		{
			if( PropSet[i].MMS_Index == 4 )
				continue; //Пока у нас ММС приводится к формату 10009 будем насильно оставлять первую маску пустой
			PropSet.ToString(String, this, i, AddComma);
			AddComma = true;
		}
	}

	String += "}";
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSAccountPlans methods  ///////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSAccountPlanList::CMMSAccountPlanList()
{
	MMS_Type = MMS_Plans;
}

//-------------------------------------------------------------------------
CMMSBuhSbKindRepr* CMMSBuhSbKindRepr::DefVal = new CMMSBuhSbKindRepr;
CObjectProperty CMMSBuhSbKindRepr::Props[] = {
	{"Поле",           0, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Field)},
	{"Заголовок",      1, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Name)},
	{"С новой строки", 2, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, FromNewLine)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSBuhSbKindRepr::PropSet(Props, DefVal);

void CMMSBuhSbKindReprList::ToString(CMMSString& String)
{
	String += "\r\n{";

	int n = GetNChildren();
	for( int i = 0; i < n; i++ )
	{
		CMMSObject* attr = GetChild(i);
		if( i > 0 )
			String += ",";
		attr->ToString(String);
	}
	
	String += "}";
}

CMMSBuhSbKind* CMMSBuhSbKind::DefVal = new CMMSBuhSbKind;
CObjectProperty CMMSBuhSbKind::Props[] = {
	{"ID",                0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",     1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",       2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",           3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",               4, CObjectProperty::t_subobject,    VAR_OFFSET(DefVal, Type)},
	{"Разделять триады",  8, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, DivideTriades)},
	{"Неотрицательный",   9, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, NonNegative)},
	{"Отбор",            10, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Selection)},
	{"Цена",             11, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Price)},
	{"Валютная цена",    12, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, CurrencyPrice)},
	{"Представление",    13, CObjectProperty::t_object,   VAR_OFFSET(DefVal, Representation)},
	{"Быстрый выбор",    14, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, FastSelection)},
	{"Ввод пустых субконто", 15, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, AllowEmpty)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSBuhSbKind::PropSet(Props, DefVal);

CMMSObject* CMMSBuhSbKind::CreateChild(const char* Name, int MMS_Index)
{
	if( Name != NULL )
	{
		if( _stricoll(Name, Representation.BeautyName()) == 0 )
			return &Representation;
	}

	switch( MMS_Index )
	{
		case 13: return &Representation;
		//case 14: return &FastSelection;
		//case 15: return &AllowEmpty;
	}
	return NULL;
}

//-------------------------------------------------------------------------
CMMSTypOper* CMMSTypOper::DefVal = new CMMSTypOper;
CObjectProperty CMMSTypOper::Props[] = {
	{"ID",               0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"sID",              1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",      2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",          3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Правила миграции", 4, CObjectProperty::t_object, VAR_OFFSET(DefVal, MigrationRules)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSTypOper::PropSet(Props, DefVal);

void CMMSTypOper::BeautyString(CBeautyString& String, int level)
{
	String.add_brace(level, '{');

	CMMSSynonymedObject::BeautyString(String, level + 1);

	String.add_brace(level, '}');
}

void CMMSTypOperList::ToString(CMMSString& String)
{
	if( GetNChildren() == 0 )
		AddChild(CreateChild(NULL, -1));
	CMMSAttributes::ToString(String);
}

void CMMSCorrProvsDefList::ToString(CMMSString& String)
{
	if( GetNChildren() == 0 )
		AddChild(CreateChild(NULL, -1));
	CMMSAttributes::ToString(String);
}

void CMMSProvHardFldDefList::ToString(CMMSString& String)
{
	while( GetNChildren() < 2 )
		AddChild(CreateChild(NULL, -1));

	CMMSAttributes::ToString(String);
}


//-------------------------------------------------------------------------
CMMSTypedField* CMMSTypedField::DefVal = new CMMSTypedField;
CObjectProperty CMMSTypedField::Props[] = {
	{"ID",                0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",     1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",       2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",           3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",               4, CObjectProperty::t_subobject,    VAR_OFFSET(DefVal, Type)},
	{"Неотрицательный",   8, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, NonNegative)},
	{"Разделять триады",  9, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, DivideTriades)},
	{"Отбор",            10, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Selection)},
	{"Периодический",    11, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Periodical)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSTypedField::PropSet(Props, DefVal);

//-------------------------------------------------------------------------
CMMSTypedDocField* CMMSTypedDocField::DefVal = new CMMSTypedDocField;
CObjectProperty CMMSTypedDocField::Props[] = {
	{"ID",                0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",     1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",       2, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",           3, CObjectProperty::t_quotstring, VAR_OFFSET(DefVal, Synonym)},
	{"Тип",               4, CObjectProperty::t_subobject,    VAR_OFFSET(DefVal, Type)},
	{"Разделять триады",  8, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, DivideTriades)},
	{"Неотрицательный",   9, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, NonNegative)},
	{"Отбор",            10, CObjectProperty::t_prop,   VAR_OFFSET(DefVal, Selection)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSTypedDocField::PropSet(Props, DefVal);


///////////////////////////////////////////////////////////////////////
///////////  class CMMSOperJournalList methods  ///////////////////////
///////////////////////////////////////////////////////////////////////
void CMMSOperJournalList::ToString(CMMSString& String)
{
	if( GetNChildren() == 0 )
		AddChild(CreateChild(NULL, -1));
	CMMSAttributes::ToString(String);
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSBuh methods  ///////////////////////////////////
///////////////////////////////////////////////////////////////////////
CMMSBuh* CMMSBuh::DefVal = new CMMSBuh;
CObjectProperty CMMSBuh::Props[] = {
	{"ID",                0, CObjectProperty::t_int,    VAR_OFFSET(DefVal, ID)},
	{"Идентификатор",     1, CObjectProperty::t_string, VAR_OFFSET(DefVal, sID)},
	{"Комментарий",       2, CObjectProperty::t_string, VAR_OFFSET(DefVal, Comment)},
	{"Синоним",           3, CObjectProperty::t_string, VAR_OFFSET(DefVal, Synonym)},
	{"Редактировать",     4, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, EditWay)},
	{"Форма для выбора",  5, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, ChoosingListForm)},
	{"Основная форма",    6, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, MainListForm)},
	{"Длина наименования счета", 7, CObjectProperty::t_int,    VAR_OFFSET(DefVal, AccountNameLen)},
	{"Длина кода счета", 8, CObjectProperty::t_int,    VAR_OFFSET(DefVal, AccountCodeLen)},
	{"Максимальное количествово субконто", 9, CObjectProperty::t_int,    VAR_OFFSET(DefVal, MaxSubcontoNum)},
	{"Длина содержания операции", 10, CObjectProperty::t_int,    VAR_OFFSET(DefVal, OperCommentLen)},
	{"Длина суммы операции", 11, CObjectProperty::t_int,    VAR_OFFSET(DefVal, OperSumLen)},
	{"Точность суммы операции", 12, CObjectProperty::t_int,    VAR_OFFSET(DefVal, OperSumPrec)},
	{"Длина суммы проводки", 13, CObjectProperty::t_int,    VAR_OFFSET(DefVal, EntrySumLen)},
	{"Точность суммы проводки", 14, CObjectProperty::t_int,    VAR_OFFSET(DefVal, EntrySumPrec)},
	{"Длина валютной суммы проводки", 15, CObjectProperty::t_int,    VAR_OFFSET(DefVal, EntryCurrencySumLen)},
	{"Точность валютной суммы проводки", 16, CObjectProperty::t_int,    VAR_OFFSET(DefVal, EntryCurrencySumPrec)},
	{"Длина количества проводки", 17, CObjectProperty::t_int,    VAR_OFFSET(DefVal, EntryAmountLen)},
	{"Точность количества проводки", 18, CObjectProperty::t_int,    VAR_OFFSET(DefVal, EntryAmountPrec)},
	{"Param20",               19, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Param20)},
	{"Param21",               20, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Param21)},
	{"Param22",               21, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Param22)},
	{"Param23",               22, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Param23)},
	{"Справочник валют", 23, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, Currency_Subconto)},
	{"Реквизит курс", 24, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, Currency_Rate)},
	{"Param26",      25, CObjectProperty::t_string,    VAR_OFFSET(DefVal, Param26)},
	{"Отбор по сумме операции", 26, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectByOperSum)},
	{"Отбор по содержанию операции", 27, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectByOperComment)},
	{"Расчет суммы операции", 28, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, CalcOperSum)},
	{"Разрешить отбор проводок", 29, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, EnableEntrySelect)},
	{"Количество уровней для отбора проводок", 30, CObjectProperty::t_int,    VAR_OFFSET(DefVal, LevelAmountForSelection)},
	{"Разделять триады сумма операции", 31, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, OperSum_DivideTriades)},
	{"Разделять триады сумма проводки", 32, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, EntrySum_DivideTriades)},
	{"Разделять триады валютной суммы проводки", 33, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, EntryCurrSum_DivideTriades)},
	{"Разделять триады количества проводки", 34, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, EntryAmount_DivideTriades)},
	{"Отбор по сумме проводки", 35, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectByEntrySum)},
	{"Отбор по валюте проводки", 36, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectByEntryCurr)},
	{"Отбор по валютной сумме проводки", 37, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectByEntryCurrSum)},
	{"Отбор по количеству проводки", 38, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectByEntryAmount)},
	{"Кратность", 39, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, Currency_Multiple)},
	{"Сложные проводки", 40, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, EnableComplexEntries)},
	{"Выбор режима расчета суммы", 41, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, ToChooseCalcSumMode)},
	{"Отбор проводок по Дебету/Кредиту", 42, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectEntryByDebCred)},
	{"Редактировать операций документов", 43, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, DocOperEditMode)},
	{"Отбор по планам счетов проводки", 44, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, SelectByEntryPlan)},
	{"Количичественный учет только по аналитике", 45, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, AmountOnlyByAnalytics)},
	{"Разделитель учета", 46, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, AccountingDividor)},
	{"Основной план счетов", 47, CObjectProperty::t_prop,    VAR_OFFSET(DefVal, MainAccPlan)},
	{"Param49", 48, CObjectProperty::t_string,    VAR_OFFSET(DefVal, Param49)},
	{"Представление чего-то", 49, CObjectProperty::t_object,    VAR_OFFSET(DefVal, Param50)},
	{"Param51", 50, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Param51)},
	{"Param52", 51, CObjectProperty::t_int,    VAR_OFFSET(DefVal, Param52)},
	{"ProvHardFldDefs", 62, CObjectProperty::t_object,    VAR_OFFSET(DefVal, ProvHardFldDefs)},
	{"EntryFields", 54, CObjectProperty::t_object,    VAR_OFFSET(DefVal, EntryFields)},
	{"OperFields", 55, CObjectProperty::t_object,    VAR_OFFSET(DefVal, OperFields)},
	{"AccListForms", 56, CObjectProperty::t_object,    VAR_OFFSET(DefVal, AccListForms)},
	{"UnknownListForms", 57, CObjectProperty::t_object,    VAR_OFFSET(DefVal, UnknownListForms)},
	{"ProvListForms", 58, CObjectProperty::t_object,    VAR_OFFSET(DefVal, ProvListForms)},
	{"SbKinds", 59, CObjectProperty::t_object,    VAR_OFFSET(DefVal, SbKinds)},
	{"AccountFields", 52, CObjectProperty::t_object,    VAR_OFFSET(DefVal, AccountFields)},
	{"AccountPlans", 53, CObjectProperty::t_object,    VAR_OFFSET(DefVal, AccountPlans)},
	{"TypOpers", 60, CObjectProperty::t_object,    VAR_OFFSET(DefVal, TypOpers)},
	{"CorrProvsDefs", 61, CObjectProperty::t_object,    VAR_OFFSET(DefVal, CorrProvsDefs)},
	{"OperJournal", 63, CObjectProperty::t_object,    VAR_OFFSET(DefVal, OperJournal)},
	{NULL, -1, CObjectProperty::t_int}
};
CPropertySet CMMSBuh::PropSet(Props, DefVal);

static CStoredSet::TSetPair Buh_EditWay[] = {
	{0, "В списке"},
	{1, "В диалоге"},
	{2, "Обоими способами"},
	{0, NULL}
};
static CStoredSet::TSetPair Buh_CalcOperSum[] = {
	{1, "По всем проводкам"},
	{2, "По всем проводкам в валютной сумме"},
	{3, "По первой проводке"},
	{4, "По первой проводке в валютной сумме"},
	{0, NULL}
};
static CStoredSet::TSetPair Buh_DocOperEditMode[] = {
	{0, "Разрешить"},
	{1, "Запретить"},
	{2, "Выборочно"},
	{0, NULL}
};

CMMSBuh::CMMSBuh()
{
	MMS_Type = MMS_Buh;

	EditWay = 0;
	AccountNameLen = 0; //8. Длина наименования счета
	AccountCodeLen = 0; //9. Длина кода счета
	MaxSubcontoNum = 0; //10. Максимальное количествово субконто
	OperCommentLen = 0; //11. Длина содержания операции
	OperSumLen = 0; //12. Длина суммы операции
	OperSumPrec = 0; //13. Точность суммы операции
	EntrySumLen = 0; //14. Длина суммы проводки
	EntrySumPrec = 0; //15. Точность суммы проводки
	EntryCurrencySumLen = 0; //16. Длина валютной суммы проводки
	EntryCurrencySumPrec = 0; //17. Точность валютной суммы проводки
	EntryAmountLen = 0; //18. Длина количества проводки
	EntryAmountPrec = 0; //19. Точность количества проводки
	Param20 = 0;
	Param21 = 0;
	Param22 = 0;
	Param23 = 0;
	LevelAmountForSelection = 0;
	Param51 = 0;
	Param52 = 0;

	EditWay.SetTable(Buh_EditWay);
	CalcOperSum.SetTable(Buh_CalcOperSum);
	DocOperEditMode.SetTable(Buh_DocOperEditMode);

	ToDestroyChildren = false;

	CMMSObjectAttribute::AddChild(&Param50);
	CMMSObjectAttribute::AddChild(&AccountFields);
	CMMSObjectAttribute::AddChild(&AccountPlans);
	CMMSObjectAttribute::AddChild(&EntryFields);
	CMMSObjectAttribute::AddChild(&OperFields);
	CMMSObjectAttribute::AddChild(&AccListForms);
	CMMSObjectAttribute::AddChild(&UnknownListForms);
	CMMSObjectAttribute::AddChild(&ProvListForms);
	CMMSObjectAttribute::AddChild(&SbKinds);
	CMMSObjectAttribute::AddChild(&TypOpers);
	CMMSObjectAttribute::AddChild(&CorrProvsDefs);
	CMMSObjectAttribute::AddChild(&ProvHardFldDefs);
	CMMSObjectAttribute::AddChild(&OperJournal);
}

CMMSObject* CMMSBuh::CreateChild(const char* Name, int MMS_Index)
{
	switch( MMS_Index )
	{
		case 49: return &Param50;
		case 52: return &AccountFields;
		case 53: return &AccountPlans;
		case 54: return &EntryFields;
		case 55: return &OperFields;
		case 56: return &AccListForms;
		case 57: return &UnknownListForms;
		case 58: return &ProvListForms;
		case 59: return &SbKinds;
		case 60: return &TypOpers;
		case 61: return &CorrProvsDefs;
		case 62: return &ProvHardFldDefs;
		case 63: return &OperJournal;
	}
	return NULL;
}

CMMSObject* CMMSBuh::CreateChild(const char* Name, const char* Value)
{
	if( _stricoll(Name, Param50.BeautyName()) == 0 )
		return &Param50;
	if( _stricoll(Name, AccountFields.BeautyName()) == 0 )
		return &AccountFields;
	if( _stricoll(Name, AccountPlans.BeautyName()) == 0 )
		return &AccountPlans;
	if( _stricoll(Name, EntryFields.BeautyName()) == 0 )
		return &EntryFields;
	if( _stricoll(Name, OperFields.BeautyName()) == 0 )
		return &OperFields;
	if( _stricoll(Name, SbKinds.BeautyName()) == 0 )
		return &SbKinds;
	if( _stricoll(Name, TypOpers.BeautyName()) == 0 )
		return &TypOpers;
	if( _stricoll(Name, CorrProvsDefs.BeautyName()) == 0 )
		return &CorrProvsDefs;
	if( _stricoll(Name, ProvHardFldDefs.BeautyName()) == 0 )
		return &ProvHardFldDefs;
	
	if( _stricoll(Name, AccListForms.BeautyName()) == 0 )
		return &AccListForms;
	if( _stricoll(Name, UnknownListForms.BeautyName()) == 0 )
		return &UnknownListForms;
	if( _stricoll(Name, ProvListForms.BeautyName()) == 0 )
		return &ProvListForms;

	if( _stricoll(Name, OperJournal.BeautyName()) == 0 )
		return &OperJournal;

	return NULL;
}

void CMMSBuh::BeautyString(CBeautyString& String, int level)
{
	String.add_parameter(level, BeautyName(), "", "", true);
	String.add_brace(level, '{');

	CPropertySet& PropSet = *GetPropSet();
	for( int i = 1; i < PropSet.nProps; i++ )
	{
		CObjectProperty& Prop = PropSet[i];
		bool ToAdd = true;
		if( TaskParameters.CompoundType != external_report )
		{
			switch( Prop.MMS_Index )
			{
				case 53:
				case 56:
				case 57:
				case 58:
					ToAdd = false;
					break;
			}
		}
		if( ToAdd )
			PropSet.BeautyString(String, level+1, this, i);
	}

	String.add_brace(level, '}');
}

void CMMSBuh::ToString(CMMSString& String)
{
	//вернём идентификаторы на место
	sID = "";
	AccListForms.sID = "Form";
	ProvListForms.sID = "Form";
	UnknownListForms.sID = "Form";
	CMMSObject::ToString(String);
}

CMMSBuhContainer::CMMSBuhContainer()
{
	MMS_Type = MMS_BuhContainer;
	ToDestroyChildren = false;
	CMMSAttributes::AddChild(&Buh);
}

///////////////////////////////////////////////////////////////////////
///////////  class CMMSBuh methods  ///////////////////////////////////
///////////////////////////////////////////////////////////////////////
void CMMSBuh::ExtractForm(GCString& Dir, CStorage& Storage, 
						  MMSObjectType StorType, MMSObjectType Type)
{
	CNameTableRecord* nt_rec_stor = NameTable.Find(StorType);
	if( Storage.Open(nt_rec_stor->PrepareStorageName(ID)) )
	{
		CNameTableRecord* nt_rec = NameTable.Find(Type);
		if( Storage.Open(nt_rec->PrepareStorageName(ID)) )
		{
			GCString DestDir;

			DestDir = nt_rec->PrepareDirName( nt_rec_stor->PrepareDirName(Dir) );
			CreateDirectory(DestDir);

			ExtractWorkBook(DestDir, Storage, NULL);

			Storage.Close();
		}
		Storage.Close();
	}
}

void CMMSBuh::ImportForm(CStorage& Storage, GCString& SrcDir, 
						 MMSObjectType StorType, MMSObjectType Type)
{
	CNameTableRecord* nt_rec_stor = NameTable.Find(StorType);
	if( !Storage.Create(nt_rec_stor->PrepareStorageName(ID)) ) return;

	GCString FormDir = nt_rec_stor->PrepareDirName(SrcDir);
	CNameTableRecord* nt_rec = NameTable.Find(Type);
	CStorage FormStor(Storage);
	FormStor.Create(nt_rec->PrepareStorageName(ID));
	if( !ImportWorkBook(FormStor, nt_rec->PrepareDirName(FormDir)) )
	{
		FormStor.Close();
		FormStor.Delete(nt_rec->PrepareStorageName(ID));
	}

	Storage.Close();
}

void CMMSBuh::ExtractListForm(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage, 
							  MMSObjectType Type, 
							  CFormDescrList& ListForms)
{
	CNameTableRecord* nt_rec = NameTable.Find(Type);
	GCString DestDir = nt_rec->PrepareDirName(Dir);
	CStorage ListStor(Storage, nt_rec->StorageName);
	CreateDirectory(DestDir);
	ExtractListForms(ListForms, DestDir, ListStor, nt_rec->StorageName+"_Number");

	ListForms.ExtractMetadata(Dir, Type, TypedTextStorage);
}

void CMMSBuh::ImportListForm(CStorage& Storage, GCString& SrcDir, 
							 MMSObjectType Type, 
							 CFormDescrList& ListForms)
{
	CNameTableRecord* nt_rec = NameTable.Find(Type);
	CStorage ListStor(Storage);
	ListStor.Create(nt_rec->StorageName);
	ImportListForms(ListForms, ListStor, nt_rec->PrepareDirName(SrcDir), nt_rec->StorageName+"_Number");
}

////////////////////////////////////////////////////////
//Main decompilation method
void CMMSBuh::Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Buh);

	MMS_Filter.Down();
	if( MMS_Filter.Test(this, nt_rec) )
	{
		GCString BuhDir = nt_rec->PrepareDirName(Dir);
		
		CreateDirectory(BuhDir);

		//Форма счета
		ExtractForm(BuhDir, Storage, NT_AccFormStor, NT_AccForm);
		//Форма операции
		ExtractForm(BuhDir, Storage, NT_OperFormStor, NT_OperForm);

		//Формы списка счетов
		ExtractListForm(BuhDir, Storage, TypedTextStorage, NT_AccListForm, AccListForms);
		//Журнал проводок
		ExtractListForm(BuhDir, Storage, TypedTextStorage, NT_ProvListForm, ProvListForms);

		//Журнал операций
		CMMSOperJournal* Journ = (CMMSOperJournal*)OperJournal.GetChild(0);
		if( Journ != NULL )
		{
			ExtractListForm(BuhDir, Storage, TypedTextStorage, NT_OperListForm, Journ->Forms);
			CNameTableRecord* nt_rec = NameTable.Find(NT_OperListForm);
			GCString DestDir = nt_rec->PrepareDirName(BuhDir);
			Journ->ExtractDescriptions(DestDir+"\\", TypedTextStorage, true);
		}

		//Неизвестно чего форма. Кроме как в ММС нигде не обнаружена. Видимо, чего-то по умолчанию.
		UnknownListForms.ExtractMetadata(BuhDir, NT_UnknownListForm, TypedTextStorage);

		AccountPlans.Decompile(BuhDir, Storage, TypedTextStorage);

		ExtractMetadata(BuhDir, TypedTextStorage);
		ExtractDescriptions(BuhDir+"\\", TypedTextStorage, false);
	}
	MMS_Filter.Up();
}


////////////////////////////////////////////////////////
//Main compilation method
void CMMSBuh::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir)
{
	CNameTableRecord* nt_rec_buh = NameTable.Find(MMS_Buh);
	GCString BuhDir = nt_rec_buh->PrepareDirName(SrcDir);

	MMS_Filter.Down();
	if( MMS_Filter.Test(this, nt_rec_buh) )
	{
		Msg(1, "INF: Параметры бухгалтерии");
		//Форма счета
		ImportForm(Storage, BuhDir, NT_AccFormStor, NT_AccForm);
		//Форма операции
		ImportForm(Storage, BuhDir, NT_OperFormStor, NT_OperForm);

		//Формы списка счетов
		ImportListForm(Storage, BuhDir, NT_AccListForm, AccListForms);
		//Журнал проводок
		ImportListForm(Storage, BuhDir, NT_ProvListForm, ProvListForms);
		//Журнал операций
		CMMSOperJournal* Journ = (CMMSOperJournal*)OperJournal.GetChild(0);
		ImportListForm(Storage, BuhDir, NT_OperListForm, Journ->Forms);
		CNameTableRecord* nt_rec = NameTable.Find(NT_OperListForm);
		GCString JDir = nt_rec->PrepareDirName(BuhDir);
		Journ->ImportDescriptions(TypedTextStorage, JDir+"\\", true);

		//Описания
		for( int i = 0; i < GetNChildren(); i++ )
		{
			GetChild(i)->Compile(Storage, TypedTextStorage, BuhDir);
		}

		ImportDescriptions(TypedTextStorage, BuhDir+"\\", false);
	}
	MMS_Filter.Up();
}

////////////////////////////////////////////////////////////
////  Планы счетов  ////////////////////////////////////////
////////////////////////////////////////////////////////////
void CMMSAccount::Decompile(GCString& Dir, CStorage& Storage, CObjectOrdering& Order)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	GCString FName = nt_rec->PrepareFileName(GCString(), this);

	FName.Replace("%Code%", Code);

	Order.Add(ID, FName, FName);

	CBeautyString String;

	BeautyString(String, 0);
	Storage.CopyStringToFile(String, Dir+"\\"+FName);
}

void CMMSAccountPlan::Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage, CObjectOrdering& Order)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	GCString PlanDir = nt_rec->PrepareDirName(GCString(), this);

	Order.Add(ID, PlanDir, PlanDir);

	PlanDir = nt_rec->PrepareDirName(Dir, this);
	CreateDirectory(PlanDir);

	CNameTableRecord* nt_rec_mdp = NameTable.Find(NT_MetadataPart);
	GCString FName = nt_rec_mdp->PrepareFileName(PlanDir, this);
	CBeautyString String;
	
	BeautyString(String, 0);
	Storage.CopyStringToFile(String, FName);

	CObjectOrdering AccountsOrder(2);
	for( int i = 0; i < Accounts.GetNChildren(); i++ )
	{
		CMMSAccount* Account = (CMMSAccount*)Accounts.GetChild(i);
		Account->Decompile(PlanDir, Storage, AccountsOrder);
	}

	ExtractDescriptions(PlanDir+"\\", TypedTextStorage, true);

	AccountsOrder.WriteFile(Storage, PlanDir);
}

void CMMSAccountPlanList::Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	GCString PlansDir = nt_rec->PrepareDirName(Dir, this);

	CObjectOrdering Order(2);
	for( int i = 0; i < GetNChildren(); i++ )
	{
		CMMSAccountPlan* Plan = (CMMSAccountPlan*)GetChild(i);
		Plan->Decompile(PlansDir, Storage, TypedTextStorage, Order);
	}

	ExtractDescriptions(PlansDir+"\\", TypedTextStorage, true);

	Order.WriteFile(Storage, PlansDir);
}



void CMMSAccountPlan::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	GCString PlanDir = nt_rec->PrepareDirName(SrcDir, this);

	ImportDescriptions(TypedTextStorage, PlanDir+"\\", true);
}

void CMMSAccountPlanList::Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir)
{
	CNameTableRecord* nt_rec = NameTable.Find(MMS_Type);
	GCString PlansDir = nt_rec->PrepareDirName(SrcDir, this);

	for( int i = 0; i < GetNChildren(); i++ )
	{
		CMMSAccountPlan* Plan = (CMMSAccountPlan*)GetChild(i);
		Plan->Compile(Storage, TypedTextStorage, PlansDir);
	}

	ImportDescriptions(TypedTextStorage, PlansDir+"\\", true);
}
