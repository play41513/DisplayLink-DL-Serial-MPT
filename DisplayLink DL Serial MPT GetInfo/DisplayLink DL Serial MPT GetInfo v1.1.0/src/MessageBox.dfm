object frmMsg: TfrmMsg
  Left = 0
  Top = 0
  BorderIcons = []
  ClientHeight = 311
  ClientWidth = 864
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Panel4: TPanel
    Left = 0
    Top = 0
    Width = 864
    Height = 313
    Align = alTop
    TabOrder = 0
    object plPASS_OK: TPanel
      Left = 1
      Top = 1
      Width = 400
      Height = 311
      Align = alLeft
      Caption = 'PASS'
      Color = clLime
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -67
      Font.Name = 'Times New Roman'
      Font.Style = []
      ParentBackground = False
      ParentFont = False
      TabOrder = 0
      OnClick = plPASS_OKClick
      ExplicitHeight = 439
    end
    object Panel2: TPanel
      Left = 383
      Top = 1
      Width = 480
      Height = 311
      Align = alRight
      Color = clBtnHighlight
      ParentBackground = False
      TabOrder = 1
      ExplicitHeight = 439
      object Panel1: TPanel
        Left = 1
        Top = 211
        Width = 478
        Height = 105
        Align = alTop
        TabOrder = 0
        ExplicitTop = 1
        ExplicitWidth = 418
        object plError7: TPanel
          Tag = 7
          Left = 1
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          Caption = '7.'#30059#38754#31895#26684
          Color = clRed
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -27
          Font.Name = #27161#26999#39636
          Font.Style = []
          ParentBackground = False
          ParentFont = False
          TabOrder = 0
          OnClick = plError1_1Click
        end
        object Panel17: TPanel
          Left = 160
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          TabOrder = 1
          object plError8_1: TPanel
            Tag = 8
            Left = 1
            Top = 1
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = '8.'#30059#38754#38275#29197
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -24
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitLeft = 6
            ExplicitTop = 4
          end
          object plError8_2: TPanel
            Tag = 8
            Left = 1
            Top = 49
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = #40657#38971#24460#36339#22238#30333#30059#38754
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -19
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
        end
        object Panel8: TPanel
          Left = 319
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          TabOrder = 2
          object plError9_2: TPanel
            Tag = 9
            Left = 1
            Top = 49
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = '('#36339#21205')'
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -24
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitLeft = 5
            ExplicitTop = 55
          end
          object plError9_1: TPanel
            Tag = 9
            Left = 1
            Top = 1
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = '9.'#24433#20687#38620#35338
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -24
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
        end
      end
      object Panel6: TPanel
        Left = 1
        Top = 1
        Width = 478
        Height = 105
        Align = alTop
        TabOrder = 1
        ExplicitWidth = 418
        object Panel7: TPanel
          Left = 319
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          Color = clRed
          ParentBackground = False
          TabOrder = 0
          object plError3_3: TPanel
            Tag = 3
            Left = 1
            Top = 65
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #19977#20998#37912#20197#19978
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitLeft = 5
            ExplicitTop = 66
          end
          object plError3_1: TPanel
            Tag = 3
            Left = 1
            Top = 1
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = '3.'#28204#35430#20013
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitLeft = 5
            ExplicitTop = -5
          end
          object plError3_2: TPanel
            Tag = 3
            Left = 1
            Top = 33
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #21934#19968#34722#24149#40657#38971
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 2
            OnClick = plError1_1Click
            ExplicitLeft = 5
            ExplicitTop = 27
          end
        end
        object Panel33: TPanel
          Left = 160
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          Color = clRed
          ParentBackground = False
          TabOrder = 1
          ExplicitLeft = 140
          object plError2_1: TPanel
            Tag = 2
            Left = 1
            Top = 1
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = '2.'#25554#19978#31435#21363#28961#30059#38754' '
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -17
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
          object plError2_2: TPanel
            Tag = 2
            Left = 1
            Top = 49
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = #38617#30059#38754
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -24
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
        end
        object Panel36: TPanel
          Left = 1
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          Color = clRed
          ParentBackground = False
          TabOrder = 2
          object plError1_2: TPanel
            Tag = 1
            Left = 1
            Top = 49
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = #21934#19968#30059#38754
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -24
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
          object plError1_1: TPanel
            Tag = 1
            Left = 1
            Top = 1
            Width = 157
            Height = 48
            Align = alTop
            BevelOuter = bvNone
            Caption = '1.'#25554#19978#31435#21363#28961#30059#38754
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -17
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitLeft = -3
            ExplicitTop = -5
          end
        end
      end
      object Panel5: TPanel
        Left = 1
        Top = 106
        Width = 478
        Height = 105
        Align = alTop
        TabOrder = 2
        ExplicitTop = 1
        ExplicitWidth = 418
        object Panel10: TPanel
          Left = 1
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          Color = clRed
          ParentBackground = False
          TabOrder = 0
          object plError4_3: TPanel
            Tag = 4
            Left = 1
            Top = 65
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #19977#20998#37912#20197#19979
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitLeft = -3
            ExplicitTop = 67
          end
          object plError4_1: TPanel
            Tag = 4
            Left = 1
            Top = 1
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = '4.'#28204#35430#20013
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
          object plError4_2: TPanel
            Tag = 4
            Left = 1
            Top = 33
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #21934#19968#34722#24149#40657#38971
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 2
            OnClick = plError1_1Click
            ExplicitLeft = -3
            ExplicitTop = 27
          end
        end
        object Panel42: TPanel
          Left = 160
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          Color = clRed
          ParentBackground = False
          TabOrder = 1
          ExplicitLeft = 167
          object plError5_3: TPanel
            Tag = 5
            Left = 1
            Top = 65
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #19977#20998#37912#20197#19978
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitLeft = -3
            ExplicitTop = 66
          end
          object plError5_1: TPanel
            Tag = 5
            Left = 1
            Top = 1
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = '5. '#28204#35430#20013
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
          object plError5_2: TPanel
            Tag = 5
            Left = 1
            Top = 33
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #38617#34722#24149#40657#38971
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 2
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
        end
        object Panel46: TPanel
          Left = 319
          Top = 1
          Width = 159
          Height = 103
          Align = alLeft
          Color = clRed
          ParentBackground = False
          TabOrder = 2
          ExplicitLeft = 333
          object plError6_3: TPanel
            Tag = 6
            Left = 1
            Top = 65
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #19977#20998#37912#20197#19979
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 0
            OnClick = plError1_1Click
            ExplicitWidth = 159
          end
          object plError6_1: TPanel
            Tag = 6
            Left = 1
            Top = 1
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = '6.'#28204#35430#20013
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 1
            OnClick = plError1_1Click
            ExplicitLeft = 5
            ExplicitTop = -2
          end
          object plError6_2: TPanel
            Tag = 6
            Left = 1
            Top = 33
            Width = 157
            Height = 32
            Align = alTop
            BevelOuter = bvNone
            Caption = #38617#34722#24149#40657#38971
            Color = clRed
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -20
            Font.Name = #27161#26999#39636
            Font.Style = []
            ParentBackground = False
            ParentFont = False
            TabOrder = 2
            OnClick = plError1_1Click
            ExplicitWidth = 137
          end
        end
      end
    end
  end
end
