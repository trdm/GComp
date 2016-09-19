//=============================================================
//            Alexey Dirks 2004
//            eMail: Adirks@ngs.ru
//=============================================================
unit GComp;

interface

uses Windows;

const
{compound type constants}
GComp_external_report = 1;
GComp_meta_data = 2;

{languages constants}
GComp_lang_Rus = 0;
GComp_lang_Eng = 1;
GComp_lang_Translit = 2;

GComp_LogToStdout = 0;
GComp_LogToListBox = 2;

type

TGCompParameters = record
	CompoundFileName : LPSTR;
	DirectoryName : LPSTR;
	CompoundType : Integer;
	Language : Integer;
	Verbose : Integer;
	LogType : Integer;
	hwndLog : HWND;
	NoOrdering : BOOL; // порядок следования объектов
	NoProfiles : BOOL;
	NoEmptyMxl : BOOL;
	TruncateMMS : BOOL;
	NotParseForms : BOOL;
	NoDefaultsInDialogs : BOOL;
	SaveVersion : BOOL; //save gcomp's version into file gcomp.ver
	MetadataBySubfolders : BOOL;
	MetadataPath : LPSTR;
end;

PTGCompParameters = ^TGCompParameters;

function GComp_GetVersion(): LPCSTR; external 'gcomp.dll';

procedure GComp_SetDefaults(Parameters: PTGCompParameters); stdcall;
procedure GComp_SetDefaults(Parameters: PTGCompParameters); external 'gcomp.dll';

function GComp_Decompile(Parameters: PTGCompParameters): BOOL; stdcall;
function GComp_Decompile(Parameters: PTGCompParameters): BOOL; external 'gcomp.dll';

function GComp_Compile(Parameters: PTGCompParameters): BOOL; stdcall;
function GComp_Compile(Parameters: PTGCompParameters): BOOL; external 'gcomp.dll';

implementation


end.
 