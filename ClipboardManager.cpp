//---------------------------------------------------------------------------
#pragma hdrstop
#include "ClipboardManager.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

// Constructor
TClipboardManager::TClipboardManager(int AMaxItems)
{
    FNextID = 1;
    FMaxItems = AMaxItems;
}

// Destructor
TClipboardManager::~TClipboardManager()
{
    Clear();
}

// Getters
int TClipboardManager::GetCount()
{
    return FItems.size();
}

int TClipboardManager::GetMaxItems()
{
    return FMaxItems;
}

// Setters
void TClipboardManager::SetMaxItems(int value)
{
    FMaxItems = value;
}

// Metodă GetItem
TClipboardItem* TClipboardManager::GetItem(int Index)
{
    if (Index >= 0 && Index < (int)FItems.size())
        return FItems[Index];
    return NULL;
}

// Metodă AddItem
TClipboardItem* TClipboardManager::AddItem(String AText)
{
    if (AText.Trim().IsEmpty())
        return NULL;

    if (FItems.size() >= FMaxItems)
    {
        delete FItems[0];
        FItems.erase(FItems.begin());
    }

    TClipboardItem* item = new TClipboardItem(FNextID++, AText);
    FItems.push_back(item);
    return item;
}

// Metodă RemoveItem
void TClipboardManager::RemoveItem(int AID)
{
    for (size_t i = 0; i < FItems.size(); i++)
    {
        if (FItems[i]->ID == AID)
        {
            delete FItems[i];
            FItems.erase(FItems.begin() + i);
            break;
        }
    }
}

// Metodă Clear
void TClipboardManager::Clear()
{
    for (size_t i = 0; i < FItems.size(); i++)
    {
        delete FItems[i];
    }
    FItems.clear();
}

// Metodă FindByID
TClipboardItem* TClipboardManager::FindByID(int AID)
{
    for (size_t i = 0; i < FItems.size(); i++)
    {
        if (FItems[i]->ID == AID)
            return FItems[i];
    }
    return NULL;
}

// Metodă GetAllAsString
String TClipboardManager::GetAllAsString()
{
    String result = "";
    for (size_t i = 0; i < FItems.size(); i++)
    {
        result += FItems[i]->ToString() + "\r\n";
    }
    return result;
}

// Metodă GetNetworkData
String TClipboardManager::GetNetworkData()
{
    String result = String().sprintf(L"COUNT:%d\r\n", FItems.size());
    for (size_t i = 0; i < FItems.size(); i++)
    {
        result += FItems[i]->ToNetworkString() + "\r\n";
    }
    result += "END\r\n";
    return result;
}

// Metodă LoadFromNetworkData
void TClipboardManager::LoadFromNetworkData(String AData)
{
    Clear();

    TStringList* lines = new TStringList();
    try
    {
        lines->Text = AData;

        if (lines->Count > 0 && lines->Strings[0].Pos(L"COUNT:") == 1)
        {
            for (int i = 1; i < lines->Count; i++)
            {
                String line = lines->Strings[i].Trim();
                if (line.IsEmpty() || line == "END")
                    break;

                TClipboardItem* item = TClipboardItem::FromNetworkString(line);
                if (item != NULL)
                {
                    FItems.push_back(item);
                    if (item->ID >= FNextID)
                        FNextID = item->ID + 1;
                }
            }
        }
    }
    __finally
    {
        delete lines;
    }
}
