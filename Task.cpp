//////////////////////////////////////////////////////////////////////
// Task.cpp: implementation of the CTask class.
//////////////////////////////////////////////////////////////////////

#include "Task.h"
#include "CommonFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////
//// class CTaskParameters  //////////////////////////////
//////////////////////////////////////////////////////////
void TestArg(const TCHAR* arg, int argc, int i)
{
	if( ++i >= argc )
	{
		Msg(0, "ERR: Не задан параметр ключа %s", arg);
		exit(1);
	}
}

void CTaskParameters::ParseCommandLine(int argc, TCHAR* argv[])
{
	TCHAR *DirName  = "SRC";
	TCHAR *FileName = "1cv7.md";
	bool PreserveDir = false;

	if (argc <= 1) //no arguments at all
	{
		TaskType = help;
		return;
	}

	for( int i=1; i < argc; i++ )
	{
		TCHAR *arg;

		arg = argv[i];
		if( 0 == strcmp("-h", arg) )
		{
			TaskType = help;
		}
		else if( 0 == strcmp("--version", arg) )
		{
			TaskType = show_version;
		}
		else if( 0 == strcmp("-c", arg) )
		{
			TaskType = compile;
			FileName = "1cv7.new.md"; //на всякий случай
		}
		else if( 0 == strcmp("-d", arg) )
		{
			TaskType = decompile;
		}
		else if( 0 == strcmp("-v", arg) )
		{
			Verbose = 1;
		}
		else if( 0 == strcmp("-vv", arg) )
		{
			Verbose = 2;
		}
		else if( 0 == strcmp("-vvv", arg) )
		{
			Verbose = 3;
		}
		else if( 0 == strcmp("-q", arg) )
		{
			Verbose = -1;
		}
		else if( 0 == strcmp("-D", arg) )
		{
			TestArg(arg, argc, i);
			arg = argv[++i];
			DirName = arg;
		}
		else if( 0 == strcmp("-DD", arg) )
		{
			TestArg(arg, argc, i);
			arg = argv[++i];
			DirName = arg;
			PreserveDir = true;
		}
		else if( 0 == strcmp("-F", arg) )
		{
			TestArg(arg, argc, i);
			arg = argv[++i];
			FileName = arg;
		}
		else if( 0 == strcmp("--external-report", arg) )
		{
			CompoundType = external_report;
		}
		else if( 0 == strcmp("--meta-data", arg) )
		{
			CompoundType = meta_data;
		}
		else if( 0 == strcmp("--no-order", arg) )
		{
			NoOrdering = true;
		}
		else if( 0 == strcmp("--no-profiles", arg) )
		{
			NoProfiles = true;
		}
		else if( 0 == strcmp("--no-empty-mxl", arg) )
		{
			NoEmptyMxl = true;
		}
		else if( 0 == strcmp("--truncate-mms", arg) )
		{
			TruncateMMS = true;
		}
		else if( 0 == strcmp("--eng", arg) )
		{
			Language = lang_Eng;
		}
		else if( 0 == strcmp("--translit", arg) )
		{
			Language = lang_Translit;
		}
		else if( 0 == strcmp("--no-defaults", arg) )
		{
			NoDefaults = true;
		}
		else if( 0 == strcmp("--no-version", arg) )
		{
			SaveVersion = false;
		}
		else if( 0 == strcmp("--filter", arg) )
		{
			TestArg(arg, argc, i);
			arg = argv[++i];
			MetadataPath = arg;
		}
		else if( 0 == strcmp("--more-subfolders", arg) )
		{
			MetadataBySubfolders = true;
		}
		else if( 0 == strcmp("--no-empty-folders", arg) )
		{
			NoEmptyFolders = true;
		}
		else if( 0 == strcmp("--sort-rights", arg) )
		{
			SortUserRightsObjects = true;
		}
		else if( 0 == strcmp("--no-parse-dialogs", arg) )
		{
			NotParseForms = true;
		}
		else if( 0 == strcmp("--sort-form-controls", arg) )
		{
			SortFormControls = true;
		}
		else if( 0 == strcmp("--no-ini", arg) )
		{
			CreateIniFile = false;
		}
		else if( 0 == strcmp("--no-broken-links", arg) )
		{
			NoBrokenLinks = true;
		}
		else if( 0 == strcmp("--save-all-rights", arg) )
		{
			MakeUserRightsInOneFile = true;
		}
		//Экспериментальная опция. Нужно будет убрать.
		else if( 0 == strcmp("--name-tr", arg) )
		{
			TestArg(arg, argc, i);
			arg = argv[++i];
			ObjectName = arg;
			TaskType = name_translation;
		}
		else
		{
			Msg(0,"Invalid option '%s'", arg);
			TaskType = help;
			return;
		}
	}

	//Copy file and dir names into class variables
	csCompoundFileName = FileName;

	//Split file name on parts
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(FileName, NULL, NULL, fname, ext);

	//Try to determine file type by extension
	if( CompoundType == by_extension )
	{
		if( stricmp(ext, ".ert") == 0 )
			CompoundType = external_report;
		else
			CompoundType = meta_data;
	}

	// отрежем заключительные \, если есть
	i = strlen(DirName) - 1;
	while( i >= 0 && DirName[i] == '\\' ) DirName[i--] = '\0';

	//In case of external report add its name to directory. 
	//When directory specified explicitly leave its name unchanged
	if( (CompoundType == external_report) && !PreserveDir )
		csDirectoryName.Format("%s\\%s", DirName, fname);
	else
		csDirectoryName = DirName;

	//Даже если пользователь и указал опцию, мы плюнем на неё если это не внешний отчет
	if( (CompoundType != external_report) ) TruncateMMS = false;

	DirectoryName = (LPSTR)(LPCSTR)csDirectoryName;
	CompoundFileName = (LPSTR)(LPCSTR)csCompoundFileName;
}


//////////////////////////////////////////////////////////
//// class CTask   ///////////////////////////////////////
//////////////////////////////////////////////////////////
CTask::CTask(int argc, TCHAR* argv[])
{
	GComp_SetDefaults(&TaskParameters);
	TaskParameters.ParseCommandLine(argc, argv);
}


void CTask::Action()
{
	switch( TaskParameters.TaskType )
	{
		case CTaskParameters::show_version:
			Msg(0, "GComp version %s", GComp_GetVersion());
			break;
		case CTaskParameters::help:
			Help();
			break;
		case CTaskParameters::compile:
			Compile();
			break;
		case CTaskParameters::decompile:
			Decompile();
			break;
		//Экспериментальный код. Нужно будет убрать.
		case CTaskParameters::name_translation:
			GComp_MakeNamePath(TaskParameters.DirectoryName, FALSE, 
				(char*)(LPCSTR)TaskParameters.ObjectName, 0);
			break;
	}
}


void CTask::Decompile()
{
	CTime Time = CTime::GetCurrentTime();
	if( TaskParameters.Verbose > 1) Msg(0, "\nExtraction started: %s", Time.Format("%Y.%m.%d %H:%M:%S"));

	GComp_ReadSettings(&TaskParameters);
	if( !GComp_Decompile(&TaskParameters) )
		exit(1);

	if( TaskParameters.CreateIniFile )
		GComp_SaveSettings(&TaskParameters);

	if( TaskParameters.CompoundType != external_report || TaskParameters.Verbose > 1)
	{
		CTimeSpan ExtractionTime = CTime::GetCurrentTime() - Time;
		Msg(0, "'%s' extracted into '%s' in %s", 
			TaskParameters.CompoundFileName, TaskParameters.DirectoryName, 
			ExtractionTime.Format("%H:%M:%S"));
	}
}

void CTask::Compile()
{
	CTime Time = CTime::GetCurrentTime();
	if( TaskParameters.Verbose > 1) Msg(0, "\nCompilation started: %s", Time.Format("%Y.%m.%d %H:%M:%S"));

	GComp_ReadSettings(&TaskParameters);
	if( !GComp_Compile(&TaskParameters) )
		exit(1);

	CTimeSpan CompilationTime = CTime::GetCurrentTime() - Time;
	Msg(0, "Dir '%s' compiled into '%s' in %s", 
		TaskParameters.DirectoryName, TaskParameters.CompoundFileName, 
		CompilationTime.Format("%H:%M:%S"));
}
