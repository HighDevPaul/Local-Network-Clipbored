//---------------------------------------------------------------------------
#ifndef ClipboardManagerH
#define ClipboardManagerH
//---------------------------------------------------------------------------
#include "ClipboardItem.h"
#include <vector>

class TClipboardManager
{
private:
    std::vector<TClipboardItem*> FItems;
    int FNextID;
    int FMaxItems;

public:
    TClipboardManager(int AMaxItems = 50);
    ~TClipboardManager();

    TClipboardItem* AddItem(String AText);
    void RemoveItem(int AID);
    void Clear();
    TClipboardItem* FindByID(int AID);
    String GetAllAsString();
    String GetNetworkData();
    void LoadFromNetworkData(String AData);

    TClipboardItem* GetItem(int Index);
    int GetCount();

    int GetMaxItems();
    void SetMaxItems(int value);

    __property int Count = {read=GetCount};
    __property int MaxItems = {read=GetMaxItems, write=SetMaxItems};
};

#endif
