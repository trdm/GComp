unit SaveRestoreSettings;

interface

uses Windows, Registry, SysUtils, GComp;

const
	key_Group = 'Software\vgcomp';
	key_Verbosity = 'Verbosity';
	key_DecompileFileName = 'DecompileFileName';
	key_DecompileDir = 'DecompileDir';
	key_CompileFileName = 'CompileFileName';
	key_CompileDir = 'CompileDir';
	key_UseDecompileFilter = 'UseDecompileFilter';
	key_DecompileFilter = 'DecompileFilter';
	key_UseCompileFilter = 'UseCompileFilter';
	key_CompileFilter = 'CompileFilter';
	key_SaveVersion = 'SaveVersion';
	key_CompoundType = 'CompoundType';
	key_Language = 'Language';
	key_MetadataBySubfolders = 'MetadataBySubfolders';
	key_NoOrdering = 'NoOrdering';
	key_NoProfiles = 'NoProfiles';
	key_NoEmptyMxl = 'NoEmptyMxl';
	key_TruncateMMS = 'TruncateMMS';
	key_NoDefaults = 'NoDefaults';
	key_NotParseForms = 'NotParseForms';
	key_NoEmptyFolders = 'NoEmptyFolders';
	key_SortUserRightsObjects = 'SortUserRightsObjects';
	key_SortFormControls = 'SortFormControls';
	key_NoBrokenLinks = 'NoBrokenLinks';
	key_CreateIniFile = 'CreateIniFile';

	key_SyncDecompileDir = 'SyncDecompileDir';

type

TTaskType = (Decompile, Compile);

TMyRegistry = class(TRegistry)
	function ReadBool(const Name: String; const DefVal: Boolean): Boolean;
	function ReadInteger(const Name: String; const DefVal: Integer): Integer;
	function ReadString(const Name: String; const DefVal: String): String;
	constructor Create();

	procedure ReadAll();
	procedure WriteAll();

	procedure ToGCompParameters(var GCompParams: TGCompParameters; TaskType: TTaskType);
	procedure FromGCompParameters(var GCompParams: TGCompParameters; TaskType: TTaskType);

	procedure CheckForIniFile(Dir: String);
end;

implementation
uses vgc_MainForm;

constructor TMyRegistry.Create();
begin
	inherited;
	RootKey := HKEY_CURRENT_USER;
	OpenKey(key_Group, true);
end;

function TMyRegistry.ReadBool(const Name: String; const DefVal: Boolean): Boolean;
begin
	try
		ReadBool := inherited ReadBool(Name);
	except
		ReadBool := DefVal;
	end;
end;

function TMyRegistry.ReadInteger(const Name: String; const DefVal: Integer): Integer;
begin
	try
		ReadInteger := inherited ReadInteger(Name);
	except
		ReadInteger := DefVal;
	end;
end;

function TMyRegistry.ReadString(const Name: String; const DefVal: String): String;
var
	str: String;
begin
	try
		str := inherited ReadString(Name);
	except
		str := DefVal;
	end;
	if str = '' then
		str := DefVal;
	ReadString := str;
end;

//-------------------------------------------------------------------------------------------------
procedure TMyRegistry.ReadAll();
begin
	with MainForm do begin
		DecompileFileName.Text := ReadString(key_DecompileFileName, '1cv7.md');
		DecompileDirectory.Text := ReadString(key_DecompileDir, 'Src');
		cbUseDecompileFilter.Checked := ReadBool(key_UseDecompileFilter, false);
		DecompileFilter.Text := ReadString(key_DecompileFilter, '');
    cbUseDecompileFilter.OnClick(nil);

		CompileFileName.Text := ReadString(key_CompileFileName, ExtractFileDir(DecompileFileName.Text) + '\1cv7.new.md');
		CompileDirectory.Text := ReadString(key_CompileDir, DecompileDirectory.Text);
		cbUseCompileFilter.Checked := ReadBool(key_UseCompileFilter, false);
		CompileFilter.Text := ReadString(key_CompileFilter, '');
		cbUseCompileFilter.OnClick(nil);

		rgCompoundType.ItemIndex := ReadInteger(key_CompoundType, 0);

		cbSaveVersion.Checked := ReadBool(key_SaveVersion, true);

		rgVerbosity.ItemIndex := ReadInteger(key_Verbosity, 1);
		rgLanguage.ItemIndex := ReadInteger(key_Language, 0);

		cbMetadataBySubfolders.Checked := ReadBool(key_MetadataBySubfolders, false);
		cbNoOrdering.Checked := ReadBool(key_NoOrdering, false);
		cbNoProfiles.Checked := ReadBool(key_NoProfiles, false);
		cbNoEmptyMxl.Checked := ReadBool(key_NoEmptyMxl, false);
		cbNoDefaults.Checked := ReadBool(key_NoDefaults, false);
		cbNotParseForms.Checked := ReadBool(key_NotParseForms, false);
		cbNoEmptyFolders.Checked := ReadBool(key_NoEmptyFolders, false);

		cbTruncateMMS.Checked := ReadBool(key_TruncateMMS, false);

		cbSortUserRightsObjects.Checked := ReadBool(key_SortUserRightsObjects, false);
		cbSortFormControls.Checked := ReadBool(key_SortFormControls, false);
		cbNoBrokenLinks.Checked := ReadBool(key_NoBrokenLinks, false);

		cbCreateIniFile.Checked := ReadBool(key_CreateIniFile, false);

		cbSyncDecompileDir.Checked := ReadBool(key_SyncDecompileDir, true);
	end;
end;

procedure TMyRegistry.WriteAll();
begin
	with MainForm do begin
		WriteString(key_DecompileFileName, DecompileFileName.Text);
		WriteString(key_DecompileDir, DecompileDirectory.Text);
		WriteBool(key_UseDecompileFilter, cbUseDecompileFilter.Checked);
		WriteString(key_DecompileFilter, DecompileFilter.Text);

		WriteString(key_CompileFileName, CompileFileName.Text);
		WriteString(key_CompileDir, CompileDirectory.Text);
		WriteBool(key_UseCompileFilter, cbUseCompileFilter.Checked);
		WriteString(key_CompileFilter, CompileFilter.Text);

		WriteInteger(key_CompoundType, rgCompoundType.ItemIndex);

		WriteBool(key_SaveVersion, cbSaveVersion.Checked);
		WriteBool(key_MetadataBySubfolders, cbMetadataBySubfolders.Checked);

		WriteInteger(key_Verbosity, rgVerbosity.ItemIndex);
		WriteInteger(key_Language, rgLanguage.ItemIndex);

		WriteBool(key_NoOrdering, cbNoOrdering.Checked);
		WriteBool(key_NoProfiles, cbNoProfiles.Checked);
		WriteBool(key_NoEmptyMxl, cbNoEmptyMxl.Checked);
		WriteBool(key_NoDefaults, cbNoDefaults.Checked);
		WriteBool(key_NotParseForms, cbNotParseForms.Checked);
		WriteBool(key_NoEmptyFolders, cbNoEmptyFolders.Checked);

		WriteBool(key_TruncateMMS, cbTruncateMMS.Checked);

		WriteBool(key_SortUserRightsObjects, cbSortUserRightsObjects.Checked);
		WriteBool(key_SortFormControls, cbSortFormControls.Checked);
		WriteBool(key_NoBrokenLinks, cbNoBrokenLinks.Checked);

		WriteBool(key_CreateIniFile, cbCreateIniFile.Checked);

		WriteBool(key_SyncDecompileDir, cbSyncDecompileDir.Checked);
	end;
end;

//-------------------------------------------------------------------------------------------------
procedure TMyRegistry.ToGCompParameters(var GCompParams: TGCompParameters; TaskType: TTaskType);
begin
	GComp_SetDefaults(@GCompParams);

	with MainForm do begin
		GCompParams.SaveVersion := cbSaveVersion.Checked;

		GCompParams.strMetadataPath := '';
		if TaskType = Decompile then
			begin
				GCompParams.strCompoundFileName := DecompileFileName.Text;
				GCompParams.strDirectoryName := DecompileDirectory.Text;
				if cbUseDecompileFilter.Checked then
					GCompParams.strMetadataPath := DecompileFilter.Text;
			end
		else
			begin
				GCompParams.strCompoundFileName := CompileFileName.Text;
				GCompParams.strDirectoryName := CompileDirectory.Text;
				if cbUseCompileFilter.Checked then
					GCompParams.strMetadataPath := CompileFilter.Text;
			end;
		GCompParams.CompoundFileName := pChar(GCompParams.strCompoundFileName);
		GCompParams.DirectoryName := pChar(GCompParams.strDirectoryName);
		if GCompParams.strMetadataPath <> '' then
			GCompParams.MetadataPath := pChar(GCompParams.strMetadataPath);

		if rgCompoundType.ItemIndex = 1 then
			GCompParams.CompoundType := GComp_external_report
		else
			GCompParams.CompoundType := GComp_meta_data;

		GCompParams.Language := rgLanguage.ItemIndex;

		GCompParams.MetadataBySubfolders := cbMetadataBySubfolders.Checked;
		GCompParams.SortUserRightsObjects := cbSortUserRightsObjects.Checked;
		GCompParams.SortFormControls := cbSortFormControls.Checked;

		GCompParams.NoOrdering := cbNoOrdering.Checked;
		GCompParams.NoProfiles := cbNoProfiles.Checked;
		GCompParams.NoEmptyMxl := cbNoEmptyMxl.Checked;
		GCompParams.NoDefaults := cbNoDefaults.Checked;
		GCompParams.NotParseForms := cbNotParseForms.Checked;
		GCompParams.NoEmptyFolders := cbNoEmptyFolders.Checked;
		GCompParams.TruncateMMS := cbTruncateMMS.Checked;
		GCompParams.NoBrokenLinks := cbNoBrokenLinks.Checked;

		GCompParams.Verbose := rgVerbosity.ItemIndex;
		GCompParams.LogType := GComp_LogToListBox;
		GCompParams.hwndLog := lbLog.Handle;
		GCompParams.hwndInfoReceiver := Handle;
		GCompParams.InfoMessage := WM_GCOMP_INFO;
	end;
end;

procedure TMyRegistry.FromGCompParameters(var GCompParams: TGCompParameters; TaskType: TTaskType);
begin
	with MainForm do begin
		rgLanguage.ItemIndex := GCompParams.Language;

		cbMetadataBySubfolders.Checked := GCompParams.MetadataBySubfolders;
		cbSortUserRightsObjects.Checked := GCompParams.SortUserRightsObjects;
		cbSortFormControls.Checked := GCompParams.SortFormControls;

		cbNoOrdering.Checked := GCompParams.NoOrdering;
		cbNoProfiles.Checked := GCompParams.NoProfiles;
		cbNoEmptyMxl.Checked := GCompParams.NoEmptyMxl;
		cbNoDefaults.Checked := GCompParams.NoDefaults;
		cbNotParseForms.Checked := GCompParams.NotParseForms;
		cbNoEmptyFolders.Checked := GCompParams.NoEmptyFolders;
	end;
end;

//-------------------------------------------------------------------------------------------------
procedure TMyRegistry.CheckForIniFile(Dir: String);
var
	Params: TGCompParameters;
	Enable: Boolean;
begin
	Enable := true;
	if FileExists(Dir + '\gcomp.ini') then
	begin
		Params.DirectoryName := PChar(Dir);
		GComp_ReadSettings(@Params);
		FromGCompParameters(Params, Decompile);
		Enable := false;
	end;

	with MainForm do begin
		rgLanguage.Enabled := Enable;
		cbMetadataBySubfolders.Enabled := Enable;
		cbSortUserRightsObjects.Enabled := Enable;
		cbSortFormControls.Enabled := Enable;
		cbNoOrdering.Enabled := Enable;
		cbNoProfiles.Enabled := Enable;
		cbNoEmptyMxl.Enabled := Enable;
		cbNoDefaults.Enabled := Enable;
		cbNotParseForms.Enabled := Enable;
		cbNoEmptyFolders.Enabled := Enable;
	end;
end;

end.


