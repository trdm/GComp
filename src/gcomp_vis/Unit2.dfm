object FormChoiceDir: TFormChoiceDir
  Left = 367
  Top = 180
  BorderStyle = bsDialog
  Caption = #1042#1099#1073#1086#1088' '#1082#1072#1090#1072#1083#1086#1075#1072
  ClientHeight = 289
  ClientWidth = 288
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ShellTreeView1: TShellTreeView
    Left = 8
    Top = 8
    Width = 273
    Height = 241
    ObjectTypes = [otFolders]
    Root = 'rfMyComputer'
    UseShellImages = True
    AutoRefresh = False
    Indent = 19
    ParentColor = False
    RightClickSelect = True
    ShowRoot = False
    TabOrder = 0
    OnDblClick = btnChoiceDirClick
  end
  object btnChoiceDir: TButton
    Left = 120
    Top = 256
    Width = 75
    Height = 25
    Caption = #1042#1099#1073#1088#1072#1090#1100
    TabOrder = 1
    OnClick = btnChoiceDirClick
  end
  object btnNewDir: TButton
    Left = 8
    Top = 256
    Width = 75
    Height = 25
    Caption = #1057#1086#1079#1076#1072#1090#1100
    TabOrder = 2
    OnClick = btnNewDirClick
  end
  object btnCancel: TButton
    Left = 200
    Top = 256
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 3
    OnClick = btnCancelClick
  end
end
