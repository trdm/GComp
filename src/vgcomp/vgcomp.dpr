program vgcomp;

uses
  Forms,
  vgc_MainForm in 'vgc_MainForm.pas' {MainForm},
  GComp in 'GComp.pas',
  SaveRestoreSettings in 'SaveRestoreSettings.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'vgcomp';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
