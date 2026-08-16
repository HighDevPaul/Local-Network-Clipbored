//---------------------------------------------------------------------------
#ifndef ClipboardItemH
#define ClipboardItemH
//---------------------------------------------------------------------------
#include <System.hpp>
#include <System.DateUtils.hpp>

class TClipboardItem
{
private:
    int FID;
    String FText;
    TDateTime FTimestamp;

public:
    TClipboardItem(int AID, String AText);
    ~TClipboardItem();

    String ToString();
    String ToNetworkString();
    static TClipboardItem* FromNetworkString(String AData);

    // Getters și Setters - DOAR DECLARAȚII
    int GetID();
    void SetID(int value);

    String GetText();
    void SetText(String value);

    TDateTime GetTimestamp();
    void SetTimestamp(TDateTime value);

    __property int ID = {read=GetID, write=SetID};
    __property String Text = {read=GetText, write=SetText};
    __property TDateTime Timestamp = {read=GetTimestamp, write=SetTimestamp};
};

#endif
