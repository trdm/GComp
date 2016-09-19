object MainForm: TMainForm
  Left = 248
  Top = 156
  Width = 650
  Height = 485
  Caption = 'Visual gcomp'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  ShowHint = True
  OnClose = FormClose
  OnCreate = FormCreate
  DesignSize = (
    642
    458)
  PixelsPerInch = 96
  TextHeight = 13
  object Label_Time: TLabel
    Left = 557
    Top = 444
    Width = 89
    Height = 13
    Anchors = [akRight, akBottom]
    AutoSize = False
    Caption = 'Label_Time'
  end
  object lbLog: TListBox
    Left = 0
    Top = 264
    Width = 643
    Height = 179
    Anchors = [akLeft, akTop, akRight, akBottom]
    ItemHeight = 13
    TabOrder = 2
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 8
    Width = 643
    Height = 257
    ActivePage = DecompileParameters
    Anchors = [akLeft, akTop, akRight]
    TabIndex = 0
    TabOrder = 0
    object DecompileParameters: TTabSheet
      Caption = #1044#1077#1082#1086#1084#1087#1080#1083#1103#1094#1080#1103
      DesignSize = (
        635
        229)
      object Label1: TLabel
        Left = 14
        Top = 3
        Width = 57
        Height = 13
        Caption = #1048#1084#1103' '#1092#1072#1081#1083#1072
      end
      object Label2: TLabel
        Left = 30
        Top = 25
        Width = 41
        Height = 13
        Caption = #1050#1072#1090#1072#1083#1086#1075
      end
      object TLabel
        Left = 424
        Top = 96
        Width = 3
        Height = 13
      end
      object DecompileFileName: TEdit
        Left = 78
        Top = -1
        Width = 529
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        Text = 'c:\tmp\1cv7.md'
        OnEnter = DecompileFileNameEnter
        OnExit = DecompileFileNameExit
      end
      object DecompileDirectory: TEdit
        Left = 78
        Top = 21
        Width = 529
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 2
        Text = 'c:\tmp\Src'
        OnExit = DecompileDirectoryExit
      end
      object cbMetadataBySubfolders: TCheckBox
        Left = 78
        Top = 43
        Width = 273
        Height = 17
        Hint = 
          #1054#1073#1098#1077#1082#1090#1099' '#1084#1077#1090#1072#1076#1072#1085#1085#1099#1093', '#1085#1077' '#1080#1084#1077#1102#1097#1080#1077' '#1092#1086#1088#1084' ('#1082#1086#1085#1089#1090#1072#1085#1090#1099', '#1087#1077#1088#1077#1095#1080#1089#1083#1077#1085#1080#1103', '#1088#1077 +
          #1075#1080#1089#1090#1088#1099' '#1080' '#1090'.'#1076'.), '#1073#1091#1076#1091#1090' '#1088#1072#1079#1083#1086#1078#1077#1085#1099' '#1087#1086' '#1087#1072#1087#1082#1072#1084'. '#1055#1086' '#1091#1084#1086#1083#1095#1072#1085#1080#1102' '#1090#1072#1082#1080#1077' '#1086#1073 +
          #1098#1077#1082#1090#1099' '#1076#1077#1082#1086#1084#1087#1080#1083#1080#1088#1091#1102#1090#1089#1103' '#1074' '#1086#1076#1080#1085' mdp-'#1092#1072#1081#1083
        Caption = #1056#1072#1089#1082#1083#1072#1076#1099#1074#1072#1090#1100' '#1086#1073#1098#1077#1082#1090#1099' '#1084#1077#1090#1072#1076#1072#1085#1085#1099#1093' '#1087#1086' '#1087#1072#1087#1082#1072#1084
        TabOrder = 4
      end
      object cbSortUserRightsObjects: TCheckBox
        Left = 78
        Top = 116
        Width = 281
        Height = 17
        Caption = #1057#1086#1088#1090#1080#1088#1086#1074#1072#1090#1100' '#1086#1087#1080#1089#1072#1085#1080#1103' '#1087#1088#1072#1074' '#1087#1086' '#1080#1084#1077#1085#1072#1084' '#1086#1073#1098#1077#1082#1090#1086#1074
        TabOrder = 8
      end
      object cbNoOrdering: TCheckBox
        Left = 78
        Top = 58
        Width = 275
        Height = 17
        Caption = #1053#1077' '#1089#1086#1079#1076#1072#1074#1072#1090#1100' '#1092#1072#1081#1083#1099' *.ord'
        TabOrder = 5
      end
      object cbNoProfiles: TCheckBox
        Left = 78
        Top = 73
        Width = 265
        Height = 17
        Caption = #1053#1077' '#1089#1086#1079#1076#1072#1074#1072#1090#1100' '#1092#1072#1081#1083#1099' Container.Profile'
        TabOrder = 6
      end
      object cbNoDefaults: TCheckBox
        Left = 78
        Top = 88
        Width = 337
        Height = 17
        Hint = 
          #1045#1089#1083#1080' '#1101#1090#1072' '#1086#1087#1094#1080#1103' '#1074#1099#1082#1083#1102#1095#1077#1085#1072', '#1090#1086' '#1074' '#1086#1087#1080#1089#1072#1085#1080#1103' '#1076#1080#1072#1083#1086#1075#1086#1074' '#1080' '#1084#1077#1090#1072#1076#1072#1085#1085#1099#1093' '#1085#1077 +
          ' '#1073#1091#1076#1091#1090' '#1074#1099#1074#1086#1076#1080#1090#1100#1089#1103' '#1087#1086#1083#1103', '#1080#1084#1077#1102#1097#1080#1077' '#1079#1085#1072#1095#1077#1085#1080#1103' '#1087#1086' '#1091#1084#1086#1083#1095#1072#1085#1080#1102
        Caption = #1042#1099#1074#1086#1076#1080#1090#1100' '#1074#1089#1077' '#1079#1085#1072#1095#1077#1085#1080#1103' '#1074' '#1086#1087#1080#1089#1072#1085#1080#1103#1093' '#1084#1077#1090#1072#1076#1072#1085#1085#1099#1093' '#1080' '#1076#1080#1072#1083#1086#1075#1086#1074
        TabOrder = 7
      end
      object cbSortFormControls: TCheckBox
        Left = 78
        Top = 131
        Width = 265
        Height = 17
        Caption = #1057#1086#1088#1090#1080#1088#1086#1074#1072#1090#1100' '#1101#1083#1077#1084#1077#1085#1090#1099' '#1076#1080#1072#1083#1086#1075#1086#1074#1099#1093' '#1092#1086#1088#1084
        TabOrder = 9
      end
      object cbNoEmptyFolders: TCheckBox
        Left = 78
        Top = 161
        Width = 235
        Height = 17
        Caption = #1059#1076#1072#1083#1103#1090#1100' '#1087#1091#1089#1090#1099#1077' '#1087#1072#1087#1082#1080
        TabOrder = 10
      end
      object btnChooseDecompileFile: TBitBtn
        Left = 609
        Top = 0
        Width = 22
        Height = 20
        Anchors = [akTop, akRight]
        TabOrder = 1
        TabStop = False
        OnClick = btnChooseDecompileFileClick
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
      object btnChooseDecompileDir: TBitBtn
        Left = 609
        Top = 23
        Width = 22
        Height = 19
        Anchors = [akTop, akRight]
        TabOrder = 3
        TabStop = False
        OnClick = btnChooseDecompileDirClick
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
      object DecompileFilter: TEdit
        Left = 78
        Top = 179
        Width = 531
        Height = 21
        TabOrder = 12
        Text = 'DecompileFilter'
      end
      object cbUseDecompileFilter: TCheckBox
        Left = 8
        Top = 182
        Width = 65
        Height = 17
        Caption = #1060#1080#1083#1100#1090#1088
        TabOrder = 11
        OnClick = cbUseDecompileFilterClick
      end
      object BitBtn_Go: TBitBtn
        Left = 448
        Top = 203
        Width = 89
        Height = 25
        Caption = 'BitBtn_Go'
        TabOrder = 13
        Visible = False
        Glyph.Data = {
          F6000000424DF600000000000000760000002800000010000000100000000100
          04000000000080000000C40E0000C40E00001000000000000000000000000000
          8000008000000080800080000000800080008080000080808000C0C0C0000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00666666666666
          6666666660666666666666666006666666666666600066666666666660000666
          6666666660000066666666666000000666666666600000006666666660000000
          0666666660000000666666666000000666666666600000666666666660000666
          6666666660006666666666666006666666666666606666666666}
      end
      object BitBtn_Stop: TBitBtn
        Left = 544
        Top = 203
        Width = 89
        Height = 25
        Caption = 'BitBtn_Stop'
        TabOrder = 14
        Visible = False
        Glyph.Data = {
          F6000000424DF600000000000000760000002800000010000000100000000100
          04000000000080000000C40E0000C40E00001000000000000000000000000000
          8000008000000080800080000000800080008080000080808000C0C0C0000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00666666666666
          6666666666666666666666000000000000666600000000000066660000000000
          0066660000000000006666000000000000666600000000000066660000000000
          0066660000000000006666000000000000666600000000000066660000000000
          0066660000000000006666666666666666666666666666666666}
      end
      object btnDecompile: TBitBtn
        Left = 78
        Top = 204
        Width = 160
        Height = 25
        Caption = #1044#1077#1082#1086#1084#1087#1080#1083#1080#1088#1086#1074#1072#1090#1100
        TabOrder = 15
        OnClick = btnDecompileClick
      end
      object cbNoBrokenLinks: TCheckBox
        Left = 78
        Top = 146
        Width = 467
        Height = 17
        Caption = 
          #1059#1076#1072#1083#1103#1090#1100' '#1101#1083#1077#1084#1077#1085#1090#1099' '#1080#1085#1090#1077#1088#1092#1077#1081#1089#1086#1074' '#1080' '#1087#1088#1072#1074', '#1089#1089#1099#1083#1072#1102#1097#1080#1077#1089#1103' '#1085#1072' '#1085#1077#1089#1091#1097#1077#1089#1090#1074#1091#1102#1097 +
          #1080#1077' '#1086#1073#1098#1077#1082#1090#1099
        TabOrder = 16
      end
      object cbNotParseForms: TCheckBox
        Left = 78
        Top = 102
        Width = 177
        Height = 17
        Hint = 
          #1054#1087#1080#1089#1072#1085#1080#1103' '#1076#1080#1072#1083#1086#1075#1086#1074#1099#1093' '#1092#1086#1088#1084' '#1073#1091#1076#1091#1090' '#1074' '#1090#1086#1084' '#1074#1080#1076#1077', '#1082#1072#1082' '#1086#1085#1080' '#1083#1077#1078#1072#1090' '#1074' MD-'#1092#1072 +
          #1081#1083#1077', '#1073#1077#1079' '#1082#1072#1082#1086#1081' '#1083#1080#1073#1086' '#1086#1073#1088#1072#1073#1086#1090#1082#1080'.'
        Caption = #1042#1099#1090#1072#1089#1082#1080#1074#1072#1090#1100' '#1076#1080#1072#1083#1086#1075#1080' as is'
        TabOrder = 17
      end
    end
    object CompileParameters: TTabSheet
      Caption = #1050#1086#1084#1087#1080#1083#1103#1094#1080#1103
      ImageIndex = 2
      DesignSize = (
        635
        229)
      object Label3: TLabel
        Left = 14
        Top = 6
        Width = 57
        Height = 13
        Alignment = taRightJustify
        Caption = #1048#1084#1103' '#1092#1072#1081#1083#1072
      end
      object Label4: TLabel
        Left = 30
        Top = 31
        Width = 41
        Height = 13
        Alignment = taRightJustify
        Caption = #1050#1072#1090#1072#1083#1086#1075
      end
      object CompileFileName: TEdit
        Left = 78
        Top = 2
        Width = 520
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
        Text = 'CompileFileName'
        OnEnter = CompileFileNameEnter
        OnExit = CompileFileNameExit
      end
      object CompileDirectory: TEdit
        Left = 78
        Top = 27
        Width = 520
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 1
        Text = 'CompileDirectory'
      end
      object btnChooseCompileFile: TBitBtn
        Left = 598
        Top = 1
        Width = 22
        Height = 23
        Anchors = [akTop, akRight]
        TabOrder = 2
        TabStop = False
        OnClick = btnChooseCompileFileClick
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
      object btnChooseCompileDir: TBitBtn
        Left = 598
        Top = 27
        Width = 22
        Height = 22
        Anchors = [akTop, akRight]
        TabOrder = 3
        TabStop = False
        OnClick = btnChooseCompileDirClick
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
      object CompileFilter: TEdit
        Left = 78
        Top = 165
        Width = 531
        Height = 21
        TabOrder = 5
        Text = 'CompileFilter'
      end
      object cbUseCompileFilter: TCheckBox
        Left = 8
        Top = 168
        Width = 68
        Height = 17
        Caption = #1060#1080#1083#1100#1090#1088
        TabOrder = 4
        OnClick = cbUseCompileFilterClick
      end
      object btnCompile: TBitBtn
        Left = 78
        Top = 194
        Width = 160
        Height = 25
        Caption = #1050#1086#1084#1087#1080#1083#1080#1088#1086#1074#1072#1090#1100
        TabOrder = 6
        OnClick = btnCompileClick
      end
    end
    object CommonParameters: TTabSheet
      Caption = #1054#1073#1097#1080#1077' '#1087#1072#1088#1072#1084#1077#1090#1088#1099
      ImageIndex = 1
      object Bevel1: TBevel
        Left = 16
        Top = 8
        Width = 609
        Height = 185
      end
      object cbSaveVersion: TCheckBox
        Left = 27
        Top = 12
        Width = 249
        Height = 17
        Caption = #1057#1086#1093#1088#1072#1085#1103#1090#1100' '#1074#1077#1088#1089#1080#1102' gcomp '#1074' '#1092#1072#1081#1083' '#39'gcomp.ver'#39
        TabOrder = 0
      end
      object cbCreateIniFile: TCheckBox
        Left = 27
        Top = 28
        Width = 297
        Height = 17
        Caption = #1057#1086#1079#1076#1072#1074#1072#1090#1100' '#1092#1072#1081#1083' gcomp.ini '#1074' '#1082#1072#1090#1072#1083#1086#1075#1077' '#1076#1077#1082#1086#1084#1087#1080#1083#1103#1094#1080#1080
        TabOrder = 1
      end
      object cbNoEmptyMxl: TCheckBox
        Left = 334
        Top = 12
        Width = 217
        Height = 17
        Caption = #1059#1076#1072#1083#1103#1090#1100' '#1087#1091#1089#1090#1099#1077' '#1096#1072#1073#1083#1086#1085#1099' '#1090#1072#1073#1083#1080#1094
        TabOrder = 2
      end
      object cbTruncateMMS: TCheckBox
        Left = 334
        Top = 28
        Width = 289
        Height = 17
        Caption = #1055#1091#1089#1090#1086#1081' MainMetadataStream '#1076#1083#1103' '#1074#1085#1077#1096#1085#1080#1093' '#1086#1090#1095#1105#1090#1086#1074
        TabOrder = 3
      end
      object rgVerbosity: TRadioGroup
        Left = 27
        Top = 124
        Width = 337
        Height = 65
        Caption = #1057#1086#1086#1073#1097#1077#1085#1080#1103' '#1086' '#1093#1086#1076#1077' '#1088#1072#1073#1086#1090#1099
        ItemIndex = 1
        Items.Strings = (
          #1058#1086#1083#1100#1082#1086' '#1086#1096#1080#1073#1082#1080
          #1054#1096#1080#1073#1082#1080', '#1089#1086#1079#1076#1072#1074#1072#1077#1084#1099#1077', '#1086#1073#1085#1086#1074#1083#1103#1077#1084#1099#1077' '#1080' '#1091#1076#1072#1083#1103#1077#1084#1099#1077' '#1092#1072#1081#1083#1099
          #1050#1091#1095#1072' '#1076#1086#1087#1086#1083#1085#1080#1090#1077#1083#1100#1085#1086#1081' '#1080#1085#1092#1086#1088#1084#1072#1094#1080#1080)
        TabOrder = 6
      end
      object rgLanguage: TRadioGroup
        Left = 335
        Top = 49
        Width = 249
        Height = 65
        Caption = #1071#1079#1099#1082' '#1089#1086#1079#1076#1072#1074#1072#1077#1084#1099#1093' '#1082#1072#1090#1072#1083#1086#1075#1086#1074' '#1080' '#1092#1072#1081#1083#1086#1074
        ItemIndex = 0
        Items.Strings = (
          #1056#1091#1089#1089#1082#1080#1081
          #1040#1085#1075#1083#1080#1081#1089#1082#1080#1081
          #1058#1088#1072#1085#1089#1083#1080#1090)
        TabOrder = 5
      end
      object rgCompoundType: TRadioGroup
        Left = 27
        Top = 49
        Width = 185
        Height = 65
        Caption = #1058#1080#1087' '#1082#1086#1084#1087#1072#1091#1085#1076#1072
        ItemIndex = 0
        Items.Strings = (
          #1050#1086#1085#1092#1080#1075#1091#1088#1072#1094#1080#1103' 1'#1057' (md-'#1092#1072#1081#1083')'
          #1042#1085#1077#1096#1085#1080#1081' '#1086#1090#1095#1105#1090' (ert-'#1092#1072#1081#1083')')
        TabOrder = 4
      end
      object cbSyncDecompileDir: TCheckBox
        Left = 32
        Top = 201
        Width = 369
        Height = 17
        Caption = #1057#1080#1085#1093#1088#1086#1085#1080#1079#1080#1088#1086#1074#1072#1090#1100' '#1082#1072#1090#1072#1083#1086#1075' '#1076#1077#1082#1086#1084#1087#1080#1083#1103#1094#1080#1080' '#1089' '#1088#1072#1089#1087#1083#1086#1083#1078#1077#1085#1080#1077#1084' '#1092#1072#1081#1083#1072
        TabOrder = 7
      end
    end
  end
  object ProgressBar: TProgressBar
    Left = 0
    Top = 442
    Width = 557
    Height = 16
    Anchors = [akLeft, akRight, akBottom]
    Min = 0
    Max = 100
    TabOrder = 1
  end
  object OpenDialog: TOpenDialog
    Filter = '*.md|*.md|*.ert|*.ert'
    Left = 536
  end
end
