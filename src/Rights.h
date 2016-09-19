
#ifndef Rights_H
#define Rights_H

#include "Declarations.h"
#include "Strings.h"
#include "Lexer.h"
#include "Container.h"


class CUserRightsProp
{
private:
	DWORD Type;
	DWORD Value;

	typedef struct {
		DWORD ID;
		const char* sID;
	} TTypeTable;
	static TTypeTable TypeTable[];

	const char* TypeAsString();
	DWORD TypeFromString(GCString& strType);
	const char* ValueAsString();
	DWORD ValueFromString(GCString& strValue);

public:
	CUserRightsProp();

	void Read(CStorage& Storage);
	void BeautyString(CBeautyString& String, int level);
	void CSVString(GCString& String);

	void SetValues(GCString& strType, GCString& strValue);
	void Write(IStream* pStream);
};

class CUserRightsBlock
{
private:
	DWORD Type;
	DWORD MetadataID;
	GCString Metadata_sID;
	DWORD nProps;
	CArray<CUserRightsProp, CUserRightsProp&> Props;

	typedef struct {
		DWORD ID;
		const char* sID;
	} TBlockType;
	static TBlockType BlockTypes[];

	const char* TypeAsString();
	DWORD TypeFromString(GCString& strType);

public:
	CUserRightsBlock();
	CUserRightsBlock& operator=(CUserRightsBlock& Block);

	void Read(CStorage& Storage);
	void BeautyString(CBeautyString& String, int level);
	void CSVString(GCString& String, GCString& Prefix);

	void ReadFromFile(CLexer& Lexer, GCString& strType, GCString& strMetadataID);
	void Write(IStream* pStream);

	bool LinkIsBroken() { return (MetadataID != 0) && (!MainMetadataStream->ObjectExist(MetadataID)); }
	static int __cdecl CompareBysID(const void *elem1, const void *elem2);
};

class CUserRights
{
private:
	DWORD ID;
	GCString Description;
	DWORD nBlocks;
	CArray<CUserRightsBlock, CUserRightsBlock&> Blocks;

public:
	CUserRights();

	void Read(CStorage& Storage);
	void BeautyString(CBeautyString& String);
	void CSVString(GCString& String, GCString& RightsName);

	void WriteToFile(CStorage& Storage, GCString FileName);

	void ReadFromFile(GCString& FileName);
	void Write(CStorage& Storage, GCString& StreamName);
};

void DecompileUserRights(GCString& Dir, CStorage& Storage);
void CompileUserRights(CStorage& Storage, GCString& Dir);


#endif
