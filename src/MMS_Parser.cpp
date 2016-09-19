
#include "Declarations.h"
#include "CommonFunctions.h"
#include "Strings.h"
#include "Lexer.h"
#include "MMS_Basic.h"
#include "DialogForm.h"
#include "Container.h"


CMMSObject* CMMSObject::CreateChild(const char* Name, int MMS_Index)
{
	return new CMMSObject;
}

CMMSObject* CMMSObject::CreateChild(const char* Name, const char* Value)
{
	CMMSObject* Child = CreateChild(Name, -1);
	if( Child == NULL )
		return NULL;

	Child->SetIDs(Name, Value);
	return Child;
}

void CMMSObject::SetIDs(const char* Name, const char* Value)
{
	if( sID.IsEmpty() ) sID = Name;
	if( ID == 0 ) ID = atoi(Value);
}

void CMMSObject::ParseMetadata(CLexer& Lexer)
{
	int ParamsParsed = 0;
	CMMSObject* Child;
	GCString Token;
	TTokenType TokType;
	enum {start, objects, wait_obj_type, end} state;
	bool AtStart = Lexer.AtStart();
	char* ObjName;

	if( AtStart )
		state = start;
	else
		state = objects;

	while( Lexer.GetMMSToken(Token, TokType) )
	{
		switch( state )
		{
		case start:
			switch( TokType )
			{
			case TOK_OpenBrace:
				state = objects;
				break;
			default:
				Msg(0, "ERR: Неверный формат: ожидался символ '{', а получили %s^ %s", Token, Lexer.FilePosInfo());
				throw NULL;
			}
			break;

		case objects:
			switch( TokType )
			{
			case TOK_QuotedString:
				SetValue(ParamsParsed, Token);
				ParamsParsed++;
				break;

			case TOK_OpenBrace:
				state = wait_obj_type;
				break;

			case TOK_CloseBrace:
				if( AtStart )
					state = end; //считали завершающую '}' в файле
				else
					return; //считали завершающую '}' объекта
				break;
			}
			break;

		case wait_obj_type:
			if( TokType == TOK_QuotedString )
				ObjName = (char*)(LPCSTR)Token;
			else
				ObjName = NULL;

			Child = CreateChild(ObjName, ParamsParsed);
			if( Child == NULL )
			{
				Msg(0, "ERR: Не знаю как создать объект с индексом %i. Позиция: %s; Объект: %s", ParamsParsed, Lexer.FilePosInfo(), sID);
				throw NULL;
			}
			AddChild(Child);
			Lexer.PushMMSToken(Token, TokType);
			Child->ParseMetadata(Lexer);
			ParamsParsed++;

			state = objects;
			break;

		case end:
			Msg(0, "ERR: Неверный формат: ожидалось завершение потока, а получили %s: %s", Token, Lexer.FilePosInfo());
			throw NULL;
			break;
		}
	}

	if( state != end )
	{
		//если доехали сюда, значит не было завершающей скобки '}'
		Msg(0, "ERR: Неверный формат: нет завершающей скобки '}': %s; %s", Lexer.FilePosInfo(), sID);
		throw NULL;
	}
}

void CMMSReferList::ParseMetadata(CLexer& Lexer)
{
	int ParamsParsed = 0;
	GCString Token;
	TTokenType TokType;
	enum {start, wait_ref, wait_ref_value, wait_ref_end} state = start;

	while( Lexer.GetMMSToken(Token, TokType) )
	{
		switch( state )
		{
		case start:
			switch( TokType )
			{
			case TOK_QuotedString:
				sID = Token;
				state = wait_ref;
				ParamsParsed++;
				break;
			}
			break;
		case wait_ref:
			switch( TokType )
			{
			case TOK_OpenBrace:
				state = wait_ref_value;
				break;
			case TOK_CloseBrace:
				return;
			}
			break;
		case wait_ref_value:
			switch( TokType )
			{
			case TOK_QuotedString:
				SetValue(ParamsParsed++, Token);
				state = wait_ref_end;
				break;
			}
			break;
		case wait_ref_end:
			switch( TokType )
			{
			case TOK_CloseBrace:
				state = wait_ref;
				break;
			}
			break;
		}
	}
}

void CMMSNamedList::ParseMetadata(CLexer& Lexer)
{
	int ParamsParsed = 0;
	GCString Token;
	TTokenType TokType;

	while( Lexer.GetMMSToken(Token, TokType) )
	{
		switch( TokType )
		{
		case TOK_CloseBrace:
			return;
			break;
		case TOK_QuotedString:
			SetValue(ParamsParsed++, Token);
			break;
		default:
			Msg(0, "ERR: Неверный формат: %s", Lexer.FilePosInfo());
			throw NULL;
		}
	}
}

void CDialogForm::ParseMetadata(CLexer& Lexer)
{
	int ParamsParsed = 0;
	GCString Token;
	TTokenType TokType;
	enum {wait_obj_begin, wait_name, count_braces, end} state = wait_obj_begin;
	GCString ObjName = NULL;
	CMMSObject* Child;
	int brace_counter;

	while( Lexer.GetMMSToken(Token, TokType) )
	{
		switch( state )
		{
		case wait_obj_begin:
			switch( TokType )
			{
			case TOK_OpenBrace:
				state = wait_name;
				break;
			case TOK_CloseBrace:
				state = end;
				break;
			default:
				goto ParseError;
			}
			break;
		case wait_name:
			switch( TokType )
			{
			case TOK_QuotedString:
				ObjName = Token;
				break;
			case TOK_OpenBrace:
			case TOK_CloseBrace:
				ObjName = "";
				break;
			default:
				goto ParseError;
			}

			if( ParamsParsed == 0 )
			{
				sID = Token;
			}
			else
			{
				Child = CreateChild(ObjName, ParamsParsed);
				if( Child == NULL )
				{
					Msg(0, "ERR: Не знаю как создать объект с индексом %i. Позиция: %s; Объект: %s", ParamsParsed, Lexer.FilePosInfo(), sID);
					throw NULL;
				}
				AddChild(Child);

				if( TokType == TOK_QuotedString ) //читаем только поименованные объекты
					Child->ParseMetadata(Lexer);
			}

			ParamsParsed++;
			if( TokType == TOK_OpenBrace ) //две скобки подряд '{{'
			{
				brace_counter = 2;
				state = count_braces;
			}
			else
				state = wait_obj_begin;
			break;

		case count_braces:
			switch( TokType )
			{
			case TOK_OpenBrace:
				brace_counter++;
				break;
			case TOK_CloseBrace:
				brace_counter--;
				break;
			default:
				goto ParseError;
			}
			if( brace_counter == 0 )
				state = wait_obj_begin;
			break;

		case end:
			Msg(0, "WARN: Мусор в конце потока: '%s'", Token);
			break;
		}
	}
	return;

ParseError:
	Msg(0, "ERR: Неверный формат: %s. Объект: %s, Токен = '%s'", Lexer.FilePosInfo(), sID, Token);
	throw NULL;
}
