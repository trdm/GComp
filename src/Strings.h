
#ifndef Strings_H
#define Strings_H

#include "Declarations.h"

class GCString : public CString
{
public:
	static const GCString EmptyString;

public:
	GCString() :CString() {};
	GCString(const char* str) :CString(str) {};
	GCString(const char c) :CString(c) {};
	GCString(int val) :CString() {Format("%i", val);};
	GCString(double val) :CString() {Format("%g", val);};
	GCString(unsigned int val) :CString() {Format("%u", val);};
	GCString(const GCString& str) :CString((const CString&)str) {};
	GCString(const CString& str) :CString(str) {};

	operator CString& () { return *((CString*)this); };

	/*
	int Resize(int delta);

	const GCString& operator+=(TCHAR ch);
	const GCString& operator+=(LPCTSTR lpsz);
	const GCString& operator+=(const GCString& str);
	//*/

	friend GCString operator+(const GCString& string1, const GCString& string2);

	friend GCString operator+(const GCString& string, const char* str);
	friend GCString operator+(const char* str, const GCString& string);

public:
	void Add(const char* str, int count = 1);

	//Все символы перевода строки заменяем на "\n", и соответственно "\" -> "\\"
	//Замена производится прямо в строке - строк с такой фигнёй не должно быть много, 
	//поэтому inplace замена будет выгоднее.
	void quote_linefeeds();
	//Обратное преобразование
	void unquote_linefeeds();

	bool IsNumber();

};


/*////////////////////////////////////////////////////////////////////////////////
 Класс для "красивого" вывода параметров в текст. Предполагается, что файл состоит
 из строчек вида
 <ИмяПараметра>: [ЗначениеПараметра]
////////////////////////////////////////////////////////////////////////////////*/
class CBeautyString : public GCString
{
public:
	const CBeautyString& operator=(LPCSTR lpsz)
		{
			((GCString*)this)->operator=(lpsz); 
			return *this;
		};

	void add_offset(int level);
	void add_brace(int level, const char brace);

	void add_parameter(int level, 
					   const char* id, 
					   const char* value, 
					   const char* default_value, 
					   bool force = false);

	void add_parameter(int level, 
					   const char* id, 
					   int value, 
					   int default_value, 
					   bool force = false);

	void add_parameter(int level, 
					   const char* id, 
					   unsigned int value, 
					   unsigned int default_value,
					   bool force = false);

	void add_parameter(int level, 
					   const char* id, 
					   bool value, 
					   bool default_value, 
					   bool force = false);

	void add_parameter(int level, 
					   const char* id, 
					   double value, 
					   double default_value, 
					   bool force = false);
};


/*/////////////////////////////////////////////////////////////////////
	Методы для вывода в строку, в формате, который будет понимать 1С
	Каждое значение заключается в кавычки; значения отделяются запятыми
/////////////////////////////////////////////////////////////////////*/
class CMMSString : public GCString
{
public:
	const CMMSString& operator=(LPCSTR lpsz)
		{
			((GCString*)this)->operator=(lpsz); 
			return *this;
		};

	void add_quoted(const char* value, bool add_comma = true);
	void add_quoted(int value, bool add_comma = true);
	void add_quoted(unsigned int value, bool add_comma = true);
	void add_quoted(bool value, bool add_comma = true);
	void add_quoted(double value, bool add_comma = true);
};

//Read Pascal-like string from binary stream
bool ReadSizedString(GCString& String, IStream* pStream);
//Write Pascal-like string into binary stream
void WriteSizedString(IStream* pStream, GCString& String);

//Преобразует строку в bool (1, 0) (Да, Нет)
bool StrToBool(const char* str);

//Функции транслитерации
GCString Translit(GCString& str);
void InplaceTranslit(GCString& str);

#endif