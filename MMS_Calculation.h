#ifndef MMS_Calculation_H
#define MMS_Calculation_H

#include "MMS_Basic.h"

///////////////////////////////////////////////////////////
//////  Журналы расчётов  /////////////////////////////////
///////////////////////////////////////////////////////////
class CCalcJournalFld : public CMMSObjectAttribute
{
private:
	static CCalcJournalFld* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CStoredBool NonNegative;
	CStoredBool DivideTriades;

public:
	CCalcJournalFld();

	CPropertySet* GetPropSet() {return &PropSet;};
};

class CCalcJournalFldList : public CMMSAttributes
{
public:
	CCalcJournalFldList() {MMS_Type = MMS_CJParams;}

	virtual const char* BeautyName()
		{ return "Поля журнала"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{ return new CCalcJournalFld; }
};

class CCalcJournSelections : public CMMSSimpleReferList
{
public:
	virtual const char* BeautyName() {return "Графы отбора";}
	virtual const char* MMSName() {return "FF";}
};

class CMMSCalcJournal : public CMMSObjectAttribute
{
private:
	static CMMSCalcJournal* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSObjRef		Subconto;		//5.  Числовой идентификатор справочника (поле "Справочник" в диалоге параметров журнала)
	CStoredSet		Period;			//6.	Периодичность: 0 - день, 1 - неделя, 2 - месяц, 3 - квартал, 4 - год
	int				StartDate;		//7.	Дата начала отсчёта в виде количества дней с какой-то нулевой даты (с начала времён?)
	CCalcJournalFldList FldList;	//8.	Реквизиты журнала
	int				ResultLength;	//9.	Длина результата
	int				ResultPrec;		//10.	Точность результата
	CStoredSet		WriteOn;		//11.	Записывать на: 0 - на конец расчётного периода; 1 - на начало расчётного периода
	CCalcJournSelections Selections;//12.	Графы отбора
	CFormDescrList	Forms;			//13.	Формы списка
	CMMSObjRef      SelectionForm;	//14.	Числовой идентификатор формы для выбора
	CMMSObjRef      MainForm;		//15.	Числовой идентификатор основной формы

public:
	CMMSCalcJournal();

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};

//----------------------
class CMMSCalcJournalList: public CMMSAttributes
{
public:
	CMMSCalcJournalList()
		{MMS_Type = MMS_CalcJournals;}

	virtual const char* BeautyName() {return "Журналы расчётов";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{ return new CMMSCalcJournal; }

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};


///////////////////////////////////////////////////////////
//////  Виды расчётов  ////////////////////////////////////
///////////////////////////////////////////////////////////
class CCalcAlgExclusion : public CMMSRefers
{
public:
	virtual const char* BeautyName() {return "Вытесняемые виды расчёта";}
};

class CCalcAlgInGroups : public CMMSSimpleReferList
{
public:
	virtual const char* BeautyName() {return "Входит в группы";}
	virtual const char* MMSName() {return "InGroups";}
};

class CMMSCaclAlgorithm : public CMMSObjectAttribute
{
private:
	static CMMSCaclAlgorithm* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	int Priority; //5.	Приоритет (число)
	GCString Param6;
	GCString Param7;
	CCalcAlgExclusion Exclusion; //8.	Настройки вытеснения
	CCalcAlgInGroups InGroups; //9.	Включать в группы

public:
	CMMSCaclAlgorithm()
		{MMS_Type = MMS_CalcAlgorithm; ToDestroyChildren = false;}

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};


//--------------
class CMMSCaclAlgorithmList : public CMMSAttributes_SF
{
public:
	CMMSCaclAlgorithmList()
		{MMS_Type = MMS_CalcAlgorithms;}

	virtual const char* BeautyName() {return "Виды расчётов";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSCaclAlgorithm;}

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};

///////////////////////////////////////////////////////////
//////  Правила  перерасчёта  /////////////////////////////
///////////////////////////////////////////////////////////
class CRecalcDependent : public CMMSRefers
{
public:
	virtual const char* BeautyName() {return "Что пересчитывать";}
};

class CRecalcBase : public CMMSRefers
{
public:
	virtual const char* BeautyName() {return "Основания для пересчёта";}
};

class CMMSRecalcRule : public CMMSObjectAttribute
{
private:
	static CMMSRecalcRule* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CStoredSet When;  //5.	Период для пересчёта: 0 - только в текущем периоде журнала расчётов; 1 - в том же периоде ЖР; 2 - в следующих N периодах
	int NumPeriods;   //6.	Когда <param5> = 2, то это число периодов для пересчёта (N)
	CRecalcDependent Dependent; //7.	Ссылки на виды расчётов, которые нужно пересчитывать
	CRecalcBase Base; //8.	Ссылки на виды расчётов, при вводе которых происходит пересчёт

public:
	CMMSRecalcRule();

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
};


//--------------
class CMMSRecalcRuleList : public CMMSAttributes
{
public:
	CMMSRecalcRuleList()
		{MMS_Type = MMS_RecalcRules;}

	virtual const char* BeautyName() {return "Правила перерасчёта";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSRecalcRule;}
};

///////////////////////////////////////////////////////////
//////  Группы расчётов  //////////////////////////////////
///////////////////////////////////////////////////////////
class CCalcGroupAlgs : public CMMSSimpleReferList
{
public:
	virtual const char* BeautyName() {return "Состав группы";}
	virtual const char* MMSName() {return "Algs";}
};

class CMMSCaclAlgGroup: public CMMSObjectAttribute
{
private:
	static CMMSCaclAlgGroup* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CCalcGroupAlgs Algs; //5.	Состав группы

public:
	CMMSCaclAlgGroup()
		{ToDestroyChildren = false;}

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
};


//--------------
class CMMSCaclAlgGroupList : public CMMSAttributes_SF
{
public:
	CMMSCaclAlgGroupList()
		{MMS_Type = MMS_Groups;}

	virtual const char* BeautyName() {return "Группы расчётов";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSCaclAlgGroup;}
};

///////////////////////////////////////////////////////////
//////  Календари  ////////////////////////////////////////
///////////////////////////////////////////////////////////
class CMMSHolydays: public CMMSObjectAttribute
{
private:
	static CMMSHolydays* DefVal;
	static CObjectProperty Props[];
	static CPropertySet    PropSet;

public:
	CMigrationRules MigrationRules; //5. Правила миграции

public:
	CMMSHolydays() {}

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return &MigrationRules;}
	virtual CMMSObject* AddChild(CMMSObject* child) {return NULL;}
	
	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);
};

class CCalendarDays : public CMMSNamedList
{
public:
	virtual CStoredProperty* CreateProperty() {return new CStoredString;}
	virtual const char* BeautyName() {return "Настройка дней";}
	virtual const char* MMSName() {return "Days";}
};

class CMMSCalendar : public CMMSObjectAttribute
{
private:
	static CMMSCalendar* DefVal;
	static CObjectProperty Props[];
	static CPropertySet    PropSet;
	static CMMSCalendar*   HolydaysDefVal;
	static CObjectProperty HolydaysProps[];
	static CPropertySet    HolydaysPropSet;

public:
	int StartDate; //5. Стартовая дата в виде числа дней с начала времён
	CCalendarDays Days; //6. Длины дней задаются в виде целых чисел. Чтобы получить правильную длину, эти числа нужно делить на 10.
	CMigrationRules MigrationRules; //7. Правила миграции
	
	CMMSHolydays Holydays;

public:
	CMMSCalendar()
		{MMS_Type = MMS_Calendar; ToDestroyChildren = false;}

	CPropertySet* GetPropSet();
	virtual bool SetValue(int MMS_Index, GCString& Value);
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);

	virtual void BeautyString(CBeautyString& String, int level);
	virtual void ToString(CMMSString& String);
};

//--------------
class CMMSCalendarList : public CMMSAttributes
{
public:
	CMMSCalendarList()
		{MMS_Type = MMS_Calendars;}

	virtual const char* BeautyName() {return "Календари";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSCalendar;}

	virtual void ToString(CMMSString& String);
};

#endif