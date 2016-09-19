
#ifndef MMS_Document_H
#define MMS_Document_H

#include "MMS_Basic.h"

////////////////////////////////////////////////////////////
//// Нумераторы  ///////////////////////////////////////////
////////////////////////////////////////////////////////////
class CDocNumDef : public CMMSObjectAttribute
{
public:
	int Length;
	CStoredSet Period;
	CStoredSet Type;
	CStoredSet	AutoNumeration;	//8.	Автоматическая нумерация: 1 - нет, 2 - да
	CStoredBool Unique;

private:
	static CDocNumDef* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CDocNumDef();

	CPropertySet* GetPropSet() {return &PropSet;};
};

class CDocNumDefList : public CMMSAttributes_SF
{
public:
	CDocNumDefList() {MMS_Type = MMS_DocNumDef;}
	
	virtual const char* BeautyName() { return "Нумераторы"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CDocNumDef;}
};

////////////////////////////////////////////////////////////
//// Общие поля документов  ////////////////////////////////
////////////////////////////////////////////////////////////
class CGenJrnlFldDef : public CMMSObjectAttribute
{
public:
	CStoredBool NonNegative;
	CStoredBool DivideTriades;
	CStoredBool Selection;

private:
	static CGenJrnlFldDef* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CGenJrnlFldDef();

	CPropertySet* GetPropSet() {return &PropSet;};
};

class CGenJrnlFldList : public CMMSAttributes_SF
{
public:
	CGenJrnlFldList() {MMS_Type = MMS_GenJrnlFldDef;}

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CGenJrnlFldDef;}
	virtual const char* BeautyName() {return "Общие поля документов";}
};

///////////////////////////////////////////////////////////
//// Документы ////////////////////////////////////////////
///////////////////////////////////////////////////////////
class CDocHeadField : public CMMSObjectAttribute
{
public:
	CStoredBool	NonNegative;	//9.	Неотрицательный: 1 или 0
	CStoredBool	DivideTriades;	//10.	Разделять триады: 1 или 0

	CDocHeadField();
	CPropertySet* GetPropSet() {return &PropSet;};
private:
	static CDocHeadField* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;
};

class CDocHeadFieldList : public CMMSAttributes
{
public:
	CDocHeadFieldList() {MMS_Type = MMS_DocHeadFields;}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CDocHeadField;}
	virtual const char* BeautyName() {return "Атрибуты шапки";}
};

//-----------------
class CDocTableField : public CMMSObjectAttribute
{
public:
	CStoredBool	NonNegative;	//9.	Неотрицательный: 1 или 0
	CStoredBool	DivideTriades;	//10.	Разделять триады: 1 или 0
	CStoredBool	ColumnSum;		//11.	Итог по колонке: 1 или 0

	CDocTableField();
	CPropertySet* GetPropSet() {return &PropSet;};

private:
	static CDocTableField* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;
};

class CDocTableFieldList : public CMMSAttributes
{
public:
	CDocTableFieldList() {MMS_Type = MMS_DocTableFields;}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CDocTableField;}
	virtual const char* BeautyName() {return "Атрибуты табличной части";}
};

class CDocBaseRefers : public CMMSRefers
{
public:
	virtual const char* BeautyName() {return "Является основанием для";}
};

//----------------
class CMMSDocument : public CMMSObjectAttribute
{
public:
	int			NumberLength;	//5.	Длина номера
	CStoredSet	NumberPeriod;	//6.	Периодичность: 0 - по всем данного вида, 1 - в пределах года, 2 - в пределах квартала, 3 - в пределах месяца, 4 - в пределах дня.
	CStoredSet	NumberType;		//7.	Тип номера: 1 - текстовый, 2 - числовой
	CStoredSet	AutoNumeration;	//8.	Автоматическая нумерация: 1 - нет, 2 - да
	CMMSObjRef	Journal;		//9.	Внутренний идентификатор журнала документов. Если 0, то журнал не задан.
	int			Param10;		//10.	Невыяснено. Всегда -1.
	CStoredBool	UniqueNumbers;	//11.	Контроль уникальности номеров: 1 или 0
	CMMSObjRef	Numerator;		//12.	Внутрений идентификатор нумератора. Если 0, то нумератор не задан.
	CStoredBool	Trade;			//13.	Оперативный учёт: 1 или 0
	CStoredBool	Salary;			//14.	Расчет: 1 или 0
	CStoredBool	Account;		//15.	Бухгалтерский учёт: 1 или 0
	CDocBaseRefers BaseParams;	//16.	Параметры ввода на основании
	CStoredBool	BaseForAny;		//17.	Может являться основанием для любого документа: 1 или 0
	CStoredSet	CreateOperation;//18.	Создавать операцию: 0 - всегда, 1 - выборочно, 2 - только при проведении
	CStoredBool	AutoNumberLines;//19.	Автоматическая нумерация строк: 1 или 0
	CStoredBool	AutoDelActions;	//20.	Автоматическое удаление движений
	CStoredBool	EditOperation;	//21.	Редактировать операцию: 1 или 0
	CStoredBool	EnableActions;	//22.	Разрешить проведение: 1 или 0
	
	CDocHeadFieldList HeadFields;
	CDocTableFieldList TableFields;
	CMigrationRules MigrationRules;

private:
	static CMMSDocument* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSDocument();
	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);

public:
	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};


////////////////////////////////////////////////////////////
class CMMSDocuments : public CMMSAttributes
{
public:
	CMMSDocuments();
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSDocument;}
	virtual const char* BeautyName() {return "Документы";}

public:
	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};

///////////////////////////////////////////////////////////
//// Последовательность документов ////////////////////////
///////////////////////////////////////////////////////////
class CDocStreamRegisters : public CMMSSimpleReferList
{
public:
	virtual const char* BeautyName() {return "Движения влияющие на последовательность";}
	virtual const char* MMSName() {return "Registers";}
};
class CDocStreamDocuments: public CMMSSimpleReferList
{
public:
	virtual const char* BeautyName() {return "Последовательность влияет на проведение документов";}
	virtual const char* MMSName() {return "Documents";}
};

class CMMSDocStream: public CMMSObjectAttribute
{
public:
	GCString Param5;
	CStoredSet Migration; //6. Настройка для УРБД: 0 - единая последовательность в центральной ИБ; 1 - отдельные последовательности в каждй ИБ.
	CDocStreamRegisters Registers; //7
	CDocStreamDocuments Documents; //8

private:
	static CMMSDocStream* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSDocStream();

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
};

class CMMSDocStreamList : public CMMSAttributes_SF
{
public:
	CMMSDocStreamList() {MMS_Type = MMS_DocumentStreams;}
	virtual const char* BeautyName() {return "Последовательность влияет на проведение документов";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSDocStream;}
};

////////////////////////////////////////////////////////////
//// Графы отбора  /////////////////////////////////////////
////////////////////////////////////////////////////////////
class CDocSelRefObj : public CMMSObjectAttribute
{
public:
	CStoredBool SelectEmpty;
	CMMSTypeLetter TypeLetter;
	CMMSObjRef TypeObject;
	CMMSReferList Refers;

private:
	static CDocSelRefObj* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CDocSelRefObj();

	CPropertySet* GetPropSet() {return &PropSet;};
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
};

class CDocSelRefObjList : public CMMSAttributes_SF
{
public:
	CDocSelRefObjList() {MMS_Type = MMS_DocSelRefObj;}

	virtual const char* BeautyName() { return "Графы отбора"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CDocSelRefObj;}
};

///////////////////////////////////////////////////////////
//////  Журналы документов  ///////////////////////////////
///////////////////////////////////////////////////////////
class CJournFldContent: public CMMSReferList
{
public:
	virtual const char* BeautyName() {return "Поля документов";}
};

class CJournalFld : public CMMSObjectAttribute
{
private:
	static CJournalFld* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CJournFldContent DocumentFields;

public:
	CJournalFld() {}

	CPropertySet* GetPropSet() {return &PropSet;};

	virtual CMMSObject* AddChild(CMMSObject* child) {return NULL;}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{ return &DocumentFields; }
};

class CJournalFldList : public CMMSAttributes
{
public:
	CJournalFldList() {MMS_Type = MMS_JournalFlds;}

	virtual const char* BeautyName()
		{ return "Поля журнала"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{ return new CJournalFld; }
};

class CJournalDocuments : public CMMSRefers
{
public:
	virtual const char* BeautyName() {return "Состав журнала";}
};

class CMMSJournal: public CMMSObjectAttribute//CMMSSynonymedObject
{
private:
	static CMMSJournal* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	GCString        Param5;         //5.	неизвестный параметр
	CStoredBool     IsCommon;       //6.	Тип журнала: 0 - обычный, 1 - общий
	CMMSObjRef      SelectionForm;  //7.	Числовой идентификатор формы для выбора
	CMMSObjRef      MainForm;       //8.	Числовой идентификатор основной формы
	CStoredBool     NonAditional;   //9.	НЕ дополнительный: 0 - журнал дополнительный, 1 - нет
	CJournalDocuments Documents;    //10.	Состав журнала
	CJournalFldList Fields;         //11.	Графы журнала
	CFormDescrList  Forms;          //12.	Формы журнала

public:
	CMMSJournal();
	CPropertySet* GetPropSet() {return &PropSet;};

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};


////////////////////////////////////////////////////////////
class CMMSJournalList: public CMMSAttributes
{
public:
	CMMSJournalList()
		{MMS_Type = MMS_Journalisters;}
	virtual const char* BeautyName() {return "Журналы документов";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{ return new CMMSJournal; }

	void Decompile(GCString& SrcDir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& SrcDir);
};

#endif
