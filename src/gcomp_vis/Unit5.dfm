object FormChoiceDB: TFormChoiceDB
  Left = 193
  Top = 114
  BorderStyle = bsDialog
  Caption = #1042#1099#1073#1086#1088' '#1073#1072#1079#1099
  ClientHeight = 282
  ClientWidth = 568
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object ListView1: TListView
    Left = 8
    Top = 8
    Width = 553
    Height = 233
    Columns = <
      item
        Caption = #1053#1072#1080#1084#1077#1085#1086#1074#1072#1085#1080#1077
        Width = 200
      end
      item
        Caption = #1055#1091#1090#1100
        Width = 330
      end>
    ReadOnly = True
    RowSelect = True
    SortType = stText
    TabOrder = 0
    ViewStyle = vsReport
    OnDblClick = btnOkClick
  end
  object btnCancel: TButton
    Left = 480
    Top = 248
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 1
    OnClick = btnCancelClick
  end
  object btnOk: TButton
    Left = 400
    Top = 248
    Width = 75
    Height = 25
    Caption = #1042#1099#1073#1088#1072#1090#1100
    TabOrder = 2
    OnClick = btnOkClick
  end
  object MDfromNew_stru: TCheckBox
    Left = 8
    Top = 256
    Width = 103
    Height = 17
    Caption = 'MD '#1080#1079' New_stru'
    TabOrder = 3
  end
  object btnDeleteRec: TButton
    Left = 320
    Top = 248
    Width = 75
    Height = 25
    Caption = #1059#1076#1072#1083#1080#1090#1100
    TabOrder = 4
  end
end
