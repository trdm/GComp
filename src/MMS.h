///////////////////////////////////////////////////////////////////////
///// Main Metadata Stream ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifndef MMS_H
#define MMS_H

#include "MMS_Basic.h"
#include "MMS_Subconto.h"
#include "MMS_Document.h"
#include "MMS_Buh.h"
#include "MMS_Calculation.h"

int __cdecl ObjNameCache_compare_id(const void *elem1, const void *elem2);
int __cdecl ObjNameCache_compare_str(const void *elem1, const void *elem2);
typedef int (__cdecl *TCompareFunction)(const void *elem1, const void *elem2);


////////////////////////////////////////////////////////////
//// Описатель версии МД и служебные поля  /////////////////
////////////////////////////////////////////////////////////
class CMMSMainDataContDef : public CMMSObjectAttribute
{
public:
	int FirstAvailableID; //Счетчик идентификаторов объектов конфигурации
	int MD_ver;
	int UnknownID;

private:
	static CMMSMainDataContDef* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSMainDataContDef();
	virtual char* BeautyName() {return "MainDataContDef";}

public:
	CPropertySet* GetPropSet() {return &PropSet;};
};

////////////////////////////////////////////////////////////
//// Параметры конфигурации  ///////////////////////////////
////////////////////////////////////////////////////////////
class CMMSTaskItemContent : public CMMSSynonymedObject
{
public:
	GCString Password;
	CStoredSet BasicLanguage;
	GCString Param7;
	CStoredBool ImmediateDeletion;
	CStoredSet RoundType;
	GCString Param10;

private:
	static CMMSTaskItemContent* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CMMSTaskItemContent();

public:
	virtual CPropertySet* GetPropSet() {return &PropSet;};
	void BeautyString(CBeautyString& String, int level);
};

class CMMSTaskItem: public CMMSAttributes
{
public:
	CMMSTaskItemContent Content;

public:
	CMMSTaskItem();

	virtual const char* BeautyName() { return "Описание задачи"; }
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return &Content;};
	virtual CMMSObject* AddChild(CMMSObject* Child) {return NULL;}

	virtual void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& Dir);
};

class CMMSCRC : public CMMSObject
{
public:
	virtual void ToString(CMMSString& String) {String.Delete(String.GetLength()-1);}
};

////////////////////////////////////////////////////////////
////  Корневой объект ММС  /////////////////////////////////
////////////////////////////////////////////////////////////
class CMMS : public CMMSAttributes
{
public:
	CMMSMainDataContDef		MainDataContDef;	//0
	CMMSTaskItem			TaskItem;			//1
	CGenJrnlFldList			GenJrnlFldList;		//2
	CDocSelRefObjList		DocSelRefObjList;	//3
	CDocNumDefList			DocNumDefList;		//4
	CConstantList			ConstantList;		//5
	CMMSSbCnts				SbCnts;				//6
	CMMSRegisterList		RegisterList;		//7
	CMMSDocuments			Documents;			//8
	CMMSJournalList			JournalList;		//9
	CMMSEnumList			EnumList;			//10
	CMMSReportList			ReportList;			//11
	CMMSCalcJournalList		CalcJournalList;	//12
	CMMSCalendarList		CalendarList;		//13
	CMMSCaclAlgorithmList	CaclAlgorithmList;	//14
	CMMSRecalcRuleList		RecalcRuleList;		//15
	CMMSCalcVarList			CalcVarList;		//16
	CMMSCaclAlgGroupList	CaclAlgGroupList;	//17
	CMMSDocStreamList		DocStreamList;		//18
	CMMSBuhContainer		BuhContainer;		//19
	CMMSCRC					CRC;				//20

private:
	static CMMS* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

	CObjNameCache ObjNameCache;
	GCString EmptyObjectName;
	void CheckObjNameCache();

public:
	CMMS();
	virtual CMMSObject* CreateChild(const char* Name, const char* Value);
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index);
	virtual CMMSObject* AddChild(CMMSObject* Child);

	virtual int CountObjects();

	CPropertySet* GetPropSet() {return NULL;}//&PropSet;};

	virtual const char* BeautyName() { return "Main Metadata Stream"; }
	virtual const char* MMSName() { return NULL; }

	bool ObjectExist(int ID);
	//Преобразование ID в символьное имя и обратно
	GCString& ObjectIDtoName(int ID);
	int ObjectNameToID(GCString& Name);

	void DeleteUnusedObjects(CStorage& Storage);

	void BuildObjNameCache();
	void BuildObjNameCache(TCompareFunction sort_method, GCString& Dir, CStorage& Storage);
	void ReadObjNameCacheFromDir(GCString& Dir);
	bool ReadObjNameCache(GCString& FileName);

	void Decompile(GCString& Dir, CStorage& Storage, CStorage& TypedTextStorage);
	void ExtractStorage(GCString& Dir, CStorage& Storage, MMSObjectType Type, bool WithContainerCOntents);
	void ExtractCommonMXL(GCString& Dir, CStorage& Storage);
	void ExtractPictureGallery(GCString& Dir, CStorage& Storage);

	virtual void AssembleMetadata(GCString& Dir);

	void Compile(CStorage& Storage, CStorage& TypedTextStorage, GCString& Dir);
	void ImportMMS(CStorage& Storage);
	void ImportGlobalModule(CStorage& Storage, GCString& Dir);
	void ImportCommonMXL(CStorage& Storage, GCString& SrcDir);
	bool ImportStorage(CStorage& Storage, GCString& SrcDir, MMSObjectType Type);
};

#endif //MMS_H