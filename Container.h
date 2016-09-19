
////////////////////////////////////////////////////////////////////
//////  Классы для работы с Container.Contents   ///////////////////
////////////////////////////////////////////////////////////////////

#ifndef CONTAINER_H
#define CONTAINER_H

#include "Strings.h"
#include "MMS.h"

typedef struct
{
	LPCSTR Type;
	LPCSTR Descr;
} CContainerInfo;

typedef enum{CT_Normal, CT_UserInterfaces} CContainerType;

class CContainerRecord : public CMMSObject
{
public:
	GCString Type;
	GCString StreamName;
	GCString ObjectName;
	GCString Param4;

private:
	static CContainerRecord* DefVal;
	static CObjectProperty Props[];
	static CPropertySet PropSet;

public:
	CContainerRecord() {};
	CPropertySet* GetPropSet() {return &PropSet;};
};

class CContainer : public CMMSAttributes
{
public:
	GCString Content;

public:
	CContainer();
	virtual const char* BeautyName() {return "Container.Contents";}
	virtual CMMSObject* CreateChild(const char* Name, int MMS_Index = -1)
		{return new CContainerRecord;};

	GCString& Create(CStorage& Storage, bool Recursive, CContainerType Type = CT_Normal);
	GCString& CreateInTypedText(CStorage& TypedTextStorage);

private:
	void Add(LPCSTR Type, LPCSTR Name, LPCSTR Descr);
	void AddDir(LPSTR DirName);
	void AddDir(const wchar_t* wDirName);
	void AddFile(LPSTR FileName, CContainerType Type = CT_Normal);
	void AddFile(const wchar_t* wFileName, CContainerType Type = CT_Normal);

};

#endif