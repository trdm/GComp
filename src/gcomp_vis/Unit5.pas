unit Unit5;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, Registry;

type
  TFormChoiceDB = class(TForm)
    ListView1: TListView;
    btnCancel: TButton;
    btnOk: TButton;
    MDfromNew_stru: TCheckBox;
    btnDeleteRec: TButton;
    procedure btnOkClick(Sender: TObject);
    procedure btnCancelClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  FormChoiceDB: TFormChoiceDB;

implementation
uses unit1;
{$R *.dfm}

procedure TFormChoiceDB.btnOkClick(Sender: TObject);
begin
  if MDfromNew_stru.Checked then
    FormManage.Edit_path_place.Text := ListView1.Selected.SubItems[0] + 'New_stru\1Cv7.MD'
  else
    FormManage.Edit_path_place.Text := ListView1.Selected.SubItems[0] + '1Cv7.MD';
  Close;
end;

procedure TFormChoiceDB.btnCancelClick(Sender: TObject);
begin
  Close;
end;

procedure TFormChoiceDB.FormCreate(Sender: TObject);
var
  Reg: TRegistry ;
  SomeValues: TStringList ;
  iCount, i: Integer ;
   item: TListItem;
  _name, _path :String;
begin
  Reg := TRegistry.Create ;
  SomeValues := TStringList.Create ;
  try
    Reg.RootKey := HKEY_CURRENT_USER ;
    if Reg.OpenKey('Software\1C\1Cv7\7.7\Titles', False) then
    begin
      Reg.GetValueNames(SomeValues) ;
      iCount := SomeValues.Count ;
      if iCount >0 then
      for i := 0 to iCount -1 do begin
        _path :=  SomeValues.Strings[i];
        _name := Reg.ReadString(SomeValues.Strings[i]);
        item := ListView1.Items.Add;
        item.Caption := _name;
        item.SubItems.Add(_path);
        end;
    end ;
  finally
    Reg.Free ;
    SomeValues.Free ;
  end ;
end;

end.
