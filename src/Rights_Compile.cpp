
#include "Rights.h"
#include "Lexer.h"
#include "CommonFunctions.h"
#include "Strings.h"
#include "NameTable.h"
#include "Storage.h"
#include "Ordering.h"
#include "MMS_Filter.h"
#include "MMS.h"

/////////////////////////////////////////////////////////////////////////////
DWORD CUserRightsProp::TypeFromString(GCString& strType)
{
	TTypeTable* ptr = TypeTable;
	while( ptr->sID != NULL )
	{
		if( strType.CollateNoCase(ptr->sID) == 0 )
			return ptr->ID;
		ptr++;
	}

	Msg(0, "ERR: Неизвестный тип прав. ID = %s", strType);
	throw NULL;

	return 0;
}

DWORD CUserRightsProp::ValueFromString(GCString& strValue)
{
	if( strValue.CollateNoCase("Да") == 0 )
		return 1;
	if( strValue.CollateNoCase("Yes") == 0 )
		return 1;
	return 0;
}

void CUserRightsProp::SetValues(GCString& strType, GCString& strValue)
{
	Type = TypeFromString(strType);
	Value = ValueFromString(strValue);
}

void CUserRightsProp::Write(IStream* pStream)
{
	pStream->Write(&Type, sizeof(Type), NULL);
	pStream->Write(&Value, sizeof(Value), NULL);
}


/////////////////////////////////////////////////////////////////////////////
DWORD CUserRightsBlock::TypeFromString(GCString& strType)
{
	TBlockType* ptr = BlockTypes;
	while( ptr->sID != NULL )
	{
		if( strType.CollateNoCase(ptr->sID) == 0 )
			return ptr->ID;
		ptr++;
	}

	Msg(0, "ERR: Неизвестный объект прав. ID = %s", strType);
	throw NULL;

	return 0;
}

void CUserRightsBlock::ReadFromFile(CLexer& Lexer, GCString& strType, GCString& strMetadataID)
{
	Type = TypeFromString(strType);
	MetadataID = MainMetadataStream->ObjectNameToID(strMetadataID);
	if( MetadataID == 0 )
		MetadataID = atoi(strMetadataID);

	enum {start, read, stop} state = start;
	GCString token, value;
	CUserRightsProp Prop;

	while( Lexer.GetToken(token, ":\r\n") )
	{
		token.TrimLeft(" \t");
		switch( state )
		{
		case start:
			if( token == '{' )
				state = read;
			else
			{
				Msg(0, "ERR: unexpected token '%s'", token);
				throw NULL;
			}
			break;
		case read:
			if( token == '}' )
				return;

			Lexer.GetRest(value);
			value.TrimLeft();
			Props.Add(Prop);
			Props[nProps].SetValues(token, value);
			nProps++;
			break;
		}
	}
}

void CUserRightsBlock::Write(IStream* pStream)
{
	pStream->Write(&Type, sizeof(Type), NULL);
	pStream->Write(&MetadataID, sizeof(MetadataID), NULL);
	pStream->Write(&nProps, sizeof(nProps), NULL);

	for( int i = 0; i < nProps; i++ )
	{
		Props[i].Write(pStream);
	}
}

/////////////////////////////////////////////////////////////////////////
void CUserRights::ReadFromFile(GCString& FileName)
{
	CLexer Lexer(Translit(FileName));
	if( !Lexer.IsValid() )
	{
		Msg(0, "ERR: Невозможно открыть файл %s: %s", FileName, strerror(errno));
		throw NULL;
	}

	CUserRightsBlock Block;

	GCString token, value;
	while( Lexer.GetToken(token, ":\r\n") )
	{
		Lexer.GetRest(value);
		value.TrimLeft();
		if( token.CollateNoCase("Описание") == 0 )
		{
			Description = value;
		}
		else
		{
			Blocks.Add(Block);
			Blocks[nBlocks].ReadFromFile(Lexer, token, value);
			nBlocks++;
		}
	}
}

void CUserRights::Write(CStorage& Storage, GCString& StreamName)
{
	Storage.CreateStream(StreamName);
	IStream* pStream = Storage.pStream;

	pStream->Write(&ID, sizeof(ID), NULL);
	Description.unquote_linefeeds();
	WriteSizedString(pStream, Description);
	pStream->Write(&nBlocks, sizeof(nBlocks), NULL);

	for( int i = 0; i < nBlocks; i++ )
	{
		Blocks[i].Write(pStream);
	}

	Storage.CloseStream();
}


////////////////////////////////////////////////////////////////////////////
void CompileUserRights(CStorage& Storage, GCString& Dir)
{
	CNameTableRecord* nt_rec = NameTable.Find(NT_UserRights);
	GCString SrcDir = nt_rec->PrepareDirName(Dir);

	MMS_Filter.Down();
	if( MMS_Filter.Test(NULL, nt_rec) )
	{
		Msg(1, "INF: Права");
		CStorage RtStorage(Storage);
		GCString CC = "{\"Container.Contents\"";
		CObjectOrdering Ordering(3);

		RtStorage.Create(nt_rec->PrepareStorageName());
		Ordering.OpenFile(SrcDir);
		while( Ordering.ReadString() )
		{
			GCString FileName;
			FileName.Format("%s\\%s.%s", SrcDir, Ordering.GetValue(0), nt_rec->Ext);

			CUserRights Rights;
			Rights.ReadFromFile(FileName);
			Rights.Write(RtStorage, GCString(Ordering.GetValue(2)));

			CC += ",{\"RightsEditorType\",\"";
			CC += Ordering.GetValue(2);
			CC += "\",\"";
			CC += Ordering.GetValue(1);
			CC += "\",\"\"}";
		}

		CC += "}\n";
		RtStorage.StreamFromString(GCString("Container.Contents"), CC);
	}
}
