#include "stdAFX.h"

#include "CQuest.h"

#ifndef __SERVER
    #include "..\GameProc\CDayNNightProc.h"
    #include "../Game.h"
    #include "../interface/it_mgr.h"
    #include "common/io_quest.h"
#else
    #include "IO_Quest.h"
    extern DWORD Get_WorldPassTIME();
#endif

#include "nlohmann/json.hpp"

using json = nlohmann::json;

#define MAX_ASSOCIATION 20 // �ִ� ���� ����

// ���� ����Ʈ ���� ��ġ
// ģ�� ��ġ[ MAX_ASSOCIATION ] .	ó�� ���� �� : 0
// ����Ʈ ���� ��ġ[ MAX_ASSOCIATION ]
/*
// �������� ����Ʈ
{
    ����Ʈ ���� ���� 10��
    ����Ʈ ����ġ 8��
    ����Ʈ Ÿ�̸�
    ����Ʈ ������
}
*/

//-------------------------------------------------------------------------------------------------
void
CQUEST::Init() {
    m_wID = 0;
    ClearAllSwitch();
    ::ZeroMemory(m_pVAR, sizeof(m_pVAR));
    ::ZeroMemory(m_ITEMs, sizeof(m_ITEMs));
}

void
CQUEST::CheckExpiredTIME() {
    // if ( this->m_wID )
    {
        if (QUEST_TIME_LIMIT(this->m_wID)) {
            DWORD dwNewExpired = ::Get_WorldPassTIME() + QUEST_TIME_LIMIT(this->m_wID);
            if (this->m_dwExpirationTIME > dwNewExpired) {
                // ���� ���յǸ鼭 �ɸ������ɶ�... ����Ÿ���� Ʋ�����ϱ�...
                this->m_dwExpirationTIME = dwNewExpired;
            }
        }
    }
}

void
CQUEST::SetID(WORD wID, bool bResetTimer) {
    m_wID = wID;

    if (bResetTimer) {
        if (QUEST_TIME_LIMIT(wID)) {
            m_dwExpirationTIME = ::Get_WorldPassTIME() + QUEST_TIME_LIMIT(wID);
        } else
            m_dwExpirationTIME = 0; // ������..
    } else if (QUEST_TIME_LIMIT(wID)) {
        // ������ �ʿ���� ����ε�...�ٲ�°��� �ð� ������ �ʿ��ϸ�...
        m_dwExpirationTIME = ::Get_WorldPassTIME() + QUEST_TIME_LIMIT(wID);
    }
}

//-------------------------------------------------------------------------------------------------
DWORD
CQUEST::GetRemainTIME() {
    DWORD dwCurTime = ::Get_WorldPassTIME();

    if (dwCurTime >= m_dwExpirationTIME) {
        // ���� �ð� ����.
        return 0;
    }

    return (m_dwExpirationTIME - dwCurTime);
}

//-------------------------------------------------------------------------------------------------
void
CQUEST::Set_VAR(int iVarNO, int iValue) {
    if (iVarNO < 0 || iVarNO >= QUEST_VAR_PER_QUEST)
        return;

    m_pVAR[iVarNO] = iValue;
}
int
CQUEST::Get_VAR(int iVarNO) {
    if (iVarNO < 0 || iVarNO >= QUEST_VAR_PER_QUEST)
        return 0;
    return m_pVAR[iVarNO];
}

//-------------------------------------------------------------------------------------------------
void
CQUEST::ClearAllSwitch() {
    ::ZeroMemory(m_btSWITCHES, sizeof(m_btSWITCHES));
}
BYTE
CQUEST::GetBit(int iIndex) {
    return (m_btSWITCHES[iIndex >> BIT_SHIFT] & g_btSwitchBitMask[iIndex & WHICH_BIT]) ? 1 : 0;
}
void
CQUEST::SetBit(int iIndex) {
    m_btSWITCHES[iIndex >> BIT_SHIFT] |= g_btSwitchBitMask[iIndex & WHICH_BIT];
}
void
CQUEST::ClearBit(int iIndex) {
    m_btSWITCHES[iIndex >> BIT_SHIFT] &= ~(g_btSwitchBitMask[iIndex & WHICH_BIT]);
}
BYTE
CQUEST::FlipBit(int iIndex) {
    if (this->GetBit(iIndex))
        this->ClearBit(iIndex);
    else
        this->SetBit(iIndex);

    return this->GetBit(iIndex);
}

//-------------------------------------------------------------------------------------------------
void
CQUEST::Set_SWITCH(int iSwitchNO, int iValue) {
    if (iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_PER_QUEST)
        return;

    if (iValue)
        this->SetBit(iSwitchNO);
    else
        this->ClearBit(iSwitchNO);
}
int
CQUEST::Get_SWITCH(int iSwitchNO) {
    if (iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_PER_QUEST)
        return -1;

    return this->GetBit(iSwitchNO);
}
int
CQUEST::Flip_SWITCH(int iSwitchNO) {
    if (iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_PER_QUEST)
        return -1;

    return this->FlipBit(iSwitchNO);
}

//-------------------------------------------------------------------------------------------------
tagBaseITEM*
CQUEST::GetSlotITEM(int iSlotNo) {
    if (iSlotNo < 0 || iSlotNo >= QUEST_ITEM_PER_QUEST)
        return NULL;

    return &(m_ITEMs[iSlotNo]);
}
tagBaseITEM*
CQUEST::GetSameITEM(WORD wItemHEADER) {
    for (short nI = 0; nI < QUEST_ITEM_PER_QUEST; nI++) {
        if (this->m_ITEMs[nI].GetHEADER() == wItemHEADER)
            return &this->m_ITEMs[nI];
    }
    return NULL;
}
bool
CQUEST::SubITEM(tagITEM& sITEM) {
    for (short nI = 0; nI < QUEST_ITEM_PER_QUEST; nI++) {
        if (this->m_ITEMs[nI].GetHEADER() == sITEM.GetHEADER()) {
            tagITEM sOriITEM;
            sOriITEM = this->m_ITEMs[nI];
            sOriITEM.SubtractOnly(sITEM);
            this->m_ITEMs[nI] = sOriITEM;

#ifndef __SERVER
            g_itMGR.AppendChatMsg(sITEM.SubtractQuestMESSAGE(), IT_MGR::CHAT_TYPE_QUEST);
#endif
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------------
bool
CQUEST::AddITEM(tagITEM& sITEM) {
    short nI;

    if (sITEM.IsEnableDupCNT()) {
        // �ߺ� ����, ���� �������� �ִ°� ??
        for (nI = 0; nI < QUEST_ITEM_PER_QUEST; nI++) {
            if (this->m_ITEMs[nI].GetHEADER() == sITEM.GetHEADER()) {
                this->m_ITEMs[nI].m_uiQuantity += sITEM.GetQuantity();
                return true;
            }
        }
    }

    // ����� �߰�..
    for (nI = 0; nI < QUEST_ITEM_PER_QUEST; nI++) {
        if (0 == this->m_ITEMs[nI].GetHEADER()) {
            this->m_ITEMs[nI] = sITEM;
            return true;
        }
    }

    return false;
}

void to_json(json& j, const CQUEST& q) {
    json vars = json::array();
    for (size_t i = 0; i < QUEST_VAR_PER_QUEST; ++i) {
        vars.push_back(q.m_pVAR[i]);
    }

    json switches = json::array();
    for (size_t i = 0; i < (QUEST_SWITCH_PER_QUEST / 8); ++i) {
        const uint8_t b = q.m_btSWITCHES[i];
        switches.push_back((b >> 0) & 0x1);
        switches.push_back((b >> 1) & 0x1);
        switches.push_back((b >> 2) & 0x1);
        switches.push_back((b >> 3) & 0x1);
        switches.push_back((b >> 4) & 0x1);
        switches.push_back((b >> 5) & 0x1);
        switches.push_back((b >> 6) & 0x1);
        switches.push_back((b >> 7) & 0x1);
    }

    json items = json::array();
    for (size_t i = 0; i < QUEST_ITEM_PER_QUEST; ++i) {
        // TODO: Serialize tagBaseITEM
    }

    j["id"] = q.m_wID;
    j["expiration"] = q.m_dwExpirationTIME;
    j["vars"] = vars;
    j["switches"] = switches;
    j["items"] = items;
}

void
from_json(const json&j, CQUEST& q) {
    if (!j.is_object()) {
        return;
    }

    if (j.contains("id") && j["id"].is_number_integer()) {
        q.m_wID = j["id"];
    }

    if (j.contains("expiration") && j["expiration"].is_number_integer()) {
        q.m_dwExpirationTIME = j["expiration"];
    }

    if (j.contains("vars") && j["vars"].is_array()) {
        json vars = j["vars"];
        for (size_t i = 0; i < min(vars.size(), QUEST_VAR_PER_QUEST); ++i) {
            q.m_pVAR[i] = vars[i];
        }
    }

    if (j.contains("switches") && j["switches"].is_array()) {
        json switches = j["switches"];
        for (size_t i = 0; i < min(switches.size(), QUEST_SWITCH_PER_QUEST) / 8; ++i) {
            uint8_t b = 0;
            b |= switches[i] << 0;
            b |= switches[i] << 1;
            b |= switches[i] << 2;
            b |= switches[i] << 3;
            b |= switches[i] << 4;
            b |= switches[i] << 5;
            b |= switches[i] << 6;
            b |= switches[i] << 7;
            q.m_btSWITCHES[i] = b;
        }
    }

    if (j.contains("items") && j["items"].is_array()) {
        json items = j["items"];
        // TODO: Deserialize tagBaseITEM
    }
}
