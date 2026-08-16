//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <System.Win.ScktComp.hpp>
#include <Vcl.Clipbrd.hpp>
#include "ClipboardManager.h"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:
    TPanel *PanelTop;
    TRadioGroup *RadioGroupMode;
    TGroupBox *GroupBoxServer;
    TLabel *LabelServerPort;
    TEdit *EditServerPort;
    TButton *ButtonStartServer;
    TButton *ButtonStopServer;
    TGroupBox *GroupBoxClient;
    TLabel *LabelServerIP;
    TLabel *LabelClientPort;
    TEdit *EditServerIP;
    TEdit *EditClientPort;
    TButton *ButtonConnect;
    TButton *ButtonDisconnect;
    TPanel *PanelClient;
    TLabel *LabelClipboards;
    TLabel *LabelStatus;
    TMemo *MemoClipboard;
    TButton *ButtonAddToClipboard;
    TButton *ButtonLoadSelected;
    TButton *ButtonClearAll;
    TListBox *ListBoxClipboards;
    TMemo *MemoStatus;
    TServerSocket *ServerSocket;
    TClientSocket *ClientSocket;

    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall RadioGroupModeClick(TObject *Sender);
    void __fastcall ButtonStartServerClick(TObject *Sender);
    void __fastcall ButtonStopServerClick(TObject *Sender);
    void __fastcall ButtonConnectClick(TObject *Sender);
    void __fastcall ButtonDisconnectClick(TObject *Sender);
    void __fastcall ButtonAddToClipboardClick(TObject *Sender);
    void __fastcall ButtonLoadSelectedClick(TObject *Sender);
    void __fastcall ButtonClearAllClick(TObject *Sender);
    void __fastcall ListBoxClipboardsClick(TObject *Sender);
    void __fastcall ServerSocketClientRead(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall ServerSocketClientConnect(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall ServerSocketClientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall ClientSocketRead(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall ClientSocketConnect(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall ClientSocketDisconnect(TObject *Sender, TCustomWinSocket *Socket);

private:
    TClipboardManager* FClipboardManager;
    bool FIsServer;

    // Metode private - DOAR DECLARAȚII
    void Log(String AMessage);
    void RefreshClipboardList();
    void SendToServer(String ACommand);
    void BroadcastToClients(String AData);
    void ProcessServerCommand(TCustomWinSocket *Socket, String ACommand);
    void ProcessClientResponse(String AResponse);
    void UpdateUIMode();
    void ProcessSingleResponse(String AResponse);
public:
    __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
