
#include "Declarations.h"
#include "CommonFunctions.h"
#include "Strings.h"
#include "Lexer.h"
#include "MMS_Basic.h"
#include "DialogForm.h"

bool CMMSObject::ParseFile(GCString& FileName)
{
	if( !FileExist(FileName) )
		return false;

	CLexer Lexer(FileName);
	ParseFile(Lexer);

	return true;
}

void CMMSObject::ParseFile(CLexer& Lexer)
{
	GCString Token, Value, PrevToken, PrevValue;
	CMMSObject* Child;

	while( Lexer.GetToken(Token, ":\t\r\n") )
	{
		Lexer.GetRest(Value);
		Token.TrimRight();
		Value.TrimLeft();

		if( Token == '{' )
		{
			if( Lexer.CurrentLine() <= 2 )
			{
				//просто установим ID и sID для текущего объекта
				PrevValue.TrimLeft();
				PrevValue.TrimRight();
				SetIDs(PrevToken, PrevValue);
			}
			else
			{
				Child = CreateChild(PrevToken, PrevValue);
				if( Child == NULL )
				{
					Msg(0, "ERR: Can't create child %s of %s", PrevToken, sID);
					throw NULL;
				}

				if( PrevValue.IsNumber() ) Child->ID = atoi(PrevValue);

				try {
					Child->ParseFile(Lexer);
				}
				catch(...) {
					Msg(0, "INFO: parsing %s", sID);
					throw NULL;
				}

				AddChild(Child);
			}

			Token = "";
			Value = "";
		}
		else
		{
			if( !PrevToken.IsEmpty() )
			{
				SetValue(PrevToken, PrevValue);
			}
			
			if( Token == '}' )
			{
				return;
			}
		}

		PrevToken = Token;
		PrevValue = Value;
	}

	if( !PrevToken.IsEmpty() )
	{
		SetValue(PrevToken, PrevValue);
	}
}


void CMMSNamedList::ParseFile(CLexer& Lexer)
{
	GCString Token, Value;

	while( Lexer.GetToken(Token, ":\t\r\n") )
	{
		if( Token == "}" )
			return;
		SetValue(-1, Token);
	}
}

void CMMSReferList::ParseFile(CLexer& Lexer)
{
	GCString Token, Value;

	while( Lexer.GetToken(Token, "\t\r\n") )
	{
		Lexer.GetRest(Value);
		if( Token == "}" )
			return;
		SetValue(-1, Token);
	}
}

void CFormLayers::ParseFile(CLexer& Lexer)
{
	GCString Token, Value;
	char* flags;

	while( Lexer.GetToken(Token, ": \t\r\n") )
	{
		Lexer.GetRest(Value);
		if( Token == "}" )
			return;

		CFormLayer* Layer = new CFormLayer;
		Layer->sID = Token;
		flags = strtok((char*)(LPCSTR)Value, ", \t");
		while( NULL != flags )
		{
			if( _stricoll(flags, "Видимый") == 0 )
				Layer->IsVisible = true;
			if( _stricoll(flags, "Активный") == 0 )
				ActiveLayer = GetNChildren();
			flags = strtok(NULL, ", \t");
		}
		AddChild(Layer);
	}
}