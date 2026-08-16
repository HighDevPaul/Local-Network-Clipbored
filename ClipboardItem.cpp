//---------------------------------------------------------------------------
#pragma hdrstop
#include "ClipboardItem.h"
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)

// Constructor
TClipboardItem::TClipboardItem(int AID, String AText)
{
    FID = AID;
    FText = AText;
    FTimestamp = Now();
}

// Destructor
TClipboardItem::~TClipboardItem()
{
}

// Getters
int TClipboardItem::GetID()
{
    return FID;
}

String TClipboardItem::GetText()
{
    return FText;
}

TDateTime TClipboardItem::GetTimestamp()
{
    return FTimestamp;
}

// Setters
void TClipboardItem::SetID(int value)
{
    FID = value;
}

void TClipboardItem::SetText(String value)
{
    FText = value;
}

void TClipboardItem::SetTimestamp(TDateTime value)
{
    FTimestamp = value;
}

// Metodă ToString
String TClipboardItem::ToString()
{
    int maxLen = std::min(50, FText.Length());
    return String().sprintf(L"[%d] %s - %s",
        FID,
        DateTimeToStr(FTimestamp).c_str(),
        FText.SubString(1, maxLen).c_str());
}

// Metodă ToNetworkString
String TClipboardItem::ToNetworkString()
{
    double timestampValue = static_cast<double>(FTimestamp);
    String timestamp = FloatToStr(timestampValue);

    return String().sprintf(L"%d|%s|%d|%s",
        FID,
        timestamp.c_str(),
        FText.Length(),
        FText.c_str());
}

// Metodă statică FromNetworkString
TClipboardItem* TClipboardItem::FromNetworkString(String AData)
{
    TClipboardItem* result = NULL;

    try
    {
        int pos1 = 0, pos2 = 0, pos3 = 0;

        // Găsim separatorii "|"
        for (int i = 1; i <= AData.Length(); i++)
        {
            if (AData[i] == L'|')
            {
                if (pos1 == 0)
                    pos1 = i;
                else if (pos2 == 0)
                    pos2 = i;
                else if (pos3 == 0)
                {
                    pos3 = i;
                    break;
                }
            }
        }

        if (pos1 == 0 || pos2 == 0 || pos3 == 0)
            return NULL;

        int id = StrToInt(AData.SubString(1, pos1 - 1));
        String timestampStr = AData.SubString(pos1 + 1, pos2 - pos1 - 1);
        int textLen = StrToInt(AData.SubString(pos2 + 1, pos3 - pos2 - 1));
        String text = AData.SubString(pos3 + 1, textLen);

        result = new TClipboardItem(id, text);

        double timestampValue = StrToFloat(timestampStr);
        result->SetTimestamp(TDateTime(timestampValue));
    }
    catch(...)
    {
        if (result != NULL)
        {
            delete result;
            result = NULL;
        }
    }

    return result;
}
