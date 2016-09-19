
#include "CommonFunctions.h"
#include "Lexer.h"
#include "Strings.h"

#define BUFF_SIZE 4096  //этого должно быть достаточно. Наверное. А может и нет.

CLexer::CLexer(GCString& FileName)
{
	try {
	File = fopen(FileName, "rt");
	} catch(...)
	{
		Msg(0, "ERR: Невозможно открыть файл '%s' : %s", FileName, strerror(errno));
	}
	this->FileName = FileName;
	LineBuff = new char[BUFF_SIZE];
	ptr = NULL;
	nLine = 0;
	nColumn = 0;
}

CLexer::~CLexer()
{
	if( File != NULL )
		fclose(File);
	if( LineBuff != NULL )
		delete[] LineBuff;
}


void CLexer::SkipSpaces(const char* spaces)
{
	if( ptr == NULL )
		return;
	ptr += strspn(ptr, spaces);
}

char* CLexer::trim(char* str)
{
	unsigned char* ptr = (unsigned char*)str;
	while( isspace(*ptr) && *ptr != '\0' ) ptr++;
	if( *ptr != '\0' )
	{
		unsigned char* end = ptr + strlen((char*)ptr) - 1;
		while( *end == '\r' || *end == '\n' ) *end-- = '\0'; //обрезаем только \r\n
		//while( isspace(*end) && *end != '\0' ) *end-- = '\0'; //обрезаем все пробелы
	}
	return (char*)ptr;
}

bool CLexer::GetLine(GCString& String)
{
	if( fgets(LineBuff, BUFF_SIZE, File) == NULL ) return false;
	nLine++;
	ptr = trim(LineBuff);
	String = ptr;
	return true;
}

bool CLexer::GetToken(GCString& Token, const char* delimiters, bool ProceedOnNextLine)
{
	nColumn = 0;
	while( ptr == NULL )
	{
		if( !ProceedOnNextLine )
		{
			Token = "";
			return false;
		}

		if( fgets(LineBuff, BUFF_SIZE, File) == NULL ) return false;
		nLine++;
		ptr = trim(LineBuff);
		if( *ptr == '\0' ) ptr = NULL;
	}
	
	if( *ptr == '{' || *ptr == '}' )
	{
		Token = *ptr;
		ptr++;
	}
	else
	{
		char* left = ptr;
		char* right = strpbrk(ptr, delimiters);

		if( right != NULL )
		{
			ptr = right + strspn(right, delimiters);
			*right = '\0';
			Token = left;
		}
		else
		{
			Token = ptr;
			ptr = NULL;
		}
	}

	if( ptr != NULL && *ptr == '\0' ) ptr = NULL;

	return true;
}

void CLexer::GetRest(GCString& String)
{
	if( ptr == NULL )
		String = "";
	else
		String = ptr;
	ptr = NULL;
}

bool CLexer::GetMMSToken(GCString& Token, TTokenType& TokenType)
{
	if( PopMMSToken(Token, TokenType) )
		return true;

	int c;
	enum {start, in_string, end_string_check} state = start;

	Token = "";
	TokenType = TOK_None;

	while( !feof(File) )
	{
		c = fgetc(File);

		switch( c )
		{
		case '\n':
			nLine++;
			nColumn = 1;
			break;
		case '\r':
			break;
		default:
			nColumn++;
		}

		switch( state )
		{
		case start:
			switch( c )
			{
			case '"':
				state = in_string;
				break;
			case '{':
				Token = "{";
				TokenType = TOK_OpenBrace;
				return true;
			case '}':
				Token = "}";
				TokenType = TOK_CloseBrace;
				return true;
			}
			break;

		case in_string:
			if( c == '"' )
				state = end_string_check;
			else
				Token += (char)c;
			break;

		case end_string_check:
			if( c == '"' )
			{
				Token += "\"\"";
				state = in_string;
			}
			else
			{
				ungetc(c, File);
				nColumn--;
				TokenType = TOK_QuotedString;
				return true;
			}
			break;
		}
	}
	return false;
}

void CLexer::PushMMSToken(GCString& Token, TTokenType TokenType)
{
	TTokenStack* entry = new TTokenStack;
	entry->Token = (LPCSTR)Token;
	entry->Type = TokenType;
	Stack.AddTail((void*)entry);
}

bool CLexer::PopMMSToken(GCString& Token, TTokenType& TokenType)
{
	if( Stack.IsEmpty() )
		return false;

	TTokenStack* entry = (TTokenStack*)Stack.GetTail();
	Token = (LPCSTR)(entry->Token);
	TokenType = entry->Type;
	Stack.RemoveTail();
	delete entry;
	return true;
}

GCString CLexer::error_string;
GCString& CLexer::FilePosInfo()
{
	if( nColumn == 0 )
		error_string.Format("%s: line %i", FileName, nLine);
	else
		error_string.Format("%s: line %i, column %i", FileName, nLine, nColumn);
	return error_string;
}