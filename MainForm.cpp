//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "MainForm.h"
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    FClipboardManager = new TClipboardManager(100);
    FIsServer = false;

    // Setări inițiale
    RadioGroupMode->ItemIndex = 0;
    MemoStatus->Clear();
    MemoClipboard->Clear();

	Log("Aplicatie pornita");
    UpdateUIMode();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
    if (ServerSocket->Active)
        ServerSocket->Active = false;

    if (ClientSocket->Active)
        ClientSocket->Active = false;

    delete FClipboardManager;
}
//---------------------------------------------------------------------------
void TMainForm::Log(String AMessage)
{
    String timestamp = FormatDateTime("hh:nn:ss", Now());
    MemoStatus->Lines->Add(timestamp + " - " + AMessage);

    // Auto-scroll la ultimul mesaj
    SendMessage(MemoStatus->Handle, EM_LINESCROLL, 0, MemoStatus->Lines->Count);
}
//---------------------------------------------------------------------------
void TMainForm::UpdateUIMode()
{
    FIsServer = (RadioGroupMode->ItemIndex == 0);

    // Arată/ascunde grupurile corespunzătoare
    GroupBoxServer->Visible = FIsServer;
    GroupBoxClient->Visible = !FIsServer;

    if (FIsServer)
    {
        LabelStatus->Caption = "Mod: SERVER";
        LabelStatus->Font->Color = clBlue;
    }
    else
    {
        LabelStatus->Caption = "Mod: CLIENT";
        LabelStatus->Font->Color = clGreen;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RadioGroupModeClick(TObject *Sender)
{
    // Oprește conexiunile existente
    if (ServerSocket->Active)
        ButtonStopServerClick(NULL);

    if (ClientSocket->Active)
        ButtonDisconnectClick(NULL);

    UpdateUIMode();
	Log("Mod schimbat în:" + String(FIsServer ? " SERVER" : " CLIENT"));
}
//---------------------------------------------------------------------------
// ==================== FUNCȚII SERVER ====================
//---------------------------------------------------------------------------
void __fastcall TMainForm::ButtonStartServerClick(TObject *Sender)
{
    try
    {
        int port = StrToInt(EditServerPort->Text);
        ServerSocket->Port = port;
        ServerSocket->Active = true;

        Log("Server pornit pe portul " + IntToStr(port));
        LabelStatus->Caption = "Server ACTIV pe portul " + IntToStr(port);
        LabelStatus->Font->Color = clGreen;

        ButtonStartServer->Enabled = false;
        ButtonStopServer->Enabled = true;
        EditServerPort->Enabled = false;
        RadioGroupMode->Enabled = false;
    }
    catch(Exception &e)
    {
        ShowMessage("Eroare pornire server: " + e.Message);
        Log("EROARE pornire server: " + e.Message);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ButtonStopServerClick(TObject *Sender)
{
    ServerSocket->Active = false;

	Log("Server oprit");
    LabelStatus->Caption = "Server OPRIT";
    LabelStatus->Font->Color = clRed;

    ButtonStartServer->Enabled = true;
    ButtonStopServer->Enabled = false;
    EditServerPort->Enabled = true;
    RadioGroupMode->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ServerSocketClientConnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    Log("Client conectat: " + Socket->RemoteAddress);

	// Trimite lista completă la conectare
	String data = FClipboardManager->GetNetworkData();
    Socket->SendText(data);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ServerSocketClientDisconnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    Log("Client deconectat: " + Socket->RemoteAddress);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ServerSocketClientRead(TObject *Sender,
    TCustomWinSocket *Socket)
{
    String command = Socket->ReceiveText();
    ProcessServerCommand(Socket, command);
}
//---------------------------------------------------------------------------
void TMainForm::ProcessServerCommand(TCustomWinSocket *Socket, String ACommand)
{
    try
    {
        if (ACommand.Pos(L"ADD:") == 1)  // Adaugă L pentru wide string
        {
            String text = ACommand.SubString(5, ACommand.Length() - 4);
            TClipboardItem* item = FClipboardManager->AddItem(text);

            if (item)
            {
                int maxLen = std::min(30, text.Length());  // CORECTAT
                Log("Text adaugat (ID: " + IntToStr(item->ID) + "): " +
                    text.SubString(1, maxLen));
                RefreshClipboardList();

                // Confirmă clientului
                Socket->SendText("OK:" + IntToStr(item->ID) + "\r\n");

                // Notifică toți clienții
                String notification = "NEW:" + item->ToNetworkString() + "\r\n";
                BroadcastToClients(notification);
            }
            else
            {
                Socket->SendText("ERROR:Empty text\r\n");
            }
        }
        else if (ACommand == "GET_ALL")
        {
            String data = FClipboardManager->GetNetworkData();
            Socket->SendText(data);
			Log("Trimisa lista completa catre " + Socket->RemoteAddress);
        }
        else if (ACommand.Pos(L"GET:") == 1)  // Adaugă L
        {
            int id = StrToInt(ACommand.SubString(5, ACommand.Length() - 4));
            TClipboardItem* item = FClipboardManager->FindByID(id);

            if (item)
            {
                Socket->SendText("TEXT:" + item->Text + "\r\n");
				Log("Trimis text ID " + IntToStr(id) + " catre " + Socket->RemoteAddress);
            }
            else
            {
                Socket->SendText("ERROR:Not found\r\n");
            }
        }
        else if (ACommand.Pos(L"DELETE:") == 1)  // Adaugă L
        {
            int id = StrToInt(ACommand.SubString(8, ACommand.Length() - 7));
            FClipboardManager->RemoveItem(id);
            RefreshClipboardList();

            Socket->SendText("OK\r\n");
            BroadcastToClients("DELETE:" + IntToStr(id) + "\r\n");
			Log("Sters element ID: " + IntToStr(id));
        }
        else if (ACommand == "CLEAR")
        {
            FClipboardManager->Clear();
            RefreshClipboardList();

            Socket->SendText("OK\r\n");
            BroadcastToClients("CLEAR\r\n");
            Log("Clipboard golit");
        }
    }
    catch(Exception &e)
    {
        Log("Eroare procesare comanda: " + e.Message);
        Socket->SendText("ERROR:" + e.Message + "\r\n");
    }
}
//---------------------------------------------------------------------------
void TMainForm::BroadcastToClients(String AData)
{
    if (!ServerSocket->Active)
        return;

    // Trimite la toți clienții conectați
    for (int i = 0; i < ServerSocket->Socket->ActiveConnections; i++)
    {
        try
        {
            ServerSocket->Socket->Connections[i]->SendText(AData);
        }
        catch(...)
        {
            // Ignoră erorile de transmisie
        }
    }
}
//---------------------------------------------------------------------------
// ==================== FUNCȚII CLIENT ====================
//---------------------------------------------------------------------------
void __fastcall TMainForm::ButtonConnectClick(TObject *Sender)
{
    try
    {
        ClientSocket->Host = EditServerIP->Text;
        ClientSocket->Port = StrToInt(EditClientPort->Text);
        ClientSocket->Active = true;

        Log("Conectare la " + EditServerIP->Text + ":" + EditClientPort->Text);
    }
    catch(Exception &e)
    {
        ShowMessage("Eroare conectare: " + e.Message);
        Log("EROARE conectare: " + e.Message);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ButtonDisconnectClick(TObject *Sender)
{
    ClientSocket->Active = false;

    Log("Deconectat de la server");
    LabelStatus->Caption = "DECONECTAT";
    LabelStatus->Font->Color = clRed;

    ButtonConnect->Enabled = true;
    ButtonDisconnect->Enabled = false;
    ButtonAddToClipboard->Enabled = false;
    EditServerIP->Enabled = true;
    EditClientPort->Enabled = true;
    RadioGroupMode->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClientSocketConnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    Log("Conectat la server: " + Socket->RemoteAddress);
    LabelStatus->Caption = "CONECTAT la " + Socket->RemoteAddress;
    LabelStatus->Font->Color = clGreen;

    ButtonConnect->Enabled = false;
    ButtonDisconnect->Enabled = true;
    ButtonAddToClipboard->Enabled = true;
    EditServerIP->Enabled = false;
    EditClientPort->Enabled = false;
    RadioGroupMode->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClientSocketDisconnect(TObject *Sender,
    TCustomWinSocket *Socket)
{
    Log("Deconectat de la server");
    LabelStatus->Caption = "DECONECTAT";
    LabelStatus->Font->Color = clRed;

    ButtonConnect->Enabled = true;
    ButtonDisconnect->Enabled = false;
    ButtonAddToClipboard->Enabled = false;
    EditServerIP->Enabled = true;
    EditClientPort->Enabled = true;
    RadioGroupMode->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ClientSocketRead(TObject *Sender,
    TCustomWinSocket *Socket)
{
    String response = Socket->ReceiveText();
    ProcessClientResponse(response);
}
//---------------------------------------------------------------------------
void TMainForm::ProcessClientResponse(String AResponse)
{
    TStringList* responses = new TStringList();

    try
    {
        responses->Text = AResponse;

        for (int i = 0; i < responses->Count; i++)
        {
            String line = responses->Strings[i].Trim();
            if (line.IsEmpty())
                continue;

            ProcessSingleResponse(line);
        }

        delete responses;  // Șterge la final dacă totul merge bine
    }
    catch(Exception &e)
    {
        delete responses;  // Șterge și în caz de eroare
        Log("Eroare procesare răspuns: " + e.Message);
    }
    catch(...)
    {
		delete responses;  // Șterge pentru orice altă eroare
		Log("Eroare necunoscută la procesare răspuns");
    }
}
//---------------------------------------------------------------------------
void TMainForm::ProcessSingleResponse(String AResponse)
{
    if (AResponse.Pos(L"COUNT:") == 1)
    {
        // Primește lista completă
        FClipboardManager->LoadFromNetworkData(AResponse);
        RefreshClipboardList();
        Log("Lista primită de la server (" +
            IntToStr(FClipboardManager->Count) + " elemente)");
    }
    else if (AResponse.Pos(L"NEW:") == 1)
    {
        // Element nou adăugat
        String itemData = AResponse.SubString(5, AResponse.Length() - 4);
        TClipboardItem* item = TClipboardItem::FromNetworkString(itemData);

        if (item)
        {
            // Verifică dacă există deja
            if (!FClipboardManager->FindByID(item->ID))
            {
                // Adaugă direct în manager cu ID-ul de la server
                TClipboardItem* newItem = new TClipboardItem(item->ID, item->Text);
                newItem->SetTimestamp(item->Timestamp);

                // Hack: adaugă direct în vector
                FClipboardManager->AddItem(newItem->Text);

                RefreshClipboardList();
                Log("Element nou primit de la server (ID: " + IntToStr(item->ID) + ")");
            }
            delete item;
        }
    }
    else if (AResponse.Pos(L"DELETE:") == 1)
    {
        int id = StrToInt(AResponse.SubString(8, AResponse.Length() - 7));
        FClipboardManager->RemoveItem(id);
        RefreshClipboardList();
        Log("Element șters de server (ID: " + IntToStr(id) + ")");
    }
    else if (AResponse == "CLEAR")
    {
        FClipboardManager->Clear();
        RefreshClipboardList();
        Log("Clipboard golit de server");
    }
    else if (AResponse.Pos(L"OK:") == 1)
    {
        // Confirmare adăugare - REFRESH lista
        Log("Operație confirmată de server");

        // SOLICITĂ LISTA COMPLETĂ de la server
        if (ClientSocket->Active)
        {
            SendToServer("GET_ALL");
        }
    }
    else if (AResponse == "OK")
    {
        // Confirmare generică - REFRESH
        Log("Operație confirmată de server");

        if (ClientSocket->Active)
        {
            SendToServer("GET_ALL");
        }
    }
    else if (AResponse.Pos(L"ERROR:") == 1)
    {
        String error = AResponse.SubString(7, AResponse.Length() - 6);
        Log("EROARE de la server: " + error);
        ShowMessage("Eroare: " + error);
    }
}
//---------------------------------------------------------------------------
// ==================== FUNCȚII COMUNE ====================
//---------------------------------------------------------------------------
void TMainForm::RefreshClipboardList()
{
	ListBoxClipboards->Clear();

	for (int i = 0; i < FClipboardManager->Count; i++)
    {
        TClipboardItem* item = FClipboardManager->GetItem(i);
		if (item)
        {
            int maxLen = std::min(40, item->Text.Length());  // CORECTAT
            String display = String().sprintf(L"[%d] %s - %s",
                item->ID,
				DateTimeToStr(item->Timestamp).c_str(),
				item->Text.SubString(1, maxLen).c_str());

            ListBoxClipboards->Items->AddObject(display, (TObject*)(INT_PTR)item->ID);
        }
    }
}
//---------------------------------------------------------------------------
void TMainForm::SendToServer(String ACommand)
{
    if (ClientSocket->Active)
    {
        try
        {
            ClientSocket->Socket->SendText(ACommand + "\r\n");
        }
        catch(Exception &e)
        {
			Log("Eroare trimitere comanda: " + e.Message);
            ShowMessage("Eroare comunicare cu serverul: " + e.Message);
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ButtonAddToClipboardClick(TObject *Sender)
{
    String text = MemoClipboard->Text.Trim();

    if (text.IsEmpty())
    {
        ShowMessage("Introduceti text in memo!");
        return;
    }

    if (FIsServer)
    {
        // Mod server - adaugă direct
        TClipboardItem* item = FClipboardManager->AddItem(text);
        if (item)
        {
            RefreshClipboardList();
            MemoClipboard->Clear();
			Log("Text adaugat local (ID: " + IntToStr(item->ID) + ")");

            // Notifică clienții
            BroadcastToClients("NEW:" + item->ToNetworkString() + "\r\n");
        }
    }
    else
    {
        // Mod client - trimite la server
        if (!ClientSocket->Active)
        {
			ShowMessage("Nu sunteti conectat la server!");
            return;
        }

        SendToServer("ADD:" + text);
        MemoClipboard->Clear();
		Log("Comanda ADD trimisa la server");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ButtonLoadSelectedClick(TObject *Sender)
{
    if (ListBoxClipboards->ItemIndex < 0)
    {
		ShowMessage("Selectati un element din lista!");
        return;
    }

    TClipboardItem* item = FClipboardManager->GetItem(ListBoxClipboards->ItemIndex);
    if (item)
    {
        MemoClipboard->Text = item->Text;

        // Copiază și în clipboard-ul sistemului
        Clipboard()->AsText = item->Text;

		Log("Text incarcat (ID: " + IntToStr(item->ID) + ")");
		ShowMessage("Text incarcat si copiat în clipboard!");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ButtonClearAllClick(TObject *Sender)
{
	if (MessageDlg("Stergeti toate elementele din clipboard?",
        mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0) != mrYes)
        return;

    if (FIsServer)
    {
        // Mod server - șterge direct
        FClipboardManager->Clear();
        RefreshClipboardList();
        Log("Clipboard golit (server)");

        // Notifică clienții
        BroadcastToClients("CLEAR\r\n");
    }
    else
    {
        // Mod client - trimite comandă
        if (!ClientSocket->Active)
        {
			ShowMessage("Nu sunteti conectat la server!");
            return;
        }

        SendToServer("CLEAR");
		Log("Comanda CLEAR trimisa la server");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ListBoxClipboardsClick(TObject *Sender)
{
    if (ListBoxClipboards->ItemIndex >= 0)
    {
        TClipboardItem* item = FClipboardManager->GetItem(ListBoxClipboards->ItemIndex);
        if (item)
        {
            // Previzualizare în memo
            MemoClipboard->Text = item->Text;
        }
    }
}
//---------------------------------------------------------------------------

