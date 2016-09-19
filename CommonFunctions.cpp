
#include <sys/stat.h>
#include <direct.h>
#include <errno.h>

#include "Declarations.h"
#include "CommonFunctions.h"
#include "DLL\GComp.h"

bool IsValidFileName(GCString& String)
{
	int len = String.GetLength();
	for( int i = 0; i < len; i++ )
	{
		int c = (unsigned char)String[i];
		if( !__iscsym(c) ) return false;
	}

	return true;
}

bool FileExist(LPCSTR FileName)
{
	struct _stat stat;

	if( _stat(FileName, &stat) == -1 ) return false;
	if( (stat.st_mode & _S_IFREG) != 0 ) return true;
	//FName - is directory. What we have to do?
	return false;
}

long GetFileSize(GCString& FileName)
{
	struct _stat stat;
	_stat(FileName, &stat);
	return stat.st_size;
}

bool DirectoryExist(LPCSTR DirName)
{
	struct _stat stat;
	if( _stat(DirName, &stat) == -1 ) return false;
	if( (stat.st_mode & _S_IFDIR) != 0 ) return true;
	//DirName - is ordinary file. What we have to do?
	return false;
}

void DumbCreateDirectory(char *dir, bool IsUNCPath)
{
	if( *dir == '\0' ) return;

	if( DirectoryExist(dir) ) return;

	if( 0 == _mkdir(dir) ) return;
	if( IsUNCPath && errno == EEXIST ) return; //_stat() не умеет работать с UNC путями

	Msg(0, "Can not create directory '%s' - %s", dir, strerror(errno));
	throw NULL;
}

void CreateDirectory(GCString&  DirName)
{
	char dir[MAX_PATH];
	char *pointer;
	int level = 0;
	bool IsUNCPath = false;

	strcpy(dir, DirName);
	pointer = dir;
	if( dir[0] == '\\' && dir[1] == '\\' ) IsUNCPath = true;

	do
	{
		level++;

		pointer = strchr(pointer, '\\');
		if( pointer != NULL )
		{
			if (':' == *(pointer - 1))  // Пути бывают и абсолютные, а каталог c:\ нам никогда не создать.
			{
				pointer++;
				continue;
			}
			else if( IsUNCPath && level <= 3 )
			{
				pointer++;
				continue;
			}
			else // временно поставим сюда конец строки
				*pointer = '\0';
		}

		DumbCreateDirectory(dir, IsUNCPath);

		if( pointer != NULL )
		{
			*pointer = '\\'; // снимем временный конец строки, вернем то, что там было.
			pointer++;
		}
	} while( pointer != NULL );
}

CStringArray* SplitPath(GCString& Path)
{
	CStringArray *parts = new CStringArray;
	int pos = 0, prev_pos = 0;
	bool HasSubdirs = false;
	while( (pos = Path.Find("\\", pos)) >= 0 )
	{
		HasSubdirs = true;
		GCString mid = Path.Mid(prev_pos, pos-prev_pos);
		parts->Add(mid);
		pos++;
		prev_pos = pos;
	}

	if( !HasSubdirs )
		parts->Add(Path);
	else
		parts->Add(Path.Mid(prev_pos));

	return parts;
}

void CopyFile(FILE* dest, FILE* src)
{
	char buf[1024];
	rewind(src);
	rewind(dest);
	while( !feof(src) )
	{
		int read = fread(buf, 1, sizeof(buf), src);
		fwrite(buf, 1, read, dest);
	}
}

bool IsIgnoredFolder(LPCSTR FileName)
{
	if( 0 == strcmp(FileName, ".")	) return true;
	if( 0 == strcmp(FileName, "..")	) return true;
	
	return false;
}

bool IsIgnoredFile(LPCSTR FileName)
{
	if( 0 == stricmp(FileName, "CVS")       ) return true;
	if( 0 == stricmp(FileName, ".svn")		) return true;
	if( 0 == stricmp(FileName, "ChangeLog") ) return true;
	if( 0 == stricmp(FileName, "gcomp.ini") ) return true;

	return IsIgnoredFolder(FileName);
}

bool FilesExist(LPCSTR Mask)
{
	struct _finddata_t find_data;
	long hFind;
	bool found = false;

	hFind = _findfirst(Mask, &find_data);
	if( hFind == -1 ) return false;
	do
	{
		if( IsIgnoredFile(find_data.name) ) continue;
		found = true;
		break;
	} while( _findnext(hFind, &find_data) == 0 );
	_findclose(hFind);

	return found;
}

//Меняет все нежелательные буквы на '_'. Замена производится прямо в строке
int MangleFileName(GCString& str)
{
	char* ptr = (char*)(LPCSTR)str;
	int repl_count = 0;
	while( *ptr != '\0' )
	{
		int c = (unsigned char)(*ptr); //unsigned - это для Русских буквочек
		if( !__iscsym(c) ) { *ptr = '_'; repl_count++; };
		ptr++;
	}
	return repl_count;
}

GCString Suffix(GCString& str)
{
	int pos = str.Find('.');
	if( pos < 0 ) return "";
	return str.Mid(pos+1);
}

void SystemErrorMessage()
{
	char *ErrMsg = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 
		NULL, 
		GetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		ErrMsg, 
		0, NULL);
	printf(ErrMsg);
	fflush(stdout);
	LocalFree(ErrMsg);
}

void Msg(int MinimalVerbosityLevel, char *msg, ...)
{
	if( MinimalVerbosityLevel <= TaskParameters.Verbose )
	{
		GCString ConvertedMsg;
		va_list ap;
		va_start(ap, msg);
		ConvertedMsg.FormatV(msg, ap);
		va_end(ap);


		switch( TaskParameters.LogType )
		{
		case LogType_stdout:
			ConvertedMsg.AnsiToOem();
			printf((LPCSTR)ConvertedMsg);
			printf("\n");
			fflush(stdout);
			break;
		case LogType_ListBox:
			{
			SendMessage(TaskParameters.hwndLog, LB_ADDSTRING, 0, (LPARAM)(LPCSTR)ConvertedMsg);
			int Count = SendMessage(TaskParameters.hwndLog, LB_GETCOUNT, 0, 0);
			SendMessage(TaskParameters.hwndLog, LB_SETCURSEL, Count-1, 0);
			SendMessage(TaskParameters.hwndLog, WM_PAINT, 0, 0);
			}
			break;
		}
	}
}

void PrintTime()
{
	CTime Time = CTime::GetCurrentTime();
	Msg(0, "Time: %s", Time.Format("%H:%M:%S"));
}

