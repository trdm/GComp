#ifndef MMS_Subconto_H
#define MMS_Subconto_H

#include "MMS_Basic.h"

////////////////////////////////////////////////////////////
//// јтрибуты справочников  ////////////////////////////////
////////////////////////////////////////////////////////////
class CSbCntParam : public CMMSObjectAttribute
{
public:
	CStoredBool NotNegative; //1 - неотрицательный, 0 - нет (дл€ чисел)
	CStoredBool DivideTriades; //1 - раздел€ть триады, 0 - нет
	CStoredBool Periodical; //1 - периодический, 0 - нет
	CStoredBool ForElement; //1 - дл€ элемента, 0 - нет
	CStoredBool ForGroup;   //1 - дл€ группы, 0 - нет
	CStoredBool Sorting; //1 - сортировка по реквизиту, 0 - нет
	CStoredBool HistManualChanged; //1 - ручное изменение, 0 - нет (дл€ периодических)
	CStoredBool HistChangedByDocs; //1 - измен€етс€ документами, 0 - нет (дл€ периодических)
	CStoredBool Selection; //1 - отбор по реквизиту, 0 - нет

private:
	static CSbCntParam* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CSbCntParam() {
		ID_Type = IDT_BothIDs;
		NotNegative = 0;
		DivideTriades = 0;
		Periodical = 0;
		ForElement = 0;
		ForGroup = 0;
		Sorting = 0;
		HistManualChanged = 0;
		HistChangedByDocs = 0;
		Selection = 0;
	};

	CPropertySet* GetPropSet() {return &PropSet;};
};

class CSbCntParams : public CMMSAttributes
{
public:
	CSbCntParams() {MMS_Type = MMS_SbCntParams; }

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CSbCntParam;}
	virtual const char* BeautyName() {return "јтрибуты";}
};

////////////////////////////////////////////////////////////
//// —правочники  //////////////////////////////////////////
////////////////////////////////////////////////////////////
class CMMSSbCnt : public CMMSObjectAttribute
{
public:
	CMMSObjRef Owner; //справочник - владелец. 0 - не подчинен
	int CodeLength; //длина кода
	CStoredSet CodeSeries; //1 - во всЄм справочнике, 2 - в пределах подчинени€
	CStoredSet CodeType; //тип кода 1 - текстовый, 2 - числовой
	CStoredSet AutoNumeration; //2 - автоматическа€ нумераци€, 1 - вручн€ка
	int NameLength; //длина наименовани€
	CStoredSet BasicRepresentation; //основное представление 1 - в виде кода, 2 - в виде наименовани€
	CStoredSet EditWay; //способ редактировани€ 0 - в списке, 1 - в диалоге, 2 - обоими способами
	int Levels; //количество уровней
	CMMSObjRef ChoosingListForm; //идентификатор формы списка дл€ выбора
	CMMSObjRef MainListForm; //идентификатор основной формы списка
	CStoredBool OneForm; //1 - одна форма дл€ элемента и группы, 0 - разные формы
	CStoredBool UniqueCodes; //контроль уникальности 1 - коды уникальны, 0 - не об€зательно
	CStoredBool GroupsOnTop; //группы сверху 1 - сверху, 0 - в общем пор€дке

	CSbCntParams Parameters;
	CFormDescrList Forms;

	CMigrationRules MigrationRules;
	
private:
	static CMMSSbCnt* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;
	void SetUpTables();

public:
	CMMSSbCnt();

	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1);
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);

public:
	CPropertySet* GetPropSet() {return &PropSet;};

	void Decompile(GCString& BaseDir, CStorage& Storage, CStorage& TypedTextStorage, CStorage& SubFolderStorage, CStorage& SubListStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& Dir, CStorage& SubFolderStorage, CStorage& SubListStorage);
};

////////////////////////////////////////////////////////////
class CMMSSbCnts : public CMMSAttributes
{
public:
	CMMSSbCnts() {MMS_Type = MMS_SbCnts; ID_Type = IDT_OnlyTextual;}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CMMSSbCnt;}
	virtual const char* BeautyName() {return "—правочники";}

	void Decompile(GCString& BaseDir, CStorage& Storage, CStorage& TypedTextStorage);
	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& Dir);
};

#endif