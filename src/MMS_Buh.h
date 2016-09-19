#ifndef MMS_Buh_H
#define MMS_Buh_H

#include "MMS_Basic.h"
#include "MMS_Document.h"

////////////////////////////////////////////////////////////
//// Счета  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////
class CAccountSubconto : public CMMSObjectAttribute
{
public:
	CStoredBool Param9;
	CStoredBool Param10;
	CMMSObjRef SbKind; //11. Вид субконто
	CStoredBool OnlyMovement; //12. Только обороты
	CStoredBool BySum; //13. Учёт по сумме
	CStoredBool ByCurrencySum; //14. Учёт по валютной сумме
	CStoredBool ByAmount; //15. Учёт по количеству
private:
	static CAccountSubconto* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CPropertySet* GetPropSet() {return &PropSet;};
	virtual void BeautyString(CBeautyString& String, int level);
};

class CAccountSubcontoList : public CMMSAttributes
{
public:
	CAccountSubcontoList() {sID = "AccSbCnts";}

	virtual const char*BeautyName() {return "Аналитика";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CAccountSubconto;}
};

class CMMSAccount: public CMMSObject
{
public:
	GCString Name;
	GCString Param2; //непонятно что. Всегда ""
	GCString Code;   //Код счёта
	CStoredBool IsCurrency; //Валютный
	CStoredBool IsAmount;   //Количественный учёт
	CStoredBool IsSingle;   //Забалансовый счет
	CStoredBool IsGroup;    //Счет я вляется группой
	CStoredSet  IsActive;   //1 - активный; 2 - пассивный; 3 - активно-пассивный
	CAccountSubcontoList SubcontoList;

private:
	static CMMSAccount* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSAccount();

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual void BeautyString(CBeautyString& String, int level);

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return &SubcontoList;}
	virtual void SetIDs(const char* Name, const char* Value);

	void Decompile(GCString& Dir, CStorage& Storage, CObjectOrdering& Order);
};

class CMMSAccountList : public CMMSAttributes
{
public:
	CMMSAccountList() {sID = "Accounts";}

	virtual const char*BeautyName() {return "Счета";}

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSAccount;}
};

////////////////////////////////////////////////////////////
//// Планы счетов ///////////////////////////////////////////
////////////////////////////////////////////////////////////
class CMMSAccountPlan: public CMMSObject
{
public:
	GCString Comment;
	GCString Synonym;
	GCString AccountMask1; //Маска кода счета
	GCString AccountMask2; //для чего их две - я не знаю
	CMMSAccountList Accounts;
	CMigrationRules Migration;
	
private:
	static CMMSAccountPlan* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;
	int AccMaskAmount;

public:
	CMMSAccountPlan();

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);

	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage, CObjectOrdering& Order);

	virtual void AssembleMetadata(GCString& Dir);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};

class CMMSAccountPlanList: public CMMSAttributes
{
public:
	CMMSAccountPlanList();

	virtual const char* BeautyName() { return "Планы счетов"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSAccountPlan;}

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);

	virtual void AssembleMetadata(GCString& Dir);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};


////////////////////////////////////////////////////////////
//// Параметры бухгалтерии /////////////////////////////////
////////////////////////////////////////////////////////////
class CMMSTypedField : public CMMSObjectAttribute
{
public:
	CStoredBool	NonNegative;	//9.	Неотрицательный: 1 или 0
	CStoredBool	DivideTriades;	//10.	Разделять триады: 1 или 0
	CStoredBool Selection;		//11.	Отбор
	CStoredBool Periodical;		//12.	Периодический

private:
	static CMMSTypedField* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CPropertySet* GetPropSet() {return &PropSet;};
};

class CMMSTypedDocField : public CMMSTypedField
{
private:
	static CMMSTypedDocField* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CPropertySet* GetPropSet() {return &PropSet;};
};

class CMMSAccountFieldList : public CMMSAttributes
{
public:
	CMMSAccountFieldList() {MMS_Type = MMS_AccParams;}

	virtual const char* BeautyName() { return "Реквизиты счета"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSTypedField;}
};

//----------------------------------------------------
class CMMSEntryFieldList : public CMMSAttributes
{
public:
	CMMSEntryFieldList() {MMS_Type = MMS_ProvParams;}

	virtual const char* BeautyName() { return "Реквизиты проводки"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSTypedDocField;}
};

//----------------------------------------------------
class CMMSOperFieldList : public CMMSAttributes
{
public:
	CMMSOperFieldList() {MMS_Type = MMS_OperParams;}

	virtual const char* BeautyName() { return "Реквизиты операции"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSTypedDocField;}
};

//----------------------------------------------------
class CMMSBuhSbKindRepr : public CMMSTypedObject
{
public:
	CMMSObjRef Field; //1.	Ссылка на поле объекта
	GCString Name;	//2.	Заголовок
	CStoredBool FromNewLine;	//3.	С новой строки
private:
	static CMMSBuhSbKindRepr* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CPropertySet* GetPropSet() {return &PropSet;};
	virtual const char* BeautyName() {return "Представление";}
};

class CMMSBuhSbKindReprList : public CMMSAttributes
{
public:
	CMMSBuhSbKindReprList() {}

	virtual const char* BeautyName() { return "Представление субконто"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSBuhSbKindRepr;}

	virtual void ToString(CMMSString& String);

};

class CMMSBuhSbKind : public CMMSObjectAttribute
{
public:
	CStoredBool	DivideTriades;	//9.	Разделять триады: 1 или 0
	CStoredBool	NonNegative;	//10.	Неотрицательный: 1 или 0
	CStoredBool Selection;		//11.	Отбор
	CMMSObjRef Price;	//12.	Реквизит справочника "Цена"
	CMMSObjRef CurrencyPrice;	//13.	Реквизит справочника "Валютная цена"
	CMMSBuhSbKindReprList Representation;	//14.	Представление
	CStoredBool FastSelection;	//15.	Быстрый выбор
	CStoredBool AllowEmpty;	//16.	Ввод пустых субконто

private:
	static CMMSBuhSbKind* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSBuhSbKind() {ToDestroyChildren = false;}

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
};

class CMMSBuhSbKindList : public CMMSAttributes
{
public:
	CMMSBuhSbKindList() {MMS_Type = MMS_SbKind;}

	virtual const char* BeautyName() { return "Виды субконто"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSBuhSbKind;}
};

//----------------------------------------------------
class CMMSTypOper : public CMMSSynonymedObject
{
public:
	CMigrationRules MigrationRules;

private:
	static CMMSTypOper* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSTypOper() {ToDestroyChildren = false;}

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return &MigrationRules;}

	virtual void BeautyString(CBeautyString& String, int level);
};

class CMMSTypOperList : public CMMSAttributes
{
public:
	CMMSTypOperList() {MMS_Type = MMS_TypOpersDef;}

	virtual const char* BeautyName() { return "Типовые операции"; }
	virtual void ToString(CMMSString& String);

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSTypOper;}
};

//----------------------------------------------------
class CMMSCorrProvsDef : public CMMSTypOper
{
};

class CMMSCorrProvsDefList : public CMMSAttributes
{
public:
	CMMSCorrProvsDefList() {MMS_Type = MMS_CorrProvsDef;}

	virtual const char* BeautyName() { return "Корректные проводки"; }
	virtual void ToString(CMMSString& String);

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSCorrProvsDef;}
};

//----------------------------------------------------
class CMMSProvHardFldDefList : public CMMSAttributes
{
public:
	CMMSProvHardFldDefList() {MMS_Type = MMS_ProvHardFldDef;}

	virtual const char* BeautyName() { return "Предопределённые реквизиты проводки"; }
	virtual void ToString(CMMSString& String);

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSTypedDocField;}
};

class CMMSOperJournal : public CMMSJournal //CMMSSynonymedObject
{
public:
	CMMSOperJournal() {MMS_Type = NT_OperListForm;}
};

class CMMSOperJournalList: public CMMSJournalList
{
public:
	CMMSOperJournalList() {MMS_Type = MMS_OperJournal;}

	virtual const char* BeautyName() { return "Журнал операций"; }
	virtual void ToString(CMMSString& String);

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{ return new CMMSOperJournal; }
};

//------------------------------------------------------
class CAccListForms : public CFormDescrList
{
public:
	CAccListForms() { sID = "ФормыСпискаСчетов"; };
	virtual const char* BeautyName() {return "Формы списка счетов";}
};
class CUnknownListForms : public CFormDescrList
{
public:
	CUnknownListForms() { sID = "UnknownListForms"; };
	virtual const char* BeautyName() {return "UnknownListForms";}
};
class CProvListForms : public CFormDescrList
{
public:
	CProvListForms() { sID = "ФормыСпискаПроводок"; };
	virtual const char* BeautyName() {return "Формы списка проводок";}
};

//------------------------------------------------------
//------------------------------------------------------
class CMMSBuh: public CMMSObjectAttribute
{
public:
	//Форма счета и операции определяется по ID
	CStoredSet EditWay; //5. Редактировать
	CMMSObjRef ChoosingListForm; //6. идентификатор формы списка для выбора
	CMMSObjRef MainListForm; //7. идентификатор основной формы списка
	int AccountNameLen; //8. Длина наименования счета
	int AccountCodeLen; //9. Длина кода счета
	int MaxSubcontoNum; //10. Максимальное количествово субконто
	int OperCommentLen; //11. Длина содержания операции
	int OperSumLen; //12. Длина суммы операции
	int OperSumPrec; //13. Точность суммы операции
	int EntrySumLen; //14. Длина суммы проводки
	int EntrySumPrec; //15. Точность суммы проводки
	int EntryCurrencySumLen; //16. Длина валютной суммы проводки
	int EntryCurrencySumPrec; //17. Точность валютной суммы проводки
	int EntryAmountLen; //18. Длина количества проводки
	int EntryAmountPrec; //19. Точность количества проводки
	int Param20;
	int Param21;
	int Param22;
	int Param23;
	CMMSObjRef Currency_Subconto; //24. ID справочника валют
	CMMSObjRef Currency_Rate; //25. ID реквизита "курс"
	GCString Param26;
	CStoredBool SelectByOperSum; //27. Отбор по сумме операции
	CStoredBool SelectByOperComment; //28. Отбор по содержанию операции
	CStoredSet  CalcOperSum; //29. Расчет суммы операции: 0 - Не рассчитывать; 1 - По всем проводкам; 2 - По всем проводкам в валютной сумме; 3 - По первой проводке; 4 - По первой проводке в валютной сумме
	CStoredBool EnableEntrySelect; //30. Разрешить отбор проводок
	int LevelAmountForSelection; //31. Количество уровней для отбора проводок (0-по всем)
	CStoredBool OperSum_DivideTriades; //32. Разделять триады сумма операции
	CStoredBool EntrySum_DivideTriades; //33. Разделять триады сумма проводки
	CStoredBool EntryCurrSum_DivideTriades; //34. Разделять триады валютной суммы проводки
	CStoredBool EntryAmount_DivideTriades; //35. Разделять триады количества проводки
	CStoredBool SelectByEntrySum; //36. Отбор по сумме проводки
	CStoredBool SelectByEntryCurr; //37. Отбор по валюте проводки
	CStoredBool SelectByEntryCurrSum; //38. Отбор по валютной сумме проводки
	CStoredBool SelectByEntryAmount; //39. Отбор по количеству проводки
	CMMSObjRef  Currency_Multiple; //40. Код реквизита кратность в плане счетов
	CStoredBool EnableComplexEntries; //41. Сложные проводки
	CStoredBool ToChooseCalcSumMode; //42. Выбор режима расчета суммы
	CStoredBool SelectEntryByDebCred; //43. Отбор проводок по Дебету/Кредиту
	CStoredSet  DocOperEditMode; //44. Редактировать операций документов: 0 - Разрешить; 1 - Запретить; 2 - Выборочно 
	CStoredBool SelectByEntryPlan; //45. Отбор по планам счетов проводки
	CStoredBool AmountOnlyByAnalytics; //46. Количичественный учет только по аналитике
	CMMSObjRef  AccountingDividor; //47. Код реквизита проводки - Разделитель учета
	CMMSObjRef  MainAccPlan; //48. Код основного плана счетов
	GCString Param49; //49.
	CMMSBuhSbKindReprList Param50; //50.	Представление чего-то
	int Param51; //51.
	int Param52; //52.
	CMMSAccountFieldList AccountFields; //53.
	CMMSAccountPlanList AccountPlans; //54.
	CMMSEntryFieldList EntryFields; //55.
	CMMSOperFieldList OperFields; //56.
	CAccListForms AccListForms; //57.
	CUnknownListForms UnknownListForms; //58.
	CProvListForms ProvListForms; //59.
	CMMSBuhSbKindList SbKinds; //60.
	CMMSTypOperList TypOpers; //61. TypOpersDef
	CMMSCorrProvsDefList CorrProvsDefs; //62. CorrProvsDef
	CMMSProvHardFldDefList ProvHardFldDefs; //63. ProvHardFldDef
	CMMSOperJournalList OperJournal; //64. OperJournal

	//CFormDescrList UnknownListForms;

private:
	static CMMSBuh* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSBuh();

	CPropertySet* GetPropSet() {return &PropSet;};
	
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
	virtual CMMSObject* AddChild(CMMSObject* Child) {return NULL;}

	virtual const char* BeautyName() { return "Параметры бухгалтерии"; }
	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);

	void ExtractForm(GCString& Dir, CStorage& Storage, MMSObjectType StorType, MMSObjectType Type);
	void ExtractListForm(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage,
		MMSObjectType Type, CFormDescrList& ListForms);
	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);

	virtual void AssembleMetadata(GCString& Dir);
	void ImportForm(CStorage& Storage, GCString& SrcDir, MMSObjectType StorType, MMSObjectType Type);
	void ImportListForm(CStorage& Storage, GCString& SrcDir, MMSObjectType Type, CFormDescrList& ListForms);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};

class CMMSBuhContainer : public CMMSAttributes
{
public:
	CMMSBuh Buh;

public:
	CMMSBuhContainer();

	virtual const char* BeautyName() { return "Параметры бухгалтерии"; }
	virtual const char* MMSName() { return "Buh"; }

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return &Buh;}
	virtual CMMSObject* AddChild(CMMSObject* Child) {return NULL;}

	virtual void AssembleMetadata(GCString& Dir);

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage)
		{Buh.Decompile(Dir, Storage, TypedTextStorage);}
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir)
		{Buh.Compile(Storage, TypedTextStorage, SrcDir);}
};

#endif