unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, ExtCtrls, ToolWin, ShellAPI, FileCtrl,
  Registry, Buttons, ImgList, Menus, ShellCtrls;

type
  TFormManage = class(TForm)
    PageControlMain: TPageControl;
    TabSheet3: TTabSheet;
    TabSheet4: TTabSheet;
    Edit_path_place: TLabeledEdit;
    Edit_parse_place: TLabeledEdit;
    PageControlGcompManage: TPageControl;
    TabSheetDecompile: TTabSheet;
    NoProfiles: TCheckBox;
    TabSheetCompile: TTabSheet;
    TruncateMms: TCheckBox;
    ToolBar1: TToolBar;
    ToolButton1: TToolButton;
    OpenDialog1: TOpenDialog;
    Edit_WinRar_path: TLabeledEdit;
    Edit_gcomp_place: TLabeledEdit;
    ImageList1: TImageList;
    btnSaveFile: TBitBtn;
    btnSaveDecompile: TBitBtn;
    SaveOptions: TCheckBox;
    btnChoiceFile: TBitBtn;
    btnDBFromReestr: TBitBtn;
    btn_choice_parse_place: TBitBtn;
    btn_choice_gcomp_place: TBitBtn;
    btn_Choice_WinRar: TBitBtn;
    Edit1: TEdit;
    btnSaveBat: TBitBtn;
    Button1: TButton;
    SaveDialog1: TSaveDialog;
    btnSave1CTitle: TBitBtn;
    Run1CConfig: TToolButton;
    ToolButton6: TToolButton;
    NoOrder: TCheckBox;
    DialogFormOptions: TRadioGroup;
    btn_Go: TButton;
    GroupBox1: TGroupBox;
    ToolBar2: TToolBar;
    btn_ClearLog: TToolButton;
    btn_SaveLog: TToolButton;
    Edit_default_Parse_dir: TLabeledEdit;
    btnChoiceDirParse: TBitBtn;
    cbParseInDirIB: TCheckBox;
    Path1C77exe: TComboBox;
    Label1: TLabel;
    TabSheet1: TTabSheet;
    Memo2: TMemo;
    MsgVariant: TRadioGroup;
    NoEmptyMxl: TCheckBox;
    FolderNames: TRadioGroup;
    NoVersion: TCheckBox;
    ListBox1: TListBox;
    cbErt: TCheckBox;
    procedure btn_choice_gcomp_placeClick(Sender: TObject);
    procedure btn_GoClick(Sender: TObject);
    procedure btn_choice_parse_placeClick(Sender: TObject);
    procedure btn_Choice_WinRarClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure btnSaveFileClick(Sender: TObject);
    procedure btnSaveDecompileClick(Sender: TObject);
    procedure btnDBFromReestrClick(Sender: TObject);
    procedure btnChoiceFileClick(Sender: TObject);
    procedure ToolButton1Click(Sender: TObject);
    function _GetStringDo():String ;
    procedure Edit_path_placeChange(Sender: TObject);
    procedure btnSaveBatClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure btnSave1CTitleClick(Sender: TObject);
    procedure SaveConfigClick(Sender: TObject);
    procedure LoadConfigClick(Sender: TObject);
    procedure Run1CConfigClick(Sender: TObject);
    procedure btn_ClearLogClick(Sender: TObject);
    procedure btn_SaveLogClick(Sender: TObject);
    procedure btnChoiceDirParseClick(Sender: TObject);
    procedure TabSheet4Show(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  FormManage: TFormManage;

implementation

uses Unit2, Unit3, Unit5, Unit4, Gcomp;

{$R *.dfm}

//------------------------------------------------------------------------------
procedure TFormManage.btn_choice_gcomp_placeClick(Sender: TObject);
begin
  OpenDialog1.InitialDir := ExtractFileDir(Edit_gcomp_place.Text);
  if OpenDialog1.Execute then begin
    Edit_gcomp_place.Text := OpenDialog1.FileName;
  end;
  _GetStringDo()
end;
//------------------------------------------------------------------------------
procedure TFormManage.btn_GoClick(Sender: TObject);
var
  vPathExec : String;
  	Params : TGCompParameters;
    vTypeFile : Integer;
begin
  _GetStringDo();
  ForceDirectories(Edit_parse_place.Text);

	GComp_SetDefaults(@Params);

  if cbErt.Checked then
    vTypeFile := 1
  else
    vTypeFile := 2;

  Params.CompoundType         := vTypeFile;

	Params.CompoundFileName     := pChar(Edit_path_place.Text);
	Params.DirectoryName        := pChar(Edit_parse_place.Text);
  Params.Verbose              := MsgVariant.ItemIndex;
  Params.NoEmptyMxl           := NoEmptyMxl.Checked;
  Params.Language             := FolderNames.ItemIndex;

  // разбор МД
  if PageControlGcompManage.ActivePage = TabSheetDecompile then begin  // decompile
    Params.NoProfiles           := NoProfiles.Checked;
    Params.NoOrdering           := NoOrder.Checked;
    Params.SaveVersion          := NoVersion.Checked; //save gcomp's version into file gcomp.ver

    Case DialogFormOptions.ItemIndex of
      0: Params.NoDefaultsInDialogs  := True;
      1:;
      2: Params.NotParseForms        := True;
    end;
  end;
  // сбор
  if PageControlGcompManage.ActivePage = TabSheetCompile then begin // compile
  	Params.TruncateMMS := TruncateMms.Checked;
  end;

	Params.LogType := GComp_LogToListBox;
	Params.hwndLog := ListBox1.Handle;

	ListBox1.Clear;

	Update();
  if PageControlGcompManage.ActivePage = TabSheetDecompile then begin  // decompile
  	if( GComp_Decompile(@Params) = TRUE ) then begin
		  ShowMessage('Обработка МД завершена');
	  end else begin
		  ShowMessage('Возникли проблемы при обработке МД');
	  end
   end;
   if PageControlGcompManage.ActivePage = TabSheetCompile then begin // compile
    if( GComp_Compile(@Params) = TRUE ) then begin
		  ShowMessage('Обработка МД завершена');
	  end else begin
		  ShowMessage('Возникли проблемы при обработке МД');
	  end
   end;

end;
//------------------------------------------------------------------------------
procedure TFormManage.btn_choice_parse_placeClick(Sender: TObject);
var
  MyNewDir : String;
begin
  MyNewDir := Edit_parse_place.Text;
  if SelectDirectory('Выбор каталога','',MyNewDir) then
    Edit_parse_place.Text := MyNewDir;
  _GetStringDo();
end;
//------------------------------------------------------------------------------
procedure TFormManage.btn_Choice_WinRarClick(Sender: TObject);
begin
  OpenDialog1.InitialDir := ExtractFileDir(Path1C77exe.Text);
  if OpenDialog1.Execute then begin
    Path1C77exe.Text := OpenDialog1.FileName;
  end;
  _GetStringDo()
end;
//------------------------------------------------------------------------------
procedure TFormManage.FormClose(Sender: TObject; var Action: TCloseAction);
var
  Reg : TRegistry;
begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_CURRENT_USER;
  Reg.OpenKey('SOFTWARE\PhoenixSoft\VisualGcomp\',true);
  Reg.WriteString('Edit_gcomp_place',Edit_gcomp_place.Text);
  Reg.WriteString('Edit_WinRar_path',Edit_WinRar_path.Text);
  Reg.WriteBool('SaveOptions', SaveOptions.Checked);

  if SaveOptions.Checked then begin
    Reg.WriteString('Edit_path_place',Edit_path_place.Text);
    Reg.WriteString('Edit_parse_place',Edit_parse_place.Text);
    Reg.WriteInteger('MsgVariant',MsgVariant.ItemIndex);
    Reg.WriteBool('NoEmptyMxl', NoEmptyMxl.Checked);
    Reg.WriteBool('NoProfiles', NoProfiles.Checked);
    Reg.WriteInteger('DialogFormOptions', DialogFormOptions.ItemIndex);
    Reg.WriteInteger('FolderNames',FolderNames.ItemIndex);
    Reg.WriteBool('TruncateMms', TruncateMms.Checked);
    Reg.WriteString('Path1C77exe',Path1C77exe.Text);
    Reg.WriteBool('cbParseInDirIB', cbParseInDirIB.Checked);
    Reg.WriteString('Edit_default_Parse_dir', Edit_default_Parse_dir.Text);
    Reg.WriteBool('NoOrder', NoOrder.Checked);
    Reg.WriteBool('NoVersion', NoVersion.Checked);
  end;
  Reg.CloseKey;
  Reg.Free;
end;
//------------------------------------------------------------------------------
procedure TFormManage.FormCreate(Sender: TObject);
var
  Reg : TRegistry;
begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_CURRENT_USER;
  Reg.OpenKey('SOFTWARE\PhoenixSoft\VisualGcomp\',true);
  try
    Edit_gcomp_place.Text := Reg.ReadString('Edit_gcomp_place');
    Edit_WinRar_path.Text := Reg.ReadString('Edit_WinRar_path');
    SaveOptions.Checked := Reg.ReadBool('SaveOptions');
  except
  end;

  if SaveOptions.Checked then begin
    Try
      Edit_path_place.Text := Reg.ReadString('Edit_path_place');
      Edit_parse_place.Text := Reg.ReadString('Edit_parse_place');
      MsgVariant.ItemIndex := Reg.ReadInteger('MsgVariant');
      NoEmptyMxl.Checked := Reg.ReadBool('NoEmptyMxl');
      NoProfiles.Checked := Reg.ReadBool('NoProfiles');
      DialogFormOptions.ItemIndex := Reg.ReadInteger('DialogFormOptions');
      FolderNames.ItemIndex := Reg.ReadInteger('FolderNames');
      TruncateMms.Checked := Reg.ReadBool('TruncateMms');
      Path1C77exe.Text := Reg.ReadString('Path1C77exe');
      cbParseInDirIB.Checked := Reg.ReadBool('cbParseInDirIB');
      Edit_default_Parse_dir.Text := Reg.ReadString('Edit_default_Parse_dir');
      NoOrder.Checked := Reg.ReadBool('NoOrder');
      NoVersion.Checked := Reg.ReadBool('NoVersion');
    except
    end
  end else begin
    MsgVariant.ItemIndex := 2;
    FolderNames.ItemIndex := 0;
    DialogFormOptions.ItemIndex := 0;
  end;

  Reg.CloseKey;
  Reg.Free;

end;
//------------------------------------------------------------------------------
procedure TFormManage.btnSaveFileClick(Sender: TObject);
begin
  if FileExists(Edit_WinRar_path.Text) = False then begin
    ShowMessage('Не найден исполняемый файл WinRar (Закладка: Параметры)');
    exit;
  end;

  with TFormArc.Create(nil) do begin
    EditArcPath.Text := Edit_path_place.Text;
    ArcObj := Edit_path_place.Text;
    ShowModal;
    Free;
  end;
end;
//------------------------------------------------------------------------------
procedure TFormManage.btnSaveDecompileClick(Sender: TObject);
begin
  if FileExists(Edit_WinRar_path.Text) = False then begin
    ShowMessage('Не найден исполняемый файл WinRar (Закладка: Параметры)');
    exit;
  end;
  with TFormArc.Create(nil) do begin
    EditArcPath.Text := Edit_parse_place.Text;
    ArcObj := Edit_parse_place.Text;
    ShowModal;
    Free;
  end;
end;
//------------------------------------------------------------------------------
procedure TFormManage.btnDBFromReestrClick(Sender: TObject);
begin
  with TFormChoiceDB.Create(nil) do begin
    ShowModal;
    Free;
  end;
  if cbParseInDirIB.Checked then
    Edit_parse_place.Text := ExtractFileDir(Edit_path_place.Text) + '\Parse'
  else
    Edit_parse_place.Text := Edit_WinRar_path.Text;
  _GetStringDo()
end;
//------------------------------------------------------------------------------
procedure TFormManage.btnChoiceFileClick(Sender: TObject);
begin
  OpenDialog1.InitialDir := ExtractFileDir(Edit_path_place.Text);
  if OpenDialog1.Execute then begin
    Edit_path_place.Text := OpenDialog1.FileName;
  end;
  if cbParseInDirIB.Checked then
    Edit_parse_place.Text := ExtractFileDir(Edit_path_place.Text) + '\Parse'
  else
    Edit_parse_place.Text := Edit_WinRar_path.Text;
  _GetStringDo()
end;
//------------------------------------------------------------------------------
procedure TFormManage.ToolButton1Click(Sender: TObject);
begin
  with TformAbout.Create(nil) do begin
    ShowModal;
    Free;
  end;
end;
//------------------------------------------------------------------------------
function TFormManage._GetStringDo():String ;
var
  vPathExec : String;
begin

  if FileExists(Edit_gcomp_place.Text) = False then begin
    ShowMessage('Не найден исполняемый файл Gcomp (Закладка: Параметры)');
    exit;
  end;

  if PageControlGcompManage.ActivePage = TabSheetDecompile then begin  // decompile
    vPathExec := '"' + Trim(Edit_gcomp_place.Text) + '" -d ';
    case MsgVariant.ItemIndex of
      0: vPathExec := vPathExec + '-q ';
      1: vPathExec := vPathExec + '-v ';
      2: vPathExec := vPathExec + '-vv ';
    end;

    if NoEmptyMxl.Checked then vPathExec := vPathExec + '--no-empty-mxl ';
    if NoProfiles.Checked then vPathExec := vPathExec + '--no-profiles ';
    if NoOrder.Checked then vPathExec := vPathExec + '--no-order ';
    if NoVersion.Checked then vPathExec := vPathExec + '--no-version ';

    case DialogFormOptions.ItemIndex of
      0: vPathExec := vPathExec + '--no-dialog-defaults ';
      2: vPathExec := vPathExec + '--no-parse-dialog ';
    end;

    case FolderNames.ItemIndex of
      1: vPathExec := vPathExec + '--translit ';
      2: vPathExec := vPathExec + '--eng ';
    end;

    vPathExec := vPathExec + '-F "' + Trim(Edit_path_place.Text) + '"';
    vPathExec := vPathExec + ' -D "' + Trim(Edit_parse_place.Text) + '"';
  end;
  if PageControlGcompManage.ActivePage = TabSheetCompile then begin // compile

    vPathExec := '"' + Trim(Edit_gcomp_place.Text) + '" -c ';
    case MsgVariant.ItemIndex of
      0: vPathExec := vPathExec + '-q ';
      1: vPathExec := vPathExec + '-v ';
      2: vPathExec := vPathExec + '-vv ';
    end;
    if NoEmptyMxl.Checked then vPathExec := vPathExec + '--no-empty-mxl ';
    case FolderNames.ItemIndex of
      1: vPathExec := vPathExec + '--translit ';
      2: vPathExec := vPathExec + '--eng ';
    end;
    if TruncateMms.Checked then vPathExec := vPathExec + '--truncate-mms ';

    vPathExec := vPathExec + '-F "' + Trim(Edit_path_place.Text) + '"';
    vPathExec := vPathExec + ' -DD "' + Trim(Edit_parse_place.Text) + '"';
  end;

  Edit1.Text := vPathExec;
end;
//------------------------------------------------------------------------------
procedure TFormManage.Edit_path_placeChange(Sender: TObject);
begin
_GetStringDo()
end;
//------------------------------------------------------------------------------
procedure TFormManage.btnSaveBatClick(Sender: TObject);
var
  F: TextFile;
begin
  SaveDialog1.InitialDir := ExtractFileDir(Edit_path_place.Text);
  SaveDialog1.DefaultExt := '*.bat';
  SaveDialog1.FileName := 'gcomp_';
  if SaveDialog1.Execute then begin
    AssignFile(F, SaveDialog1.FileName);
    Rewrite(F);
    Writeln(F, edit1.Text);
    CloseFile(F);
  end;
end;
//------------------------------------------------------------------------------
procedure TFormManage.Button1Click(Sender: TObject);
begin
  Close;
end;
//------------------------------------------------------------------------------
procedure TFormManage.btnSave1CTitleClick(Sender: TObject);
var
  Reg: TRegistry;
  SomeValues: TStringList;
  iCount, i, vHave, Change : Integer;
  vName, vPath : String;
  NameDB : String;
begin
  NameDB := InputBox('Имя базы','Имя базы для записи в реестр','');
  if NameDB <> '' then begin
    Try
      Reg := TRegistry.Create;
      SomeValues := TStringList.Create;
      vHave := 0;
      Reg.RootKey := HKEY_CURRENT_USER;
      if Reg.OpenKey('Software\1C\1Cv7\7.7\Titles', False) then begin
        Reg.GetValueNames(SomeValues);
        iCount := SomeValues.Count;
        if iCount > 0 then begin
          for i := 0 to iCount - 1 do begin
            vName := Reg.ReadString(SomeValues.Strings[i]);
            vPath := SomeValues.Strings[i];
            if vName = NameDB then begin
              ShowMessage('База с таким наименованием уже зарегистрирована');
              vHave := 1;
              Break;
            end; // if
            if vPath = ExtractFileDir(Edit_path_place.Text) + '\' then begin
              ShowMessage('База с таким адресом уже зарегистрирована');
              vHave := 1;
              Break;
            end; // if
          end; // for
          if vHave = 0 then begin
            Reg.WriteString(ExtractFileDir(Edit_path_place.Text) + '\',NameDB);
          end; // if
        end; // if
      end; // if
    finally
      Reg.Free;
      SomeValues.Free;
    end;
  end; // if 1
end;
//------------------------------------------------------------------------------
procedure TFormManage.SaveConfigClick(Sender: TObject);
var
  Reg : TRegistry;
begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_CURRENT_USER;
  Reg.OpenKey('SOFTWARE\PhoenixSoft\VisualGcomp\Data\' + InputBox('Visual gcomp','Название настройки',''),true);
  Reg.WriteString('Edit_path_place', Edit_path_place.Text);
  Reg.WriteString('Edit_parse_place', Edit_parse_place.Text);
  Reg.Free;
end;
//------------------------------------------------------------------------------
procedure TFormManage.LoadConfigClick(Sender: TObject);
begin
  with TFormChoiceDB.Create(nil) do begin
    ShowModal;
    Free;
  end;
  _GetStringDo();
end;
//------------------------------------------------------------------------------
procedure TFormManage.Run1CConfigClick(Sender: TObject);
begin
  if FileExists(Path1C77exe.Text) = False then begin
    ShowMessage('Не найден исполняемый файл 1С:Предприятие 7.7 (Закладка: Параметры)');
    exit;
  end;
  ShellExecute(0,'open',PChar(Path1C77exe.text),PChar(' config ' + '/D' + ExtractFileDir(Edit_path_place.Text)),'c:\',SW_SHOWNORMAL);
end;
//------------------------------------------------------------------------------
procedure TFormManage.btn_ClearLogClick(Sender: TObject);
begin
  ListBox1.Clear;
end;
//------------------------------------------------------------------------------
procedure TFormManage.btn_SaveLogClick(Sender: TObject);
var
  F: TextFile;
  i : Integer;
begin
  SaveDialog1.InitialDir := ExtractFileDir(Edit_path_place.Text);
  SaveDialog1.DefaultExt := '*.txt';
  SaveDialog1.FileName := 'Log_';
  if SaveDialog1.Execute then begin
    AssignFile(F, SaveDialog1.FileName);
    Rewrite(F);
    for i := 0 to (ListBox1.Items.Count - 1) do begin
      Writeln(F, ListBox1.Items.Strings[i]);
    end; // for
    CloseFile(F);
    ShowMessage('Лог сохранен: ' + SaveDialog1.FileName);
  end;
end;
//------------------------------------------------------------------------------
procedure TFormManage.btnChoiceDirParseClick(Sender: TObject);
var
  MyNewDir : string;
begin
  MyNewDir := Edit_default_Parse_dir.Text;
  SelectDirectory('Выбор каталога','',MyNewDir);
  Edit_default_Parse_dir.Text := MyNewDir;
  _GetStringDo()
end;
//------------------------------------------------------------------------------
procedure TFormManage.TabSheet4Show(Sender: TObject);
var
  Reg : TRegistry;
  MyStrList :TStringList;
  I : Integer;
  vPath : String;
begin
  Path1C77exe.Items.Clear;
  Reg := TRegistry.Create();
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  vPath :=   'SOFTWARE\1C\1Cv7\7.7\';
  if Reg.OpenKey(vPath,False) = False then
    ShowMessage('Не установлено 1С Предприятие 7.7')
  else begin
    MyStrList := TStringList.Create();
    Reg.GetKeyNames(MyStrList);
      for I := 0 to MyStrList.Count - 1 do begin
        Reg.CloseKey;
        Reg.OpenKey(vPath + MyStrList.Strings[I], False);
        Path1C77exe.Items.Add(Reg.ReadString('1CPath'))
      end; // for
  end; // if
end;

end.
