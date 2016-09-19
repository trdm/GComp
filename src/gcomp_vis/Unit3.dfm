object FormArc: TFormArc
  Left = 333
  Top = 220
  BorderStyle = bsDialog
  Caption = #1040#1088#1093#1080#1074#1080#1088#1086#1074#1072#1085#1080#1077
  ClientHeight = 130
  ClientWidth = 321
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
  object EditArcName: TLabeledEdit
    Left = 8
    Top = 24
    Width = 305
    Height = 21
    EditLabel.Width = 60
    EditLabel.Height = 13
    EditLabel.Caption = #1048#1084#1103' '#1072#1088#1093#1080#1074#1072
    TabOrder = 0
  end
  object EditArcPath: TLabeledEdit
    Left = 8
    Top = 64
    Width = 273
    Height = 21
    EditLabel.Width = 62
    EditLabel.Height = 13
    EditLabel.Caption = #1055#1091#1090#1100' '#1072#1088#1093#1080#1074#1072
    TabOrder = 1
  end
  object btnGoArc: TButton
    Left = 152
    Top = 96
    Width = 75
    Height = 25
    Caption = #1042#1099#1087#1086#1083#1085#1080#1090#1100
    TabOrder = 2
    OnClick = btnGoArcClick
  end
  object btnCancel: TButton
    Left = 232
    Top = 96
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    TabOrder = 3
    OnClick = btnCancelClick
  end
  object btnchoicePathArc: TBitBtn
    Left = 288
    Top = 63
    Width = 25
    Height = 25
    TabOrder = 4
    OnClick = btnchoicePathArcClick
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      0400000000000001000000000000000000001000000000000000000000000000
      8000008000000080800080000000800080008080000080808000C0C0C0000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00666666666666
      6666666666666666666666666666666666666666666666666666000000000006
      66667777777777766666003333333330666677666666666766660B0333333333
      066676766666666676660FB033333333306676676666666667660BFB03333333
      330676667666666666760FBFB0000000000076666777777777770BFBFBFBFB06
      666676666666667666660FBFBFBFBF06666676666666667666660BFB00000006
      6666766677777776666660006666666600066777666666667776666666666666
      6006666666666666677666666666066606066666666676667676666666666000
      6666666666666777666666666666666666666666666666666666}
    NumGlyphs = 2
  end
end
