
#ifndef NAME_TABLE_H
#define NAME_TABLE_H

#include "Declarations.h"
#include "Strings.h"

class CNameTableRecord
{
public:
	MMSObjectType Type;
	GCString Type1C;
	GCString sID;
	GCString StorageName;
	bool HasSuffix;
	GCString Dir;
	GCString FileName;
	GCString Ext;

public:
	GCString PrepareFileName(GCString& Dir, CMMSObject* object = NULL);
	GCString PrepareDirName(GCString& Dir, CMMSObject* object = NULL);
	GCString PrepareStorageName(int suffix = 0);
};

class CNameTable
{
private:
	CArray<CNameTableRecord*, CNameTableRecord*&> NameTable;
	int nRecords;

public:
	CNameTable() {};
	void RusConstructor();
	void EngConstructor();
	~CNameTable();

	void AddRecord(MMSObjectType Type, LPCSTR Type1C, LPCSTR sID, 
		LPCSTR StorageName, bool HasSuffix, 
		LPCSTR Dir, LPCSTR FileName = NULL, LPCSTR Ext = NULL);
	
	CNameTableRecord* Find(MMSObjectType Type);
	CNameTableRecord* Find(GCString& sID);
	CNameTableRecord* FindByType1C(GCString& Type1C);

	inline int GetNRecords() {return nRecords;};
	inline CNameTableRecord* operator[](int i) {return NameTable[i];};
};

#endif