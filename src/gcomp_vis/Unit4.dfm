object FormAbout: TFormAbout
  Left = 193
  Top = 114
  BorderStyle = bsDialog
  Caption = 'About'
  ClientHeight = 180
  ClientWidth = 246
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 46
    Top = 8
    Width = 153
    Height = 20
    Caption = 'Visual gcomp 1.1.1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 40
    Width = 196
    Height = 13
    Caption = #1040#1074#1090#1086#1088': '#1059#1096#1072#1082#1086#1074' '#1057#1077#1088#1075#1077#1081' '#1040#1083#1077#1082#1085#1089#1072#1076#1088#1086#1074#1080#1095
  end
  object Label3: TLabel
    Left = 8
    Top = 80
    Width = 177
    Height = 13
    Caption = #1057#1088#1077#1076#1072' '#1088#1072#1079#1088#1072#1073#1086#1090#1082#1080': Borland Delphi 7'
  end
  object Label4: TLabel
    Left = 40
    Top = 56
    Width = 188
    Height = 13
    Caption = ' aka Phoenix (PhoenixUSA@yandex.ru)'
  end
  object Label5: TLabel
    Left = 8
    Top = 104
    Width = 225
    Height = 26
    Caption = #1042#1099#1088#1072#1078#1072#1102' '#1073#1083#1072#1075#1086#1076#1072#1088#1085#1086#1089#1090#1100' '#1044#1080#1088#1082#1089' '#1040#1083#1077#1082#1089#1077#1102' '#1079#1072' '#1087#1088#1080#1084#1077#1088' '#1088#1072#1073#1086#1090#1099' '#1089' gcomp.dll'
    WordWrap = True
  end
  object Button1: TButton
    Left = 85
    Top = 144
    Width = 75
    Height = 25
    Caption = #1047#1072#1082#1088#1099#1090#1100
    TabOrder = 0
    OnClick = Button1Click
  end
end
