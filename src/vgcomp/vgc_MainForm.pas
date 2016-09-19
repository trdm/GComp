unit vgc_MainForm;

interface

uses
	Windows, Messages, SysUtils, Registry, Variants, Classes, Graphics,
	Controls, Forms, StdCtrls, ComCtrls, Buttons, Dialogs, FileCtrl,
	SaveRestoreSettings, GComp, ExtCtrls, ActnList, StdActns;

const
	WM_GCOMP_INFO = WM_User + 123;

type

TMainForm = class(TForm)
	PageControl1: TPageControl;
	DecompileParameters: TTabSheet;
	Label1: TLabel;
	DecompileFileName: TEdit;
	Label2: TLabel;
	DecompileDirectory: TEdit;
	OpenDialog: TOpenDialog;
	ProgressBar: TProgressBar;
	lbLog: TListBox;
	Label_Time: TLabel;
	CommonParameters: TTabSheet;
	cbMetadataBySubfolders: TCheckBox;
	cbSortUserRightsObjects: TCheckBox;
	CompileParameters: TTabSheet;
	cbSaveVersion: TCheckBox;
	cbNoOrdering: TCheckBox;
	cbCreateIniFile: TCheckBox;
	cbNoProfiles: TCheckBox;
	cbNoEmptyMxl: TCheckBox;
	cbTruncateMMS: TCheckBox;
	cbNoDefaults: TCheckBox;
	cbSortFormControls: TCheckBox;
	cbNoEmptyFolders: TCheckBox;
	btnChooseDecompileFile: TBitBtn;
	btnChooseDecompileDir: TBitBtn;
	rgVerbosity: TRadioGroup;
	rgLanguage: TRadioGroup;
	Label3: TLabel;
	Label4: TLabel;
	CompileFileName: TEdit;
	CompileDirectory: TEdit;
	btnChooseCompileFile: TBitBtn;
	btnChooseCompileDir: TBitBtn;
	rgCompoundType: TRadioGroup;
	DecompileFilter: TEdit;
	CompileFilter: TEdit;
	cbUseDecompileFilter: TCheckBox;
	cbUseCompileFilter: TCheckBox;
	cbSyncDecompileDir: TCheckBox;
	Bevel1: TBevel;
		BitBtn_Go: TBitBtn;
    BitBtn_Stop: TBitBtn;
    btnDecompile: TBitBtn;
    btnCompile: TBitBtn;
    cbNoBrokenLinks: TCheckBox;
		cbNotParseForms: TCheckBox;

	procedure FormCreate(Sender: TObject);
	procedure FormClose(Sender: TObject; var Action: TCloseAction);

	procedure btnDecompileClick(Sender: TObject);
	procedure btnCompileClick(Sender: TObject);

	procedure DecompileDirectoryExit(Sender: TObject);
	procedure DecompileFileNameExit(Sender: TObject);
	procedure btnChooseDecompileFileClick(Sender: TObject);
	procedure btnChooseDecompileDirClick(Sender: TObject);
	procedure cbUseDecompileFilterClick(Sender: TObject);

	procedure CompileFileNameExit(Sender: TObject);
	procedure btnChooseCompileFileClick(Sender: TObject);
	procedure btnChooseCompileDirClick(Sender: TObject);
	procedure cbUseCompileFilterClick(Sender: TObject);
		procedure DecompileFileNameEnter(Sender: TObject);
    procedure CompileFileNameEnter(Sender: TObject);

	private
		DecompileFileName_Before: string;
		CompileFileName_Before: string;

		DecompileParams: TGCompParameters;
		CompileParams: TGCompParameters;
		DecompileThread: TDecompileThread;
		CompileThread: TCompileThread;
		DecompileRunning: boolean;
		CompileRunning: boolean;
		StartTime: TDateTime;
		ProcessedTime: TDateTime;

		procedure SaveSettings();
		procedure LoadSettings();

		function DecompileParamsValid(): Boolean;
		procedure DecompileDone(Sender: TObject);
		function TryToStopDecompile(): boolean;

		function CompileParamsValid(): Boolean;
		procedure CompileDone(Sender: TObject);
		function TryToStopCompile(): boolean;

		procedure ShowProgress(Progress: Integer);

	public
		procedure WndProc(var Message: TMessage); override;
end;

var
	MainForm: TMainForm;
	Reg: TMyRegistry;

implementation
{$R *.dfm}

procedure TMainForm.ShowProgress(Progress: Integer);
begin
	ProgressBar.Position := Progress;
	ProcessedTime := Time() - StartTime;
	Label_Time.Caption := TimeToStr(ProcessedTime) + ' ' + IntToStr(Progress) + '/' + IntToStr(ProgressBar.Max);
end;

procedure TMainForm.WndProc(var Message: TMessage);
begin
	if (Message.Msg = (WM_USER+44037)) and (Message.wParam = 27) then
		//фиг его не знает, что это за мессага, но работает
		begin
			if not (DecompileRunning or CompileRunning) then
				begin
					Close();
				end
			else
				begin
					TryToStopDecompile();
					TryToStopCompile();
				end;
			inherited;
		end
	else if Message.Msg = WM_GCOMP_INFO then
	begin
		case Message.wParam of
			GComp_Inf_Total:
				begin
					ProgressBar.Min := 1;
					ProgressBar.Max := Message.lParam;
					ProgressBar.Step := 1;
					ProgressBar.Position := 0;
				end;
			GComp_Inf_Progress:
				begin
					ShowProgress(Message.lParam);
				end;
		end;
	end
	else
		inherited;
end;

//---------------------------------------------------------------
procedure TMainForm.FormCreate(Sender: TObject);
var
	i : Integer;
begin
	Caption := 'Visual gcomp. Версия gcomp.dll: ' + GComp_GetVersion();
	Label_Time.Caption := '';
	PageControl1.ActivePageIndex := 0;

	Reg := TMyRegistry.Create;
	LoadSettings;

	i := 1;
	while i < ParamCount() do
	begin
		if ParamStr(i) = '-D' then
			DecompileDirectory.Text := ParamStr(i+1)
		else if( ParamStr(i) = '-F' ) then
			DecompileFileName.Text := ParamStr(i+1);
		i := i + 1;
	end;
	DecompileDirectory.Text := ExcludeTrailingBackslash(DecompileDirectory.Text);
	if UpperCase(ExtractFileDir(DecompileFileName.Text)) = UpperCase(DecompileDirectory.Text) then
		DecompileDirectory.Text := DecompileDirectory.Text + '\Src';

	btnDecompile.Glyph := BitBtn_Go.Glyph;
	btnCompile.Glyph := BitBtn_Go.Glyph;

	Width := Bevel1.Left + Bevel1.Width + Bevel1.Left;
	PageControl1.Width := ClientWidth - PageControl1.Left;

	Label_Time.Top := ClientHeight - ProgressBar.Height;
	Label_Time.Width := 90;
	Label_Time.Left := ClientWidth - Label_Time.Width;
	ProgressBar.Top := Label_Time.Top;
	ProgressBar.Width := Label_Time.Left - ProgressBar.Left - 1;
	lbLog.Height := ProgressBar.Top - lbLog.Top - 1;
	lbLog.Width := ClientWidth - lbLog.Left;

	DecompileRunning := false;
	CompileRunning := false;
end;

procedure TMainForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
	if not TryToStopDecompile then
		Action := caNone
	else if not TryToStopCompile then
		Action := caNone;

	if Action <> caNone then
	begin
		SaveSettings();
		Reg.Free;
		DecompileThread.Free;
		CompileThread.Free;
	end;
end;


//---------------------------------------------------------------
procedure TMainForm.btnChooseDecompileFileClick(Sender: TObject);
begin
	OpenDialog.InitialDir := ExtractFileDir(DecompileFileName.Text);
	if OpenDialog.Execute then
	begin
		DecompileFileName.Text := OpenDialog.FileName;
		DecompileFileName.OnExit(Sender);
	end;
end;

procedure TMainForm.DecompileFileNameEnter(Sender: TObject);
begin
	DecompileFileName_Before := DecompileFileName.Text;
end;

procedure TMainForm.DecompileFileNameExit(Sender: TObject);
begin
	if (cbSyncDecompileDir.Checked)
		 and (ExtractFileDir(DecompileFileName_Before) <> ExtractFileDir(DecompileFileName.Text))
		 and (ExtractFileDir(DecompileFileName.Text) <> ExtractFileDir(DecompileDirectory.Text)) then
	begin
		DecompileDirectory.Text := ExtractFileDir(DecompileFileName.Text) + '\Src';
		Reg.CheckForIniFile(DecompileDirectory.Text);
	end;
	if LowerCase(ExtractFileExt(DecompileFileName.Text)) = '.ert' then
		rgCompoundType.ItemIndex := 1
	else
		rgCompoundType.ItemIndex := 0;
end;

//---------------------------------------------------------------
procedure TMainForm.btnChooseDecompileDirClick(Sender: TObject);
var
	NewDir: String;
begin
	NewDir := DecompileDirectory.Text;
	if SelectDirectory(NewDir, [sdAllowCreate, sdPerformCreate, sdPrompt], 0) then
	//if SelectDirectory('', '', NewDir) then
	begin
		DecompileDirectory.Text := NewDir;
		DecompileDirectory.OnExit(Sender);
	end;
end;

procedure TMainForm.DecompileDirectoryExit(Sender: TObject);
begin
	Reg.CheckForIniFile(DecompileDirectory.Text);
end;

//------------------------------------------------------------------
procedure TMainForm.btnChooseCompileFileClick(Sender: TObject);
begin
	OpenDialog.InitialDir := ExtractFileDir(CompileFileName.Text);
	if OpenDialog.Execute then
	begin
		CompileFileName.Text := OpenDialog.FileName;
		CompileFileName.OnExit(Sender);
	end;
end;

procedure TMainForm.CompileFileNameEnter(Sender: TObject);
begin
	CompileFileName_Before := CompileFileName.Text;
end;

procedure TMainForm.CompileFileNameExit(Sender: TObject);
begin
	if (cbSyncDecompileDir.Checked)
		 and (ExtractFileDir(CompileFileName_Before) <> ExtractFileDir(CompileFileName.Text))
		 and (ExtractFileDir(CompileFileName.Text) <> ExtractFileDir(CompileDirectory.Text)) then
	begin
		CompileDirectory.Text := ExtractFileDir(CompileFileName.Text) + '\Src';
	end;
	if LowerCase(ExtractFileExt(CompileFileName.Text)) = '.ert' then
		rgCompoundType.ItemIndex := 1
	else
		rgCompoundType.ItemIndex := 0;
end;

procedure TMainForm.btnChooseCompileDirClick(Sender: TObject);
var
	NewDir: String;
begin
	NewDir := CompileDirectory.Text;
	if SelectDirectory(NewDir, [sdAllowCreate, sdPerformCreate, sdPrompt], 0) then
	//if SelectDirectory('', '', NewDir) then
	begin
		CompileDirectory.Text := NewDir;
	end;
end;

//---------------------------------------------------------------
function TMainForm.DecompileParamsValid(): Boolean;
begin
	DecompileParamsValid := true;
	if Trim(DecompileFileName.Text) = '' then
	begin
		DecompileParamsValid := false;
		Application.MessageBox('Не указано имя файла', 'Неверно заданы параметры');
	end
	else if Trim(DecompileDirectory.Text) = '' then
	begin
		DecompileParamsValid := false;
		Application.MessageBox('Не указан каталог', 'Неверно заданы параметры');
	end;
end;

//---------------------------------------------------------------
procedure TMainForm.btnDecompileClick(Sender: TObject);
begin
	if not (DecompileRunning or CompileRunning) then
	begin
		if DecompileParamsValid then
		begin
			StartTime := Time;
			btnDecompile.Caption := 'Прервать декомпиляцию';
			btnDecompile.Glyph := BitBtn_Stop.Glyph;
			lbLog.Clear;
			lbLog.SetFocus;

			Reg.ToGCompParameters(DecompileParams, Decompile);

			if FileExists(DecompileParams.DirectoryName + '\gcomp.ini') then
			begin
				Reg.CheckForIniFile(DecompileParams.DirectoryName);
				GComp_ReadSettings(@DecompileParams);
			end
			else if cbCreateIniFile.Checked then
			begin
				GComp_SaveSettings(@DecompileParams);
				Reg.CheckForIniFile(DecompileParams.DirectoryName);
			end;

			DecompileRunning := true;
			DecompileThread.Free;
			DecompileThread := TDecompileThread.Create(@DecompileParams);
			DecompileThread.OnTerminate := DecompileDone;
			DecompileThread.Resume;
		end
	end
	else
	begin
		TryToStopDecompile();
		TryToStopCompile();
	end;
end;

procedure TMainForm.DecompileDone(Sender: TObject);
begin
	DecompileRunning := false;
	btnDecompile.Caption := 'Декомпилировать';
	btnDecompile.Glyph := BitBtn_Go.Glyph;

	ShowProgress(ProgressBar.Position);
	ProgressBar.Position := 0;

	if DecompileThread.Success then
		MainForm.lbLog.AddItem('Декомпиляция завершена успешно', nil)
	else
		MainForm.lbLog.AddItem('Ошибки при декомпиляции!', nil);
	MainForm.lbLog.ItemIndex := MainForm.lbLog.Count - 1;
end;

function TMainForm.TryToStopDecompile(): boolean;
begin
	TryToStopDecompile := true;

	if DecompileRunning then
	begin
		TryToStopDecompile := false;
		if IDYES = Application.MessageBox('Прервать декомпиляцию?', '', MB_YESNO) then
			if DecompileRunning then
			begin
				DecompileParams.CancelWork := TRUE;
				TryToStopDecompile := true;
			end;
	end;
end;

//--------------------------------------------------------------------------------
function TMainForm.CompileParamsValid(): Boolean;
begin
	CompileParamsValid := true;
	if Trim(CompileFileName.Text) = '' then
	begin
		CompileParamsValid := false;
		Application.MessageBox('Не указано имя файла', 'Неверно заданы параметры');
	end
	else if Trim(CompileDirectory.Text) = '' then
	begin
		CompileParamsValid := false;
		Application.MessageBox('Не указан каталог', 'Неверно заданы параметры');
	end;
end;

procedure TMainForm.btnCompileClick(Sender: TObject);
begin
	if not (DecompileRunning or CompileRunning) then
	begin
		if CompileParamsValid then
		begin
			StartTime := Time;
			btnCompile.Caption := 'Прервать компиляцию';
			btnCompile.Glyph := BitBtn_Stop.Glyph;
			lbLog.Clear;
			lbLog.SetFocus;

			Reg.ToGCompParameters(CompileParams, Compile);

			if FileExists(CompileParams.DirectoryName + '\gcomp.ini') then
			begin
				GComp_ReadSettings(@CompileParams);
			end;

			CompileRunning := true;
			CompileThread.Free;
			CompileThread := TCompileThread.Create(@CompileParams);
			CompileThread.OnTerminate := CompileDone;
			CompileThread.Resume;
		end
	end
	else
	begin
		TryToStopDecompile();
		TryToStopCompile();
	end;
end;

procedure TMainForm.CompileDone(Sender: TObject);
begin
	CompileRunning := false;
	btnCompile.Caption := 'Компилировать';
	btnCompile.Glyph := BitBtn_Go.Glyph;

	ShowProgress(ProgressBar.Position);
	ProgressBar.Position := 0;

	if CompileThread.Success then
		MainForm.lbLog.AddItem('Компиляция завершена успешно', nil)
	else
		MainForm.lbLog.AddItem('Ошибки при компиляции!', nil);
	MainForm.lbLog.ItemIndex := MainForm.lbLog.Count - 1;
end;

function TMainForm.TryToStopCompile(): boolean;
begin
	TryToStopCompile := true;

	if CompileRunning then
	begin
		TryToStopCompile := false;
		if IDYES = Application.MessageBox('Прервать компиляцию?', '', MB_YESNO) then
			if CompileRunning then
			begin
				CompileParams.CancelWork := TRUE;
				TryToStopCompile := true;
			end;
	end;
end;

//---------------------------------------------------------------
procedure TMainForm.SaveSettings();
begin
	Reg.WriteAll();
end;

procedure TMainForm.LoadSettings();
begin
	Reg.ReadAll();
	Reg.CheckForIniFile(DecompileDirectory.Text);
end;

procedure TMainForm.cbUseDecompileFilterClick(Sender: TObject);
begin
	DecompileFilter.Visible := cbUseDecompileFilter.Checked;
end;

procedure TMainForm.cbUseCompileFilterClick(Sender: TObject);
begin
	CompileFilter.Visible := cbUseCompileFilter.Checked;
end;

end.
