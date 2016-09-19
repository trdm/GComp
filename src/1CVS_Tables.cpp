
#include "Declarations.h"
#include "MMS.h"
#include "NameTable.h"


struct TObjectNameRecord;
typedef void (*TNameTrFunc)(GCString ObjectName, TObjectNameRecord* rec, GCString& Path);

struct TObjectNameRecord {
	char* Name;
	MMSObjectType Type;
	TNameTrFunc NameTrFunc;
	TObjectNameRecord* Children;
};



TObjectNameRecord* FindRecord(TObjectNameRecord* table, const char* Name)
{
	if( table == NULL ) return NULL;

	TObjectNameRecord* record = table;
	while( record->Name != NULL )
	{
		if( _stricoll(record->Name, Name) == 0 )
			return record;
		record++;
	}

	return NULL;
}

bool GetToken(GCString& Name, GCString& Token)
{
	Token = Name.SpanExcluding("./\\");
	if( Token.IsEmpty() ) return false;
	Name = Name.Mid(Token.GetLength());
	int len = Name.SpanIncluding("./\\").GetLength();
	if( len > 0 ) Name = Name.Mid(len);
	return true;
}

MMSObjectType ChildType(MMSObjectType Type)
{
	switch( Type )
	{
	case MMS_SbCnts: return MMS_SbCnt; break;
	case MMS_Documents: return MMS_Document; break;
	}
	return MMS_Object;
}

//справочник, документ, регистр, и т.д.
void basic_object(GCString ObjectName, TObjectNameRecord* rec, GCString& Path)
{
	CNameTableRecord *nt_rec;
	CMMSObject mms_obj;
	GCString Token;

	if( !GetToken(ObjectName, Token) )
		return;

	//Имя объекта
	mms_obj.sID = Token;
	nt_rec = NameTable.Find(ChildType(rec->Type));
	Path = nt_rec->PrepareDirName(Path, &mms_obj);

	//Имя атрибута
	if( GetToken(ObjectName, Token) )
	{
		rec = FindRecord(rec->Children, Token);
		if( rec == NULL && ObjectName.CollateNoCase("Описание") == 0 )
		{
			Path += "\\";
			Path += Token;
			Path += ".";
			Path += "Описание.txt";
		}
		else if( rec != NULL )
		{
			if( rec->NameTrFunc != NULL )
				rec->NameTrFunc(ObjectName, rec, Path);
		}
	}
	else
	{
		nt_rec = NameTable.Find(NT_MetadataPart);
		Path = nt_rec->PrepareFileName(Path);
	}
}

/*
void mdp_object(GCString ObjectName, TObjectNameRecord* rec, GCString& Path)
{
	GCString Name, Token;
	if( !GetToken(ObjectName, Token) )
		return;

	//Имя объекта
	mms_obj.sID = Token;
	nt_rec = NameTable.Find(ChildType(rec->Type));
	Path = nt_rec->PrepareDirName(Path, &mms_obj);
}
*/


TObjectNameRecord ObjectWithDescr[] = {
	{"Метаданные", NT_MetadataPart, NULL, NULL},
	{"Описание",   NT_UserHelp,     NULL, NULL},
	{NULL}
};

TObjectNameRecord DialogForm[] = {
	{"Модуль",   NT_FormModule, NULL, NULL},
	{"Диалог",   NT_DialogForm, NULL, NULL},
	{"Таблица",  NT_MoxelSheet, NULL, NULL},
	{"Описание", NT_UserHelp,   NULL, NULL},
	{NULL}
};

TObjectNameRecord Reference[] = {
	{"Описание",    NT_UserHelp,         NULL, NULL},
	{"Форма",       MMS_Object,          NULL, DialogForm},
	{"ФормаГруппы", NT_SubcontoFolder,   NULL, DialogForm},
	{"ФормаСписка", NT_SubcontoListForm, NULL, DialogForm},
	{NULL}
};

TObjectNameRecord Names[] = {
	{"Константа",  MMS_Consts,    NULL, ObjectWithDescr},
	{"Справочник", MMS_SbCnts,    basic_object, Reference},
	{"Регистр",    MMS_Registers, NULL, ObjectWithDescr},
	{NULL}
};



bool NameToPath(GCString Name, GCString& Path)
{
	TObjectNameRecord *record;
	GCString Token;

	if( !GetToken(Name, Token) ) return false;
	record = FindRecord(Names, Token);
	if( record == NULL ) return false;

	CNameTableRecord* nt_rec = NameTable.Find(record->Type);
	Path = nt_rec->PrepareDirName(Path);
	if( record->NameTrFunc != NULL )
		record->NameTrFunc(Name, record, Path);


	return true;
}
