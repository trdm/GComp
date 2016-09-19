
#include "Strings.h"
#include "DLL\GComp.h"

const GCString GCString::EmptyString;

/*
int GCString::Resize(int delta)
{
	int OldDataLen = GetData()->nDataLength;
	int Len = GetLength();
	int NewLen = Len + delta;
	int AllocLen = GetData()->nAllocLength;
	if( NewLen > AllocLen )
	{
		int NewAllocLen = GetData()->nAllocLength * 2;
		if( NewAllocLen < 256 )
			NewAllocLen = 256;
		while( NewAllocLen <= NewLen )
			NewAllocLen *= 2;
		GetBufferSetLength(NewAllocLen);
	}

	GetData()->nDataLength = OldDataLen;
	return GetData()->nAllocLength;
}

const GCString& GCString::operator+=(TCHAR ch)
{
	//Resize(1);
	return CString::operator+=(ch);
}

const GCString& GCString::operator+=(LPCTSTR lpsz)
{
	//Resize(strlen(lpsz));
	return CString::operator+=(lpsz);
}

const GCString& GCString::operator+=(const GCString& str)
{
	//Resize(str.GetLength());
	return CString::operator+=(str);
}
//*/

GCString operator+(const GCString& string1, const GCString& string2)
{
	return operator+((const CString&)string1, (const CString&)string2);
};

GCString operator+(const GCString& string, const char* str)
{
	return operator+((const CString&)string, str);
}

GCString operator+(const char* str, const GCString& string)
{
	return operator+(str, (const CString&)string);
}


bool GCString::IsNumber()
{
	int iVal = atoi((LPCSTR)(*this));
	char str[32];
	return this->Compare(itoa(iVal, str, 10)) == 0;
}

void GCString::Add(const char* str, int count)
{
	while( count-- > 0 ) operator +=(str);
}



//Все символы перевода строки заменяем на "\n", и соответственно "\" -> "\\"
//Замена производится прямо в строке - строк с такой фигнёй не должно быть много, 
//поэтому inplace замена будет выгоднее.
void GCString::quote_linefeeds()
{
	Replace("\\", "\\\\");
	Replace("\r\n", "\\n");
	Replace("\n", "\\n");
}
//Обратное преобразование
void GCString::unquote_linefeeds()
{
	int len = GetLength();
	
	for( int i = 0; i < len; i++ )
	{
		if( GetAt(i) != '\\' ) continue;
		switch( GetAt(i+1) )
		{
		case 'n':
			SetAt(i++,  '\r');
			SetAt(i,  '\n');
			break;
		case '\\':
			Delete(i);
			len--;
			break;
		}
	}
}


/*////////////////////////////////////////////////////////////////////////////////
 Методы для "красивого" вывода параметров в текст. Предполагается, что файл состоит
 из строчек вида
 <ИмяПараметра>: [ЗначениеПараметра]
////////////////////////////////////////////////////////////////////////////////*/

void CBeautyString::add_offset(int level)
{
	Add("\t", level);
}

void CBeautyString::add_brace(int level, const char brace)
{
	char str[] = "{\r\n";
	Add("\t", level);
	str[0] = brace;
	operator+= (str);
}

void CBeautyString::add_parameter(int level, const char* id, const char* value, const char* default_value, bool force)
{
	if( !force )
	{
		if( value == default_value ) return;
		if( value == NULL ) return;
		if( default_value == NULL && value[0] == '\0' ) return;
		if( default_value != NULL && strcmp(value, default_value) == 0) return;
	}

	add_offset(level);
	operator+=(id);
	operator+=(": ");
	operator+=(value);
	operator+=("\r\n");
}

void CBeautyString::add_parameter(int level, const char* id, int value, int default_value, bool force)
{
	if( !force && value == default_value ) return;

	char str[64];
	sprintf(str, "%i", value);
	add_parameter(level, id, str, "");
}

void CBeautyString::add_parameter(int level, const char* id, unsigned int value, unsigned int default_value, bool force)
{
	if( !force && value == default_value ) return;

	char str[64];
	sprintf(str, "0x%x", value);
	add_parameter(level, id, str, "");
}

void CBeautyString::add_parameter(int level, const char* id, bool value, bool default_value, bool force)
{
	if( !force && value == default_value ) return;

	if( value )
		add_parameter(level, id, "Да", "");
	else
		add_parameter(level, id, "Нет", "");
}

void CBeautyString::add_parameter(int level, const char* id, double value, double default_value, bool force)
{
	if( !force && value == default_value ) return;

	char str[64];
	sprintf(str, "%g", value);
	add_parameter(level, id, str, "");
}



/*/////////////////////////////////////////////////////////////////////
	Методы для вывода в строку, в формате, который будет понимать 1С
	Каждое значение заключается в кавычки; значения отделяются запятыми
/////////////////////////////////////////////////////////////////////*/

void CMMSString::add_quoted(const char* value, bool add_comma)
{
	GCString str;

	if( add_comma )
		str = ",\"";
	else
		str = '"';
	str += value;
	str += '"';
	operator+= (str);
}

void CMMSString::add_quoted(int value, bool add_comma)
{
	char str[64];
	sprintf(str, "%i", value);
	add_quoted(str, add_comma);
}
void CMMSString::add_quoted(unsigned int value, bool add_comma)
{
	char str[64];
	sprintf(str, "%u", value);
	add_quoted(str, add_comma);
}
void CMMSString::add_quoted(bool value, bool add_comma)
{
	if( value )
		add_quoted("1", add_comma);
	else
		add_quoted("0", add_comma);
}
void CMMSString::add_quoted(double value, bool add_comma)
{
	char str[64];
	sprintf(str, "%g", value);
	add_quoted(str, add_comma);
}

//Read Pascal-like string from binary stream
bool ReadSizedString(GCString& String, IStream* pStream)
{
	BYTE len;
	char buf[256];
	ULONG BytesRead;

	pStream->Read(&len, sizeof(len), &BytesRead);
	if( BytesRead != sizeof(len) ) return false;
	pStream->Read(buf, len, &BytesRead);
	if( BytesRead != len ) return false;

	buf[len] = '\0';
	String = buf;
	return true;
}

//Write Pascal-like string into binary stream
void WriteSizedString(IStream* pStream, GCString& String)
{
	BYTE len = String.GetLength();
	pStream->Write(&len, sizeof(len), NULL);
	if( len > 0 ) pStream->Write((LPCSTR)String, len, NULL);
}

bool StrToBool(const char* str)
{
	if( strcmp(str, "1") == 0 ) return true;
	if( _stricoll(str, "Да") == 0 ) return true;
	return false;
}


//Класс нужен только для того, чтобы при создании статического объекта 
//он автоматом инициализировался
class CTranslitTable
{
public:
	static CMapWordToPtr Map;
public:
	CTranslitTable();
};
CMapWordToPtr CTranslitTable::Map;

CTranslitTable::CTranslitTable()
{
	Map.SetAt('а', "a");
	Map.SetAt('б', "b");
	Map.SetAt('в', "v");
	Map.SetAt('г', "g");
	Map.SetAt('д', "d");
	Map.SetAt('е', "e");
	Map.SetAt('ё', "jo");
	Map.SetAt('ж', "zh");
	Map.SetAt('з', "z");
	Map.SetAt('и', "i");
	Map.SetAt('й', "j");
	Map.SetAt('к', "k");
	Map.SetAt('л', "l");
	Map.SetAt('м', "m");
	Map.SetAt('н', "n");
	Map.SetAt('о', "o");
	Map.SetAt('п', "p");
	Map.SetAt('р', "r");
	Map.SetAt('с', "s");
	Map.SetAt('т', "t");
	Map.SetAt('у', "u");
	Map.SetAt('ф', "f");
	Map.SetAt('х', "h");
	Map.SetAt('ц', "c");
	Map.SetAt('ч', "ch");
	Map.SetAt('ш', "sh");
	Map.SetAt('щ', "shh");
	Map.SetAt('ъ', "#");
	Map.SetAt('ы', "y");
	Map.SetAt('ь', "'");
	Map.SetAt('э', "je");
	Map.SetAt('ю', "ju");
	Map.SetAt('я', "ja");

	Map.SetAt('А', "A");
	Map.SetAt('Б', "B");
	Map.SetAt('В', "V");
	Map.SetAt('Г', "G");
	Map.SetAt('Д', "D");
	Map.SetAt('Е', "E");
	Map.SetAt('Ё', "Jo");
	Map.SetAt('Ж', "Zh");
	Map.SetAt('З', "Z");
	Map.SetAt('И', "I");
	Map.SetAt('Й', "J");
	Map.SetAt('К', "K");
	Map.SetAt('Л', "L");
	Map.SetAt('М', "M");
	Map.SetAt('Н', "N");
	Map.SetAt('О', "O");
	Map.SetAt('П', "P");
	Map.SetAt('Р', "R");
	Map.SetAt('С', "S");
	Map.SetAt('Т', "T");
	Map.SetAt('У', "U");
	Map.SetAt('Ф', "F");
	Map.SetAt('Х', "H");
	Map.SetAt('Ц', "C");
	Map.SetAt('Ч', "Ch");
	Map.SetAt('Ш', "Sh");
	Map.SetAt('Щ', "Shh");
	Map.SetAt('Ъ', "#");
	Map.SetAt('Ы', "Y");
	Map.SetAt('Ь', "'");
	Map.SetAt('Э', "Je");
	Map.SetAt('Ю', "Ju");
	Map.SetAt('Я', "Ja");
}
static CTranslitTable TranslitTable;

void InplaceTranslit(GCString& str)
{
	if ( lang_Rus == TaskParameters.Language ) return;

	GCString Result;
	for (int i=0; i < str.GetLength(); i++)
	{
		TCHAR ch = str.GetAt(i);
		void *ptr;
		if( CTranslitTable::Map.Lookup(ch, ptr) )
			Result += (LPCSTR)ptr;
		else
			Result += ch;
	}

	str = Result;
}

GCString Translit(GCString& str)
{
	GCString Result = (LPCSTR)str;
	InplaceTranslit(Result);
	return Result;
}
