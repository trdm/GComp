
#include <io.h>
#include <locale.h>
#include <direct.h>
#include "DLL\GComp.h"
#include "Declarations.h"
#include "CommonFunctions.h"
#include "Lexer.h"
#include "MMS.h"
#include "Storage.h"
#include "NameTable.h"
#include "Ordering.h"
#include "MMS_Filter.h"
#include "DialogForm.h"

CGCompParameters* pPassedParameters = NULL;
CGCompParameters TaskParameters;
TTaskType TaskType;
CNameTable NameTable;
CMMSFilter MMS_Filter;
CMMS* MainMetadataStream = NULL;
int TotalObjectsProcessed = 0;

static char Version[32] = "";
LPCSTR GComp_GetVersion()
{
	GCString FileVersion;
	HINSTANCE hGcomp = GetModuleHandle("gcomp.dll");
	char fname[MAX_PATH];
	GetModuleFileName(hGcomp, fname, MAX_PATH);
	DWORD z, size;
	size = GetFileVersionInfoSize(fname, &z);
	if( size > 0 )
	{
		char* pVersion = new char[size];
		GetFileVersionInfo(fname, 0, size, pVersion);
		DWORD* translate;
		UINT len;
		VerQueryValue(pVersion, "\\VarFileInfo\\Translation", (void**)&translate, &len);
		GCString blockName;
		blockName.Format("\\StringFileInfo\\%04x%04x\\FileVersion", LOWORD(*translate), HIWORD(*translate));
		LPCTSTR valuebuf;
		VerQueryValue(pVersion, (LPTSTR)(LPCTSTR)blockName, (void**)&valuebuf, &len);
		if( len > 0 )
			FileVersion = valuebuf+2;
		delete pVersion;
	}

	FileVersion.Replace(" ", "");
	FileVersion.Replace(',', '.');
	sprintf(Version, "%s.beta", FileVersion);
	return Version;
}


void GComp_SetDefaults(CGCompParameters* Parameters) 
{
	Parameters->CompoundFileName = NULL;
	Parameters->DirectoryName = NULL;

	Parameters->CompoundType = by_extension;
	Parameters->Verbose = 0;
	Parameters->LogType = LogType_stdout;
	Parameters->hwndLog = NULL;
	Parameters->NoOrdering = FALSE;
	Parameters->NoProfiles = FALSE;
	Parameters->NoEmptyMxl = FALSE;
	Parameters->TruncateMMS = FALSE;
	Parameters->NotParseForms = FALSE;
	Parameters->NoDefaults = FALSE;
	Parameters->Language = lang_Rus;
	Parameters->SaveVersion = TRUE;
	Parameters->NoEmptyFolders = FALSE;
	Parameters->MetadataBySubfolders = FALSE;
	Parameters->MetadataPath = NULL;
	Parameters->SaveWholeMMS = TRUE;
	Parameters->SortUserRightsObjects = FALSE;
	Parameters->SortFormControls = FALSE;
	Parameters->hwndInfoReceiver = NULL;
	Parameters->InfoMessage = 0;
	Parameters->CancelWork = FALSE;
	Parameters->NoBrokenLinks = FALSE;
	Parameters->MakeUserRightsInOneFile = FALSE;
};

BOOL YesNoToBool(GCString& token, GCString& value)
{
	if( value.CollateNoCase("yes")   == 0 ) return true;
	if( value.CollateNoCase("y")     == 0 ) return true;
	if( value.CollateNoCase("да")    == 0 ) return true;
	if( value.CollateNoCase("no")    == 0 ) return false;
	if( value.CollateNoCase("n")     == 0 ) return false;
	if( value.CollateNoCase("нет")   == 0 ) return false;

	Msg(0, "ERR: Неизвестное значение '%s' параметра %s в gcomp.ini", value, token);
	throw NULL;
}

TGComp_Languages ReadLanguage(GCString& token, GCString& value)
{
	if( value.CompareNoCase("rus") == 0 )
		return lang_Rus;
	else if( value.CompareNoCase("eng") == 0 )
		return lang_Eng;
	else if( value.CompareNoCase("translit") == 0 )
		return lang_Translit;
	else
	{
		Msg(0, "ERR: Неизвестный язык '%s' в gcomp.ini", value);
		throw NULL;
	}
}

void GComp_ReadSettings(CGCompParameters* Parameters)
{
	if( Parameters->DirectoryName == NULL )
		return;
	if( strlen(Parameters->DirectoryName) == 0 )
		return;

	GCString ini_name;
	ini_name.Format("%s\\gcomp.ini", Parameters->DirectoryName);
	if( !FileExist(ini_name) ) return;

	CLexer ini_file(ini_name);
	GCString token, value;
	while( ini_file.GetToken(token, "=: \t\r\n") )
	{
		ini_file.GetRest(value);
		if( stricmp(token, "no-ordering") == 0 )
			Parameters->NoOrdering = YesNoToBool(token, value);
		
		else if( stricmp(token, "no-profiles") == 0 )
			Parameters->NoProfiles = YesNoToBool(token, value);
		
		else if( stricmp(token, "no-empty-mxl") == 0 )
			Parameters->NoEmptyMxl = YesNoToBool(token, value);
		
		else if( stricmp(token, "no-defaults") == 0 )
			Parameters->NoDefaults = YesNoToBool(token, value);

		else if( stricmp(token, "no-parse-forms") == 0 )
			Parameters->NotParseForms = YesNoToBool(token, value);

		else if( stricmp(token, "language") == 0 )
			Parameters->Language = ReadLanguage(token, value);

		else if( stricmp(token, "more-subfolders") == 0 )
			Parameters->MetadataBySubfolders = YesNoToBool(token, value);

		else if( stricmp(token, "save-all-rights") == 0 )
			Parameters->MakeUserRightsInOneFile = YesNoToBool(token, value);

		else if( stricmp(token, "save-whole-mms") == 0 ) {}	//do nothing

		else if( stricmp(token, "no-empty-folders") == 0 )
			Parameters->NoEmptyFolders = YesNoToBool(token, value);
		else if( stricmp(token, "sort-rights-objects") == 0 )
			Parameters->SortUserRightsObjects = YesNoToBool(token, value);
		else if( stricmp(token, "sort-form-controls") == 0 )
			Parameters->SortFormControls = YesNoToBool(token, value);

		else
		{
			Msg(0, "ERR: Неизвестный параметр '%s' в gcomp.ini", token);
			throw NULL;
		}
	}
}


inline char* BoolToYesNo(BOOL value)
{
	return value ? "Yes" : "No";
}

char* LanguageToStr(TGComp_Languages lang)
{
	switch( lang )
	{
	case lang_Rus:
		return "Rus";
		break;
	case lang_Eng:
		return "Eng";
		break;
	case lang_Translit:
		return "Translit";
		break;
	}
	Msg(0, "ERR: Неизвестный язык %i", (int)lang);
	throw NULL;
}

void GComp_SaveSettings(CGCompParameters* Parameters)
{
	if( Parameters->DirectoryName == NULL )
		return;
	if( strlen(Parameters->DirectoryName) == 0 )
		return;

	GCString ini_name;
	ini_name.Format("%s\\gcomp.ini", Parameters->DirectoryName);

	CBeautyString str;
	str.add_parameter(0, "language",    LanguageToStr(Parameters->Language), "", true);
	str.add_parameter(0, "no-defaults", BoolToYesNo(Parameters->NoDefaults), "", true);
	str.add_parameter(0, "no-parse-forms", BoolToYesNo(Parameters->NotParseForms), "", true);
	str.add_parameter(0, "more-subfolders", BoolToYesNo(Parameters->MetadataBySubfolders), "", true);
	str.add_parameter(0, "no-profiles", BoolToYesNo(Parameters->NoProfiles), "", true);
	str.add_parameter(0, "no-empty-mxl", BoolToYesNo(Parameters->NoEmptyMxl), "", true);
	str.add_parameter(0, "no-ordering", BoolToYesNo(Parameters->NoOrdering), "", true);
	str.add_parameter(0, "no-empty-folders", BoolToYesNo(Parameters->NoEmptyFolders), "", true);
	str.add_parameter(0, "sort-rights-objects", BoolToYesNo(Parameters->SortUserRightsObjects), "", true);
	str.add_parameter(0, "sort-form-controls", BoolToYesNo(Parameters->SortFormControls), "", true);
	str.add_parameter(0, "save-all-rights", BoolToYesNo(Parameters->MakeUserRightsInOneFile), "", true);
	
	CreateDirectory(GCString(Parameters->DirectoryName));
	CStorage Storage;
	Storage.CopyStringToFile(str, ini_name);
}

void InitializeNameTable()
{
	if( lang_Eng == TaskParameters.Language )
		NameTable.EngConstructor();
	else
		NameTable.RusConstructor();
}

BOOL GComp_Decompile(CGCompParameters* Parameters)
{
	pPassedParameters = Parameters;
	pPassedParameters->CancelWork = false;
	TotalObjectsProcessed = 0;

	TaskParameters = *Parameters;
	TaskType = TaskType_Decompile;

	char* locale = setlocale( LC_ALL, ".ACP");
	GCString csDirectoryName = TaskParameters.DirectoryName;
	
	if( MainMetadataStream ) delete MainMetadataStream;
	MainMetadataStream = new CMMS;

	#ifndef _DEBUG
	try
	#endif
	{
		SendInfoMessage(InfoMsg_Start, 0);
		InitializeNameTable();

		MMS_Filter.Init(TaskParameters.MetadataPath);

		CStorage::InitZLibEngine();

		CStorage Storage;
		Storage.OpenCompound(GCString(TaskParameters.CompoundFileName));
		CStorage TypedTextStorage(Storage, "TypedText");

		CNameTableRecord* nt_rec = NameTable.Find(NT_MainMetadataStream);
		if( TaskParameters.CompoundType == external_report )
		{
			if( TaskParameters.TruncateMMS )
			{
				MainMetadataStream->TaskItem.Content.ID = 1;
				MainMetadataStream->BuhContainer.Buh.ID = 2;
			}
			else
			{
				Storage.ParseStream(MainMetadataStream, nt_rec->StorageName, false);
			}
		}
		else
		{
			Storage.Open(GCString("Metadata"));
			Storage.ParseStream(MainMetadataStream, nt_rec->StorageName, false);
			Storage.Close();
		}

		CreateDirectory(csDirectoryName); //create directory for decompyle to

		if( TaskParameters.SaveVersion )
		{
			GCString Version = GComp_GetVersion();
			GCString FName = csDirectoryName + "\\gcomp.ver";
			Storage.CopyStringToFile(Version, FName);
		}

		MainMetadataStream->Decompile(csDirectoryName, Storage, TypedTextStorage);

		if( MMS_Filter.IsEmpty() )
			Storage.PruneDirectory(csDirectoryName, TaskParameters.NoEmptyFolders);

		CStorage::ReleaseZLibEngine();
		SendInfoMessage(InfoMsg_Stop, 0);
	}
	#ifndef _DEBUG
	/* надо бы что-то такое замутить
	catch(CGCompError* e)
	{
		if( MMS != NULL ) delete MMS;
		e->LogError();
		return false;
	}
	*/
	catch(...)
	{
		if( MainMetadataStream != NULL ) delete MainMetadataStream;
		pPassedParameters = NULL;
		return FALSE;
	}
	#endif

	pPassedParameters = NULL;
	delete MainMetadataStream;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
//////  Compilation functions  ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
FILE* AssembleMMS();
extern int MMS_debug;

void MakeCopyOfMMS(FILE* mmsFile)
{
	FILE *f = fopen("MMS.mdp", "wb");
	char cwd[MAX_PATH];
	_getcwd(cwd, sizeof(cwd));
	Msg(0, "Создана копия потока MainMetadataStream в файле: %s\\MMS.mdp", cwd);
	CopyFile(f, mmsFile);
	fclose(f);
}

BOOL GComp_Compile(CGCompParameters* Parameters)
{
	pPassedParameters = Parameters;
	pPassedParameters->CancelWork = false;
	TotalObjectsProcessed = 0;

	TaskParameters = *Parameters;
	TaskType = TaskType_Compile;

	char* locale = setlocale( LC_ALL, ".ACP");
	GCString csDirectoryName = TaskParameters.DirectoryName;

	if( MainMetadataStream ) delete MainMetadataStream;
	MainMetadataStream = new CMMS;

#ifndef _DEBUG
	try
#endif
	{
		SendInfoMessage(InfoMsg_Start, 0);
		InitializeNameTable();
		MMS_Filter.Init(TaskParameters.MetadataPath);

		if( !DirectoryExist(csDirectoryName) )
		{
			Msg(0, "ERR: Source directory '%s' not exist", TaskParameters.DirectoryName);
			return FALSE;
		}
		CStorage Storage;

		if( MMS_Filter.IsEmpty() )
		{
			if( !Storage.CreateCompound(GCString(TaskParameters.CompoundFileName)) )
				return FALSE;

			MainMetadataStream->AssembleMetadata(csDirectoryName);

		}
		else
		{
			//При выборочной компиляции не создаём compound
			CNameTableRecord* nt_rec = NameTable.Find(NT_MainMetadataStream);
			Storage.OpenCompound(GCString(TaskParameters.CompoundFileName));
			Storage.Open(GCString("Metadata"));
			Storage.ParseStream(MainMetadataStream, nt_rec->StorageName, false);
			Storage.Close();
		}

		CStorage::InitZLibEngine();
		
		CStorage TypedTextStorage(Storage);
		if( TaskParameters.CompoundType != external_report )
			TypedTextStorage.Create(GCString("TypedText"));
		MainMetadataStream->Compile(Storage, TypedTextStorage, csDirectoryName);

		CStorage::ReleaseZLibEngine();
		SendInfoMessage(InfoMsg_Stop, 0);
	}
#ifndef _DEBUG
	catch(...)
	{
		if( MainMetadataStream != NULL ) delete MainMetadataStream;
		MainMetadataStream = NULL;
		pPassedParameters = NULL;
		return FALSE;
	}
#endif

	pPassedParameters = NULL;
	delete MainMetadataStream;
	MainMetadataStream = NULL;
	return TRUE;
}

BOOL GComp_ParseMainMetadataStream(LPCSTR fname, HWND hLogWnd)
{
	char* locale = setlocale( LC_ALL, ".ACP");
	GComp_SetDefaults(&TaskParameters);
	TaskParameters.hwndLog = hLogWnd;
	if( hLogWnd ) TaskParameters.LogType = LogType_ListBox;
	TaskType = TaskType_Decompile;
	
	InitializeNameTable();

	if( MainMetadataStream ) delete MainMetadataStream;

	MainMetadataStream = new CMMS;
	try
	{
		GCString FName(fname);
		CLexer Lexer(FName);
		MainMetadataStream->ParseMetadata(Lexer);
		MainMetadataStream->BuildObjNameCache();
	}
	catch(...)
	{
		Msg(0, "ошибка при разборе файла %s", fname);
		return FALSE;
	}

	return TRUE;
}

BOOL GComp_DecompileDialogForm(LPCSTR dest_fname, LPCSTR src_fname, int nObjectID)
{
	try
	{
		CMMSObject *pParent = MainMetadataStream->FindChild(nObjectID, true);
		Msg(0, "pParent = %i", pParent);
		GCString FName(src_fname);
		CLexer Lexer(FName);
		CDialogForm DialogForm(pParent);
		DialogForm.ParseMetadata(Lexer);
		CStorage Storage;
		DialogForm.WriteToFile(Storage, dest_fname);
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GComp_ReadObjectNameCache(LPCSTR fname, HWND hLogWnd)
{
	char* locale = setlocale( LC_ALL, ".ACP");
	GComp_SetDefaults(&TaskParameters);
	TaskParameters.hwndLog = hLogWnd;
	if( hLogWnd ) TaskParameters.LogType = LogType_ListBox;
	TaskType = TaskType_Compile;

	InitializeNameTable();

	if( MainMetadataStream ) delete MainMetadataStream;
	MainMetadataStream = new CMMS;

	if( !FileExist(fname) )
	{
		Msg(0, "ERR: Файл '%s' не существует", fname);
		return FALSE;
	}

	GCString FName(fname);
	try {
		return MainMetadataStream->ReadObjNameCache(FName);
	} catch(...)
	{
		Msg(0, "ошибка при разборе файла %s", fname);
	}
	return FALSE;
}

BOOL GComp_CompileDialogForm(LPCSTR dest_fname, LPCSTR src_fname)
{
	try 
	{
		GCString FName(src_fname);
		CLexer Lexer(FName);
		CDialogForm DialogForm(NULL);
		DialogForm.ParseFile(Lexer);
		CMMSString DlgAsString;
		DialogForm.ToString(DlgAsString);
		CStorage Storage;
		FName = dest_fname;
		Storage.CopyStringToFile(DlgAsString, FName);
	} 
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL GComp_AssembleMainMetadataStream(LPCSTR szDir, LPCSTR szDestFileName)
{
	char* locale = setlocale( LC_ALL, ".ACP");
	GComp_SetDefaults(&TaskParameters);
	TaskParameters.DirectoryName = (char*)szDir;
	TaskType = TaskType_Compile;
	GCString sDirectoryName = szDir;
	CStorage Storage;

	if( MainMetadataStream ) delete MainMetadataStream;

	try
	{
		MainMetadataStream = new CMMS;
		InitializeNameTable();
		MainMetadataStream->AssembleMetadata(sDirectoryName);

		MainMetadataStream->ReadObjNameCacheFromDir(sDirectoryName);
		MainMetadataStream->BuildObjNameCache(ObjNameCache_compare_str, sDirectoryName, Storage);

		MainMetadataStream->MainDataContDef.MD_ver = 10009;
		CMMSString sMMS;
		MainMetadataStream->ToString(sMMS);
		GCString sDestFileName = szDestFileName;
		Storage.CopyStringToFile(sMMS, sDestFileName);
	}
	catch(...)
	{
		return FALSE;
	}

	return TRUE;
}


bool NameToPath(GCString Name, GCString& Path);
BOOL GComp_MakeNamePath(LPCTSTR srcDir, BOOL translite, LPTSTR objName, int buffsize)
{
	GCString Name = objName, Path = srcDir;

	TaskParameters.Language = lang_Rus;
	InitializeNameTable();

	BOOL res = NameToPath(Name, Path);
	Msg(0, "Name: %s, Path: %s", Name, Path);
	return res;
}

void SendInfoMessage(TInfoMessageType MsgType, LPARAM lParam)
{
	if( pPassedParameters != NULL )
	{
		if( pPassedParameters->CancelWork )
		{
			Msg(0, "Work cancelled by user request");
			throw NULL;
		}
	}

	if( TaskParameters.hwndInfoReceiver == NULL )
		return;
	SendMessage(TaskParameters.hwndInfoReceiver, TaskParameters.InfoMessage, (WPARAM)MsgType, lParam);
}
