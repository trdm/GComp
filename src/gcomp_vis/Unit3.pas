unit Unit3;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, ShellAPI, FileCtrl, Buttons;

type
  TFormArc = class(TForm)
    EditArcName: TLabeledEdit;
    EditArcPath: TLabeledEdit;
    btnGoArc: TButton;
    btnCancel: TButton;
    btnchoicePathArc: TBitBtn;
    procedure btnGoArcClick(Sender: TObject);
    procedure btnCancelClick(Sender: TObject);
    procedure btnChoiceNameClick(Sender: TObject);
    procedure btnchoicePathArcClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    ArcObj : String;
  end;

var
  FormArc: TFormArc;

implementation
uses unit1, unit2;
{$R *.dfm}

procedure TFormArc.btnGoArcClick(Sender: TObject);
var
  vArc : String;
begin
  try
    ShellExecute(
      0,
      'open',
      PChar(FormManage.Edit_WinRar_path.Text),
      PChar('a -ep1 "' + EditArcPath.Text + '\' + EditArcName.Text + '" "' + ArcObj + '"'),
      nil ,
      SW_SHOWNORMAL
    );
  except
    ShowMessage('Ошибка при создании архива');
  end;
  Close;
end;

procedure TFormArc.btnCancelClick(Sender: TObject);
begin
  Close;
end;

procedure TFormArc.btnChoiceNameClick(Sender: TObject);
begin
  //ShowMessage('Пока не сделал');
  //GetFileName(FormManage.Edit_path_place.Text);
  GetFileAttributes(pchar(FormManage.Edit_path_place.Text));
end;

procedure TFormArc.btnchoicePathArcClick(Sender: TObject);
var
  MyNewDir : String;
begin
  MyNewDir := EditArcPath.Text;
  SelectDirectory('Выбор каталога','',MyNewDir);
  EditArcPath.Text := MyNewDir;
end;

procedure TFormArc.FormShow(Sender: TObject);
begin
  EditArcPath.Text := ExtractFileDir(EditArcPath.Text);
end;

end.
