
#ifndef Lexer_h
#define Lexer_h

#include "Declarations.h"
#include "Strings.h"
#include <afxcoll.h>

typedef enum {TOK_None, TOK_OpenBrace, TOK_CloseBrace, TOK_QuotedString} TTokenType;

typedef struct {
	GCString Token;
	TTokenType Type;
} TTokenStack;

class CLexer
{
private:
	GCString FileName;
	FILE* File;
	int nLine;
	int nColumn;
	char* LineBuff;
	char* ptr;

	CPtrList Stack;

	static GCString error_string;

public:
	CLexer(GCString& FileName);
	~CLexer();

	bool IsValid() { return File != NULL; }
	bool AtStart() { return ftell(File) == 0; }

	void SkipSpaces(const char* spaces = " \t\r\n");
	bool GetLine(GCString& String);
	bool GetToken(GCString& Token, const char* delimiters = ": \t\r\n", bool ProceedOnNextLine = true);
	void GetRest(GCString& String);
	int CurrentLine() { return nLine; }

	bool GetMMSToken(GCString& Token, TTokenType& TokenType);
	void PushMMSToken(GCString& Token, TTokenType TokenType);
	bool PopMMSToken(GCString& Token, TTokenType& TokenType);

	GCString& FilePosInfo();

private:
	char* trim(char* str);
};


#endif