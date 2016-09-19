program Project1;

uses
  Forms,
  Unit1 in 'Unit1.pas' {FormManage},
  Unit3 in 'Unit3.pas' {FormArc},
  Unit5 in 'Unit5.pas' {FormChoiceDB},
  Unit4 in 'Unit4.pas' {FormAbout},
  GComp in 'GComp.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'Visual gcomp';
  Application.CreateForm(TFormManage, FormManage);
  Application.CreateForm(TFormAbout, FormAbout);
  Application.Run;
end.
