unit Unit2;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, ShellCtrls, ExtCtrls;

type
  TFormChoiceDir = class(TForm)
    ShellTreeView1: TShellTreeView;
    btnChoiceDir: TButton;
    btnNewDir: TButton;
    btnCancel: TButton;
    procedure btnChoiceDirClick(Sender: TObject);
    procedure btnNewDirClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure btnCancelClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    EditToResult : TLabeledEdit;
  end;

var
  FormChoiceDir: TFormChoiceDir;

implementation
uses unit1;
{$R *.dfm}

procedure TFormChoiceDir.btnChoiceDirClick(Sender: TObject);
begin
  EditToResult.Text := ShellTreeView1.Path;
  Close;
end;

procedure TFormChoiceDir.btnNewDirClick(Sender: TObject);
begin
  CreateDir(ShellTreeView1.Path + InputBox('Новая папка','',''));
  ShellTreeView1.Refresh(ShellTreeView1.Selected);
end;

procedure TFormChoiceDir.FormShow(Sender: TObject);
begin
  try
    ShellTreeView1.Path := FormManage.Edit_path_place.Text;
  except
    //
  end;
end;

procedure TFormChoiceDir.btnCancelClick(Sender: TObject);
begin
  Close;
end;

end.
