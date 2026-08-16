object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'Clipbored'
  ClientHeight = 600
  ClientWidth = 800
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Segoe UI'
  Font.Style = []
  Position = poScreenCenter
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  TextHeight = 15
  object PanelTop: TPanel
    Left = 0
    Top = 0
    Width = 800
    Height = 185
    Align = alTop
    TabOrder = 0
    ExplicitWidth = 798
    object RadioGroupMode: TRadioGroup
      Left = 16
      Top = 0
      Width = 185
      Height = 65
      Caption = 'Mod Aplicatie'
      Items.Strings = (
        'Server'
        'Client')
      TabOrder = 0
      OnClick = RadioGroupModeClick
    end
    object GroupBoxServer: TGroupBox
      Left = 225
      Top = 10
      Width = 273
      Height = 169
      Caption = 'Setari servar'
      TabOrder = 1
      object LabelServerPort: TLabel
        Left = 16
        Top = 24
        Width = 22
        Height = 15
        Caption = 'Port'
      end
      object EditServerPort: TEdit
        Left = 16
        Top = 43
        Width = 121
        Height = 23
        TabOrder = 0
        Text = '8888'
      end
      object ButtonStartServer: TButton
        Left = 16
        Top = 80
        Width = 121
        Height = 33
        Caption = 'Porneste Server'
        TabOrder = 1
        OnClick = ButtonStartServerClick
      end
      object ButtonStopServer: TButton
        Left = 16
        Top = 119
        Width = 121
        Height = 33
        Caption = 'Opreste server'
        Enabled = False
        TabOrder = 2
        OnClick = ButtonStopServerClick
      end
    end
    object GroupBoxClient: TGroupBox
      Left = 504
      Top = 8
      Width = 273
      Height = 169
      Caption = 'Setari Client'
      TabOrder = 2
      Visible = False
      object LabelServerIP: TLabel
        Left = 16
        Top = 24
        Width = 44
        Height = 15
        Caption = 'IP server'
      end
      object LabelClientPort: TLabel
        Left = 152
        Top = 24
        Width = 22
        Height = 15
        Caption = 'Port'
      end
      object EditServerIP: TEdit
        Left = 16
        Top = 43
        Width = 121
        Height = 23
        TabOrder = 0
        Text = '127.0.0.1'
      end
      object EditClientPort: TEdit
        Left = 152
        Top = 43
        Width = 105
        Height = 23
        TabOrder = 1
        Text = '8888'
      end
      object ButtonConnect: TButton
        Left = 16
        Top = 80
        Width = 121
        Height = 33
        Caption = 'Conecteaza'
        TabOrder = 2
        OnClick = ButtonConnectClick
      end
      object ButtonDisconnect: TButton
        Left = 16
        Top = 119
        Width = 121
        Height = 33
        Caption = 'Deconecteaza'
        Enabled = False
        TabOrder = 3
        OnClick = ButtonDisconnectClick
      end
    end
  end
  object PanelClient: TPanel
    Left = 0
    Top = 185
    Width = 800
    Height = 415
    Align = alClient
    Caption = 'PanelClient'
    TabOrder = 1
    ExplicitWidth = 798
    ExplicitHeight = 407
    object LabelClipboards: TLabel
      Left = 16
      Top = 16
      Width = 79
      Height = 15
      Caption = 'Lista Clipboard'
    end
    object LabelStatus: TLabel
      Left = 424
      Top = 224
      Width = 32
      Height = 15
      Caption = 'Status'
    end
    object MemoClipboard: TMemo
      Left = 16
      Top = 35
      Width = 385
      Height = 150
      Lines.Strings = (
        'MemoClipboard')
      ScrollBars = ssVertical
      TabOrder = 0
    end
    object ButtonAddToClipboard: TButton
      Left = 16
      Top = 191
      Width = 121
      Height = 33
      Caption = 'Adauga La Clipbord'
      TabOrder = 1
      OnClick = ButtonAddToClipboardClick
    end
    object ButtonLoadSelected: TButton
      Left = 143
      Top = 191
      Width = 121
      Height = 33
      Caption = 'Incarca Selectat'
      TabOrder = 2
      OnClick = ButtonLoadSelectedClick
    end
    object ButtonClearAll: TButton
      Left = 270
      Top = 191
      Width = 131
      Height = 33
      Caption = 'Sterge Toate'
      TabOrder = 3
      OnClick = ButtonClearAllClick
    end
    object ListBoxClipboards: TListBox
      Left = 16
      Top = 243
      Width = 385
      Height = 150
      ItemHeight = 15
      TabOrder = 4
      OnClick = ListBoxClipboardsClick
    end
    object MemoStatus: TMemo
      Left = 424
      Top = 243
      Width = 353
      Height = 150
      Lines.Strings = (
        'MemoStatus')
      ReadOnly = True
      ScrollBars = ssVertical
      TabOrder = 5
    end
  end
  object ServerSocket: TServerSocket
    Active = False
    Port = 0
    ServerType = stNonBlocking
    OnClientConnect = ServerSocketClientConnect
    OnClientDisconnect = ServerSocketClientDisconnect
    OnClientRead = ServerSocketClientRead
    Left = 448
    Top = 233
  end
  object ClientSocket: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Port = 0
    OnConnect = ClientSocketConnect
    OnDisconnect = ClientSocketDisconnect
    OnRead = ClientSocketRead
    Left = 528
    Top = 233
  end
end
