//=============================================================
//            Alexey Dirks 2004
//            eMail: Adirks@ngs.ru
//=============================================================
unit GComp;

interface

uses Windows, Classes;

const
{compound type constants}
GComp_external_report = 1;
GComp_meta_data = 2;

{languages constants}
GComp_lang_Rus = 0;
GComp_lang_Eng = 1;
GComp_lang_Translit = 2;

{Log type constants}
GComp_LogToStdout  = 0;
GComp_LogToFile    = 1;
GComp_LogToListBox = 2;

{Log messages constants - passed in wParam}
GComp_Inf_Start = 0;
GComp_Inf_Stop = 1;
GComp_Inf_Total = 2;     //в lParam передаётся общее количество объектов
GComp_Inf_Progress = 3;  //в lParam передаётся количество обработанных объектов

type
TGCompParameters = record
	CompoundFileName: LPSTR;
	DirectoryName: LPSTR;
	CompoundType: Integer;
	Language: Integer;
	Verbose: Integer;
	LogType: Integer;
	hwndLog: HWND;
	NoOrdering: BOOL; // порядок следования объектов
	NoProfiles: BOOL;
	NoEmptyMxl: BOOL;
	TruncateMMS: BOOL;
	NotParseForms: BOOL; //диалоговые формы будут выгружены AsIs
	NoDefaults: BOOL;
	SaveVersion: BOOL; //save gcomp's version into file gcomp.ver
	NoEmptyFolders: BOOL;
	MetadataBySubfolders: BOOL;
	MetadataPath: LPSTR;
	SaveWholeMMS: BOOL; //unused
	SortUserRightsObjects: BOOL;
	SortFormControls: BOOL;
	hwndInfoReceiver: HWND;
	InfoMessage: UINT;
	CancelWork: BOOL;
	NoBrokenLinks: BOOL;

	Reserved1: DWORD;
	Reserved2: DWORD;
	Reserved3: DWORD;
	Reserved4: DWORD;
	//=== params for safety in Delphi ===
	strCompoundFileName: String;
	strDirectoryName: String;
	strMetadataPath: String;
end;
PTGCompParameters = ^TGCompParameters;

//======================================================================================
//======== Thread classes ==============================================================
// Note that threads created suspended - so we need call Resume() after creation
//======================================================================================
TDecompileThread = class(TThread)
	private
		Params: PTGCompParameters;

	public
		Success: Boolean;
		constructor Create(aParams: PTGCompParameters);

	protected
		procedure Execute; override;
end;

TCompileThread = class(TThread)
	private
		Params: PTGCompParameters;

	public
		Success: Boolean;
		constructor Create(aParams: PTGCompParameters);

	protected
		procedure Execute; override;
end;

//==== DLL imports =====================================================================
function  GComp_GetVersion(): LPCSTR; cdecl; external 'gcomp.dll' name 'GComp_GetVersion';
procedure GComp_SetDefaults(Parameters: PTGCompParameters); cdecl; external 'gcomp.dll' name 'GComp_SetDefaults';
function  GComp_Decompile(Parameters: PTGCompParameters): BOOL; cdecl; external 'gcomp.dll' name 'GComp_Decompile';
function  GComp_Compile(Parameters: PTGCompParameters): BOOL; cdecl; external 'gcomp.dll' name 'GComp_Compile';
procedure GComp_SaveSettings(Parameters: PTGCompParameters); cdecl; external 'gcomp.dll' name 'GComp_SaveSettings';
procedure GComp_ReadSettings(Parameters: PTGCompParameters); cdecl; external 'gcomp.dll' name 'GComp_ReadSettings';
implementation

//------------------------------------------------------------------------------
constructor TDecompileThread.Create(aParams: PTGCompParameters);
begin
	Params := aParams;
	inherited Create(true);
	FreeOnTerminate := false;
end;

procedure TDecompileThread.Execute();
begin
	Success := GComp_Decompile(Params);
end;

//------------------------------------------------------------------------------
constructor TCompileThread.Create(aParams: PTGCompParameters);
begin
	Params := aParams;
	inherited Create(true);
	FreeOnTerminate := false;
end;

procedure TCompileThread.Execute();
begin
	Success := GComp_Compile(Params);
end;

end.



 
