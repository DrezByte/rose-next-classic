#pragma once

#ifdef __SERVER
    #include "choticon.h"
    #include "cquest.h"
    #include "io_stb.h"
#else
    #include "common/choticon.h"
    #include "common/cquest.h"
    #include "common/io_stb.h"
#endif

#include "calculation.h"
#include "cinventory.h"

#include "rose/common/game_config.h"

#pragma warning(disable : 4201)

#ifdef __BORLANDC__
    #include <crtdbg.h>
#endif

#pragma pack(push, 1)
struct tagBasicINFO {
    // NAME
    // SEX
    char m_cBirthStone;
    char m_cFaceIDX;
    char m_cHairIDX;
    short m_nClass;
    char m_cUnion;
    char m_cRank;
    char m_cFame;

    void Init(char cBirthStone, char cFaceIDX, char cHairIDX) {
        m_cBirthStone = cBirthStone;
        m_cFaceIDX = cFaceIDX;
        m_cHairIDX = cHairIDX;

        m_nClass = 0;
        m_cUnion = 0;
        m_cRank = 0;
        m_cFame = 0;
    }
};

struct tagBasicAbility {
    union {
        struct {
            short m_nSTR;
            short m_nDEX;
            short m_nINT;
            short m_nCON;
            short m_nCHARM;
            short m_nSENSE;
        };
        short m_nBasicA[BA_MAX];
    };
    void Init() {
        m_nSTR = 10;
        m_nDEX = 10;
        m_nINT = 10;
        m_nCON = 10;
        m_nCHARM = 10;
        m_nSENSE = 10;
    }
};

#define MAX_EXPERIENCE 0x7FFFFFFFFFFFFFFF
#define MAX_STAMINA 5000

#define MAX_MAINTAIN_STATUS 40 // 8 * 50 = 400
struct tagMaintainSTATUS {
    DWORD m_dwExpiredSEC;
    short m_nValue;
    short m_nDummy;
};

struct tagGrowAbility {
    union {
        struct {
            short m_nHP;
            short m_nMP;

            __int64 m_lEXP;
            short m_nLevel;
            short m_nBonusPoint;
            short m_nSkillPoint;
            BYTE m_btBodySIZE;
            BYTE m_btHeadSIZE;
            __int64 m_lPenalEXP;

            short m_nFameG;
            short m_nFameB;
            short m_nUnionPOINT[MAX_UNION_COUNT];

            int m_iGuildNO;
            short m_nGuildCNTRB;
            BYTE m_btGuildPOS;

            short m_nPKFlag;
            short m_nSTAMINA;

            tagMaintainSTATUS m_STATUS[MAX_MAINTAIN_STATUS];

#ifdef __KCHS_BATTLECART__
            short m_nPatHP;
            DWORD m_dwPatCoolTIME;
#endif
        };
        BYTE m_pDATA[1];
    };

    void Init() {
        ::ZeroMemory(m_pDATA, sizeof(tagGrowAbility));
        m_nHP = 50;
        m_nMP = 40;

        m_lEXP = 0;
        m_nLevel = 1;
        m_btBodySIZE = 100; // ����ũ��
        m_btHeadSIZE = 100; // �Ӹ�ũ��
        m_nSTAMINA = MAX_STAMINA;

#ifndef __SERVER
        m_nBonusPoint = 0;
        m_nSkillPoint = 0;
        m_lPenalEXP = 0;
        m_nFameG = m_nFameB = 0;
        m_iGuildNO = m_nGuildCNTRB = m_btGuildPOS = 0;
        ::ZeroMemory(m_nUnionPOINT, sizeof(m_nUnionPOINT));
        m_nPKFlag = 0;
    #ifdef __KCHS_BATTLECART__
        m_nPatHP = 0;
        m_dwPatCoolTIME = 0;
    #endif

#endif
    }

#ifdef __GAMESERVER
    bool UpdateSTATUS(DWORD dwCurAbsSEC, int iAbilityType, int i10Min, short nValue) {
        short nIndex = iAbilityType - (int)AT_MAINTAIN_ABILITY;
        if (nIndex >= 0 && nIndex < MAX_MAINTAIN_STATUS) {
            // ����ð� + �Է°� * 10��
            if (this->m_STATUS[nIndex].m_dwExpiredSEC > dwCurAbsSEC) {
                // �ð� �߰�...
                this->m_STATUS[nIndex].m_dwExpiredSEC += (i10Min * 10 * 60);
            } else {
                // �ð� ����...
                this->m_STATUS[nIndex].m_dwExpiredSEC = dwCurAbsSEC + (i10Min * 10 * 60);
            }
            this->m_STATUS[nIndex].m_nValue = nValue;
            return true;
        }
        return false;
    }
    bool IsBankFREE(DWORD dwCurAbsSEC) {
        return (this->m_STATUS[AT_BANK_FREE - AT_MAINTAIN_ABILITY].m_dwExpiredSEC > dwCurAbsSEC);
    }
    bool IsBankAddON(DWORD dwCurAbsSEC) {
        return (this->m_STATUS[AT_BANK_ADDON - AT_MAINTAIN_ABILITY].m_dwExpiredSEC > dwCurAbsSEC);
    }
    short GetStoreSKIN(DWORD dwCurAbsSEC) {
        if (this->m_STATUS[AT_STORE_SKIN - AT_MAINTAIN_ABILITY].m_dwExpiredSEC > dwCurAbsSEC)
            return this->m_STATUS[AT_STORE_SKIN - AT_MAINTAIN_ABILITY].m_nValue;
        return 0;
    }
#endif
};

/// �ƹ�Ÿ ������ų - �������� 120���� ���� ��ȹ���� ����� 90��
#define MAX_LEARNED_SKILL_CNT 120
#define MAX_LEARNED_SKILL_PAGE 4
#define MAX_LEARNED_SKILL_PER_PAGE (MAX_LEARNED_SKILL_CNT / MAX_LEARNED_SKILL_PAGE)

struct tagSkillAbility {
    union {
        short m_nSkillINDEX[MAX_LEARNED_SKILL_CNT];
        short m_nPageIndex[MAX_LEARNED_SKILL_PAGE][MAX_LEARNED_SKILL_PER_PAGE];
    };
    void Init() {
        ::ZeroMemory(m_nSkillINDEX, sizeof(short) * MAX_LEARNED_SKILL_CNT);

        m_nSkillINDEX[0] = 11;
        m_nSkillINDEX[1] = 12;
        m_nSkillINDEX[2] = 16;
        m_nSkillINDEX[3] = 19;
        m_nSkillINDEX[4] = 20;
        m_nSkillINDEX[5] = 21;
    }
};

#define QUEST_PER_PLAYER 10 // ���δ� �ִ� ��� ���� ����Ʈ��...

#define QUEST_EPISODE_VAR_CNT 5 // ���Ǽҵ� ���� ���� ����
#define QUEST_JOB_VAR_CNT 3 // ������ ���� ���� ����
#define QUEST_PLANET_VAR_CNT 7 // �༺��...
#define QUEST_UNION_VAR_CNT 10 // ���պ�...

#ifdef __APPLY_EXTAND_QUEST_VAR
    #define QUEST_ETC_VAR_CNT 53
    #define QUEST_SWITCH_CNT (512 + 512) // ��ü ����Ʈ ���� ����ġ��..
#else
    #define QUEST_SWITCH_CNT (512) // ��ü ����Ʈ ���� ����ġ��..
#endif
#define QUEST_SWITCH_GROUP_CNT (QUEST_SWITCH_CNT / sizeof(DWORD))

struct tagQuestData {
private:
    enum { BIT_SHIFT = 3, WHICH_BIT = 0x07 };
    BYTE GetBit(int iIndex) {
        return (m_btSWITCHES[iIndex >> BIT_SHIFT] & g_btSwitchBitMask[iIndex & WHICH_BIT]) ? 1 : 0;
    }
    void SetBit(int iIndex) {
        m_btSWITCHES[iIndex >> BIT_SHIFT] |= g_btSwitchBitMask[iIndex & WHICH_BIT];
    }
    void ClearBit(int iIndex) {
        m_btSWITCHES[iIndex >> BIT_SHIFT] &= ~(g_btSwitchBitMask[iIndex & WHICH_BIT]);
    }
    BYTE FlipBit(int iIndex) {
        if (this->GetBit(iIndex))
            this->ClearBit(iIndex);
        else
            this->SetBit(iIndex);

        return this->GetBit(iIndex);
    }

public:
    short m_nEpisodeVAR[QUEST_EPISODE_VAR_CNT];
    short m_nJobVAR[QUEST_JOB_VAR_CNT];
    short m_nPlanetVAR[QUEST_PLANET_VAR_CNT];
    short m_nUnionVAR[QUEST_UNION_VAR_CNT];
    CQUEST m_QUEST[QUEST_PER_PLAYER];
    union {
        DWORD m_dwSWITCHES[QUEST_SWITCH_CNT / 32];
        BYTE m_btSWITCHES[QUEST_SWITCH_CNT / 8];
    };

#ifdef __APPLY_EXTAND_QUEST_VAR
    short m_nEtcVAR[QUEST_ETC_VAR_CNT]; // 2 * 50
#endif

    void Init() {
        ::ZeroMemory(m_nEpisodeVAR, sizeof(short) * QUEST_EPISODE_VAR_CNT);
        ::ZeroMemory(m_nJobVAR, sizeof(short) * QUEST_JOB_VAR_CNT);
        ::ZeroMemory(m_nPlanetVAR, sizeof(short) * QUEST_PLANET_VAR_CNT);
        ::ZeroMemory(m_nUnionVAR, sizeof(short) * QUEST_UNION_VAR_CNT);
#ifdef __APPLY_EXTAND_QUEST_VAR
        ::ZeroMemory(m_nEtcVAR, sizeof(short) * QUEST_ETC_VAR_CNT);
#endif

        this->ClearAllSwitchs();

        for (short nI = 0; nI < QUEST_PER_PLAYER; nI++) {
            m_QUEST[nI].Init();
        }
    }

#ifdef __GAMESERVER
    void CheckExpiredTIME() {
        for (short nI = 0; nI < QUEST_PER_PLAYER; nI++) {
            if (this->m_QUEST[nI].GetID()) {
                this->m_QUEST[nI].CheckExpiredTIME();
            }
        }
    }
#endif

    //-------------------------------------------------------------------------------------------------
    void ClearAllSwitchs() { ::ZeroMemory(m_dwSWITCHES, sizeof(m_dwSWITCHES)); }
    void Set_SWITCH(int iSwitchNO, int iValue) {
        if (iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT)
            return;

        if (iValue)
            this->SetBit(iSwitchNO);
        else
            this->ClearBit(iSwitchNO);
    }
    int Get_SWITCH(int iSwitchNO) {
        if (iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT)
            return -1;

        return this->GetBit(iSwitchNO);
    }
    int Flip_SWITCH(int iSwitchNO) {
        if (iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT)
            return -1;

        return this->FlipBit(iSwitchNO);
    }
};

#define BANKSLOT_DEFAULT 90
#define BANKSLOT_ADDON 30
#define BANKSLOT_PLATINUM 40

#define BANKSLOT_TOTAL (BANKSLOT_DEFAULT + BANKSLOT_ADDON + BANKSLOT_PLATINUM)

#define BANKSLOT_PLATINUM_0 (BANKSLOT_DEFAULT + BANKSLOT_ADDON)

struct tagBankData {
    tagITEM m_ItemLIST[BANKSLOT_TOTAL];
    __int64 m_i64ZULY;

    void Init() {
        ::ZeroMemory(m_ItemLIST, sizeof(tagITEM) * BANKSLOT_TOTAL);
        m_i64ZULY = 0;
    }

#ifndef __SERVER
    short Get_EmptySlot(short nStartSlotNO);
    short Add_ITEM(tagITEM& sITEM);
#else
    short Get_EmptySlot(short nStartSlotNO, short nEndSlotNO);
    short Add_ITEM(tagITEM& sITEM, short nStartSlotNO, short nEndSlotNO);
#endif

    short Add_ITEM(short nSlotNO, tagITEM& sITEM);
    short Sub_ITEM(short nSlotNO, tagITEM& sITEM);
    void Set_ITEM(short nSlotNO, tagITEM& sITEM) { m_ItemLIST[nSlotNO] = sITEM; }
};

#define MAX_WISH_ITEMS 30

#ifndef __SERVER
struct tagWishLIST {
    tagITEM m_WishITEM[MAX_WISH_ITEMS];

    void Init() { ::ZeroMemory(m_WishITEM, sizeof(m_WishITEM)); }
    bool Set(BYTE btSlotNO, tagITEM& sITEM) {
        if (btSlotNO >= MAX_WISH_ITEMS)
            return false;

        this->m_WishITEM[btSlotNO] = sITEM;
        return true;
    }
};
#else
struct tagWishLIST {
    tagBaseITEM m_WishITEM[MAX_WISH_ITEMS];

    void Init() { ::ZeroMemory(m_WishITEM, sizeof(m_WishITEM)); }
    bool Set(BYTE btSlotNO, tagBaseITEM& sITEM) {
        if (btSlotNO >= MAX_WISH_ITEMS)
            return false;

        this->m_WishITEM[btSlotNO] = sITEM;
        return true;
    }
};
#endif

//-------------------------------------------------------------------------------------------------
#pragma pack(pop)

struct tagBattleAbility {
    short m_nMaxHP;
    short m_nMaxMP;
    short m_nATT;
    short m_nDEF;
    short m_nMAG;
    short m_nHIT;
    short m_nRES;
    short m_nAVOID;
    short m_nSPD;
    short m_nMaxWEIGHT;
    short m_nWEIGHT;

    int m_iDefDurabity;
    int m_iDefGrade;

    int m_iCritical;
    float m_fRateUseMP;

#ifdef __KCHS_BATTLECART__
    short m_nPatMaxHP;
    short m_nImmunity;
#endif
};

class CUserDATA {
protected:
    inline int GetPassiveSkillValue(short nAbilityTYPE) { return this->m_iAddValue[nAbilityTYPE]; }
    inline void AddPassiveSkillValue(short nAbilityTYPE, short nValue) {
        this->m_iAddValue[nAbilityTYPE] += nValue;
    }
    inline short GetPassiveSkillRate(short nAbilityTYPE) {
        return this->m_nPassiveRate[nAbilityTYPE];
    }
    inline void AddPassiveSkillRate(short nAbilityTYPE, short nRate) {
        this->m_nPassiveRate[nAbilityTYPE] += nRate;
    }

    void Cal_AddAbility(tagITEM& sITEM, short nItemTYPE);
#ifdef __GAMESERVER
    void Cal_AddPatAbility(tagITEM& sITEM, short nItemTYPE);
#endif
    void Cal_AddAbility();

    void Cal_RecoverHP();
    void Cal_RecoverMP();

    int Cal_MaxHP();
    int Cal_MaxMP();

    int Cal_HIT();
    int Cal_DEFENCE();

    int Cal_RESIST();
    int Cal_MaxWEIGHT();
    int Cal_AvoidRATE();
    int Cal_CRITICAL();
#ifdef __APPLY_2ND_JOB
    int Cal_IMMUNITY();
#endif

    short GetPassiveSkillAttackPower(int iCurAP, short nRightWeaponItemNo);

protected:
    short GetPassiveSkillAttackSpeed(float fCurSpeed, short nRightWeaponItemNo);

#ifndef __SERVER
    short GetPassiveSkillAttackSpeed(short nRightWeaponItemNo);
#endif

protected:
#ifdef __GAMESERVER
    float Cal_RunSPEED();
#endif

    void Cal_BattleAbility();
    int Cal_ATTACK();

public:
    short GetCur_HP() { return this->m_GrowAbility.m_nHP; }
    short GetCur_MP() { return this->m_GrowAbility.m_nMP; }
    virtual short GetCur_WEIGHT() { return this->m_Battle.m_nWEIGHT; }

    int GetCur_LEVEL() { return this->m_GrowAbility.m_nLevel; }

    int GetDef_ATK() { return this->m_Battle.m_nATT; }
    int GetDef_DEF() { return this->m_Battle.m_nDEF; }
    int GetDef_RES() { return this->m_Battle.m_nRES; }
    int GetDef_HIT() { return this->m_Battle.m_nHIT; }
    int GetDef_AVOID() { return this->m_Battle.m_nAVOID; }
    int GetDef_CRITICAL() { return this->m_Battle.m_iCritical; }

    void SetDef_IMMUNITY(int iImmunity);

    int GetCur_BIRTH() { return this->m_BasicINFO.m_cBirthStone; }
    int GetCur_RANK() { return this->m_BasicINFO.m_cRank; }
    int GetCur_UNION() { return this->m_BasicINFO.m_cUnion; }
    int GetCur_FAME() { return this->m_BasicINFO.m_cFame; }
    int GetCur_JOB() { return this->m_BasicINFO.m_nClass; }

    int64_t GetCur_EXP() { return this->m_GrowAbility.m_lEXP; }
    int GetCur_BonusPOINT() { return this->m_GrowAbility.m_nBonusPoint; }
    int GetCur_SkillPOINT() { return this->m_GrowAbility.m_nSkillPoint; }
    BYTE GetCur_HeadSIZE() { return this->m_GrowAbility.m_btHeadSIZE; }
    BYTE GetCur_BodySIZE() { return this->m_GrowAbility.m_btBodySIZE; }

#ifndef __SERVER
    short GetCur_PatHP();
    void SetCur_PatHP(short nPatHP);
    DWORD GetCur_PatCoolTIME();
    void SetCur_PatCoolTIME(DWORD dwCoolTIME);
#endif

    /// �⺻ �ɷ�ġ...
    int GetDef_STR() { return this->m_BasicAbility.m_nSTR; }
    int GetDef_DEX() { return this->m_BasicAbility.m_nDEX; }
    int GetDef_INT() { return this->m_BasicAbility.m_nINT; }
    int GetDef_CON() { return this->m_BasicAbility.m_nCON; }
    int GetDef_CHARM() { return this->m_BasicAbility.m_nCHARM; }
    int GetDef_SENSE() { return this->m_BasicAbility.m_nSENSE; }

    void SetDef_STR(short nValue) { this->m_BasicAbility.m_nSTR = nValue; }
    void SetDef_DEX(short nValue) { this->m_BasicAbility.m_nDEX = nValue; }
    void SetDef_INT(short nValue) { this->m_BasicAbility.m_nINT = nValue; }
    void SetDef_CON(short nValue) { this->m_BasicAbility.m_nCON = nValue; }
    void SetDef_CHARM(short nValue) { this->m_BasicAbility.m_nCHARM = nValue; }
    void SetDef_SENSE(short nValue) { this->m_BasicAbility.m_nSENSE = nValue; }

    int GetCur_STR() {
        return (GetDef_STR() + m_iAddValue[AT_STR] + m_PassiveAbilityFromRate[AT_STR - AT_STR]);
    } // �ٷ�
    int GetCur_DEX() {
        return (GetDef_DEX() + m_iAddValue[AT_DEX] + m_PassiveAbilityFromRate[AT_DEX - AT_STR]);
    } // ��ø
    int GetCur_INT() {
        return (GetDef_INT() + m_iAddValue[AT_INT] + m_PassiveAbilityFromRate[AT_INT - AT_STR]);
    } // ����
    int GetCur_CON() {
        return (GetDef_CON() + m_iAddValue[AT_CON] + m_PassiveAbilityFromRate[AT_CON - AT_STR]);
    } // ����
    int GetCur_CHARM() {
        return (
            GetDef_CHARM() + m_iAddValue[AT_CHARM] + m_PassiveAbilityFromRate[AT_CHARM - AT_STR]);
    } // �ŷ�
    int GetCur_SENSE() {
        return (
            GetDef_SENSE() + m_iAddValue[AT_SENSE] + m_PassiveAbilityFromRate[AT_SENSE - AT_STR]);
    } // ����

    int GetCur_SaveMP() // MP ���� ����
    {
        return (GetPassiveSkillValue(AT_PSV_SAVE_MP) + m_iAddValue[AT_SAVE_MP]
            + (short)(m_iAddValue[AT_SAVE_MP] * GetPassiveSkillRate(AT_PSV_SAVE_MP) / 100.f));
    }

    int GetCur_DropRATE() { return m_iDropRATE; } // ��� Ȯ��
    void Cal_DropRATE() // ��� Ȯ��
    {
        m_iDropRATE = GetPassiveSkillValue(AT_PSV_DROP_RATE) + m_iAddValue[AT_DROP_RATE]
            + (short)(m_iAddValue[AT_DROP_RATE] * GetPassiveSkillRate(AT_PSV_DROP_RATE) / 100.f);
    }

    int GetMax_SummonCNT() { return (50 + GetPassiveSkillValue(AT_PSV_SUMMON_MOB_CNT)); }

#ifdef __SERVER
    int GetCur_UnionPOINT(char btUnionIDX) {
        return btUnionIDX >= 1 ? this->m_GrowAbility.m_nUnionPOINT[btUnionIDX - 1] : 0;
    }
    virtual void SetCur_UnionPOINT(BYTE btUnionIDX, short nValue) = 0;
    void AddCur_UnionPOINT(BYTE btUnionIDX, short nValue) {
        if (btUnionIDX >= 1) {
            short nResult = this->m_GrowAbility.m_nUnionPOINT[btUnionIDX - 1] + nValue;
            this->SetCur_UnionPOINT(btUnionIDX, nResult);
        }
    }
    void SubCur_UnionPOINT(BYTE btUnionIDX, short nValue) {
        if (btUnionIDX >= 1) {
            short nResult = this->m_GrowAbility.m_nUnionPOINT[btUnionIDX - 1] - nValue;
            this->SetCur_UnionPOINT(btUnionIDX, nResult);
        }
    }
#else
    int GetCur_UnionPOINT(WORD wAbilityTYPE) {
        return this->m_GrowAbility.m_nUnionPOINT[wAbilityTYPE - AT_UNION_POINT1];
    }
    void SetCur_UnionPOINT(WORD wAbilityTYPE, short nValue) {
        this->m_GrowAbility.m_nUnionPOINT[wAbilityTYPE - AT_UNION_POINT1] = nValue;
    }
    void AddCur_UnionPOINT(WORD wAbilityTYPE, short nValue) {
        this->m_GrowAbility.m_nUnionPOINT[wAbilityTYPE - AT_UNION_POINT1] += nValue;
    }
#endif

    int GetCur_FameB() { return this->m_GrowAbility.m_nFameB; }
    int GetCur_FameG() { return this->m_GrowAbility.m_nFameG; }

#ifndef __SERVER
    int GetCur_GuildNO() { return this->m_GrowAbility.m_iGuildNO; }
    int GetCur_GuildPOS() { return this->m_GrowAbility.m_nGuildCNTRB; }
    int GetCur_GuildCONTRIBUTION() { return this->m_GrowAbility.m_btGuildPOS; }
#endif

#ifdef __SERVER
    virtual void AddClanSCORE(int /*iValue*/) = 0;
#endif

    short GetDef_MaxHP() { return this->m_Battle.m_nMaxHP; }
    short GetDef_MaxMP() { return this->m_Battle.m_nMaxMP; }

    short GetCur_MaxWEIGHT() { return this->m_Battle.m_nMaxWEIGHT; }
    int GetCur_MAG() { return this->m_Battle.m_nMAG; } // ������

    //	int	  GetTot_DEF_QUALITY()		{	return	this->m_Battle.m_iDefQuality;		}
    int GetTot_DEF_DURABITY() { return this->m_Battle.m_iDefDurabity; }
    int GetTot_DEF_GRADE() { return this->m_Battle.m_iDefGrade; }

    float GetCur_RateUseMP() { return this->m_Battle.m_fRateUseMP; }

    void SetCur_MONEY(__int64 iMoney) { this->m_Inventory.m_i64Money = iMoney; }
    __int64 GetCur_MONEY() { return this->m_Inventory.m_i64Money; } // ��
    void Add_CurMONEY(int iMoney) { this->m_Inventory.m_i64Money += iMoney; }
    void Sub_CurMONEY(int iMoney) { this->m_Inventory.m_i64Money -= iMoney; }
    void SetCur_HP(short nValue);
    void SetCur_MP(short nValue) { this->m_GrowAbility.m_nMP = nValue; }

    int AddCur_HP(short nValue) {
        m_GrowAbility.m_nHP += nValue;
        if (m_GrowAbility.m_nHP > GetCur_MaxHP())
            m_GrowAbility.m_nHP = GetCur_MaxHP();
        return m_GrowAbility.m_nHP;
    }
    int AddCur_MP(short nValue) {
        m_GrowAbility.m_nMP += nValue;
        if (m_GrowAbility.m_nMP > GetCur_MaxMP())
            m_GrowAbility.m_nMP = GetCur_MaxMP();
        return m_GrowAbility.m_nMP;
    }
    int SubCur_HP(short nValue) {
        m_GrowAbility.m_nHP -= nValue;
        if (m_GrowAbility.m_nHP < 0)
            m_GrowAbility.m_nHP = 0;
        return m_GrowAbility.m_nHP;
    }
    int SubCur_MP(short nValue) {
        m_GrowAbility.m_nMP -= nValue;
        if (m_GrowAbility.m_nMP < 0)
            m_GrowAbility.m_nMP = 0;
        return m_GrowAbility.m_nMP;
    }

#ifndef __SERVER
    int AddCur_STAMINA(short nValue) {
        m_GrowAbility.m_nSTAMINA += nValue;
        if (m_GrowAbility.m_nSTAMINA > GetCur_MaxSTAMINA())
            m_GrowAbility.m_nSTAMINA = GetCur_MaxSTAMINA();
        return m_GrowAbility.m_nSTAMINA;
    }
    int SubCur_STAMINA(short nValue) {
        m_GrowAbility.m_nSTAMINA -= nValue;
        if (m_GrowAbility.m_nSTAMINA < 0)
            m_GrowAbility.m_nSTAMINA = 0;
        return m_GrowAbility.m_nSTAMINA;
    }
#else
    void AddCur_STAMINA(short nValue) {
        this->m_GrowAbility.m_nSTAMINA += nValue;
        if (this->m_GrowAbility.m_nSTAMINA > MAX_STAMINA)
            this->m_GrowAbility.m_nSTAMINA = MAX_STAMINA;
        else if (this->m_GrowAbility.m_nSTAMINA < 0)
            this->m_GrowAbility.m_nSTAMINA = 0;
    }
#endif

    void SetCur_FAME(char cValue) { this->m_BasicINFO.m_cFame = cValue; }
    void SetCur_RANK(char cValue) { this->m_BasicINFO.m_cRank = cValue; }

#ifndef __SERVER
    void SetCur_UNION(char cValue) { this->m_BasicINFO.m_cUnion = cValue; }
#else
    virtual void SetCur_UNION(char cValue) = 0;
#endif

#ifdef __SERVER
    char GetCur_FACE() { return this->m_BasicINFO.m_cFaceIDX; }
    void SetCur_FACE(char cValue) { this->m_BasicINFO.m_cFaceIDX = cValue; }

    char GetCur_HAIR() { return this->m_BasicINFO.m_cHairIDX; }
    void SetCur_HAIR(char cValue) { this->m_BasicINFO.m_cHairIDX = cValue; }
#endif

    virtual void SetCur_JOB(short nValue) { this->m_BasicINFO.m_nClass = nValue; }

    void SetCur_LEVEL(WORD wValue) {
        this->m_GrowAbility.m_nLevel = (wValue > Rose::GameStaticConfig::MAX_LEVEL)
            ? Rose::GameStaticConfig::MAX_LEVEL
            : wValue;
    }
    void SetCur_EXP(int64_t lValue) { this->m_GrowAbility.m_lEXP = lValue; }
    void SetCur_BonusPOINT(short nValue) { this->m_GrowAbility.m_nBonusPoint = nValue; }
    void SetCur_SkillPOINT(short nValue) { this->m_GrowAbility.m_nSkillPoint = nValue; }

    tagBasicINFO m_BasicINFO;
    tagBasicAbility m_BasicAbility;
    tagGrowAbility m_GrowAbility;
    tagSkillAbility m_Skills;

    tagQuestData m_Quests;
    CInventory m_Inventory;
    CHotICONS m_HotICONS;
    tagBankData m_Bank;
    tagWishLIST m_WishLIST;

    tagBattleAbility m_Battle;

    int m_iAddValue[AT_MAX];
    short m_nPassiveRate[AT_MAX];

    BYTE m_btRecoverHP;
    BYTE m_btRecoverMP;

    int m_iDropRATE;

    short m_PassiveAbilityFromValue[BA_MAX];
    short m_PassiveAbilityFromRate[BA_MAX];
    short m_nPassiveAttackSpeed;

#ifndef __SERVER
    int m_iAppliedPenaltyEXP;
#endif

#ifdef __SERVER
    short m_nPatHP_Mode;
#endif

    tagITEM Get_EquipITEM(WORD wEquipIDX) { return m_Inventory.m_ItemEQUIP[wEquipIDX]; }
    tagITEM* Get_EquipItemPTR(WORD wEquipIDX) { return &m_Inventory.m_ItemEQUIP[wEquipIDX]; }

    tagITEM Get_InventoryITEM(WORD wTotalListIndex) {
        return m_Inventory.m_ItemLIST[wTotalListIndex];
    }
    tagITEM Get_InventoryITEM(t_InvTYPE InvTYPE, WORD wPageIndex) {
        return m_Inventory.m_ItemPAGE[InvTYPE][wPageIndex];
    }

#ifdef __SERVER
    short Add_CatchITEM(short nListRealNO,
        tagITEM& sITEM); //		{	return m_Inventory.AppendITEM( nListRealNO, sITEM,
                         // m_Battle.m_nWEIGHT
                         //);	}
    short Add_ITEM(tagITEM& sITEM) { return m_Inventory.AppendITEM(sITEM, m_Battle.m_nWEIGHT); }
    short Add_ITEM(short nListRealNO,
        tagITEM& sITEM); //				{	return m_Inventory.AppendITEM( nListRealNO, sITEM,
                         // m_Battle.m_nWEIGHT);		}
    void Sub_ITEM(short nListRealNO,
        tagITEM& sITEM); //				{	m_Inventory.SubtractITEM( nListRealNO, sITEM,
                         // m_Battle.m_nWEIGHT);
                         //}
    void Set_ITEM(short nListRealNO, tagITEM& sITEM);
    void ClearITEM(WORD wListRealNO) {
        ///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
        m_Inventory.DeleteITEM(wListRealNO);
    }
#else
    virtual short Add_CatchITEM(short nListRealNO,
        tagITEM& sITEM); //		{	return m_Inventory.AppendITEM( nListRealNO, sITEM,
                         // m_Battle.m_nWEIGHT
                         //);	}
    virtual short Add_ITEM(tagITEM& sITEM) {
        return m_Inventory.AppendITEM(sITEM, m_Battle.m_nWEIGHT);
    }
    virtual short Add_ITEM(short nListRealNO,
        tagITEM& sITEM); //				{	return m_Inventory.AppendITEM( nListRealNO, sITEM,
                         // m_Battle.m_nWEIGHT);		}
    virtual void Sub_ITEM(short nListRealNO,
        tagITEM& sITEM); //				{	m_Inventory.SubtractITEM( nListRealNO, sITEM,
                         // m_Battle.m_nWEIGHT);
                         //}
    virtual void Set_ITEM(short nListRealNO, tagITEM& sITEM);
    virtual void ClearITEM(WORD wListRealNO) {
        ///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
        m_Inventory.DeleteITEM(wListRealNO);
    }
#endif

    /// ���� ��ų�� ���� ������ ���� ������ �Լ�...
#ifdef __SERVER
    virtual void SetCur_SEX(char cValue) = 0;
#endif
    virtual short GetCur_RACE() = 0; /// ����,,,, gsv_SELECT_CHAR.m_btCharRACE �� �����ϸ��
    virtual short GetCur_ATK_SPD() = 0;
    virtual float GetCur_MOVE_SPEED() = 0;
    virtual int GetCur_ATK() = 0;
    virtual int GetCur_DEF() = 0;
    virtual int GetCur_RES() = 0;
    virtual int GetCur_HIT() = 0;
    virtual int GetCur_AVOID() = 0;
    virtual int GetCur_CRITICAL() = 0;

    virtual int GetCur_MaxHP() = 0; /// ��ų�� ���� ������ ���� ������� �������� ������ �Լ�
    virtual int GetCur_MaxMP() = 0; /// ��ų�� ���� ������ ���� ������� �������� ������ �Լ�
#ifndef __SERVER
    virtual int GetCur_MaxSTAMINA() = 0;
#endif
    virtual void UpdateCur_Ability() = 0;
    virtual BYTE GetCur_MOVE_MODE() { return 0; }

#ifdef __SERVER
    virtual int GetCur_FUEL() = 0;
    virtual void SubCur_FUEL(short nValue) = 0;
#endif

    virtual void SetCur_TeamNO(int /*nValue*/) = 0;
    void SetCur_PK_FLAG(short nValue) { this->m_GrowAbility.m_nPKFlag = nValue; }
    short GetCur_PK_FLAG() { return this->m_GrowAbility.m_nPKFlag; }

    short GetCur_STAMINA() { return this->m_GrowAbility.m_nSTAMINA; }
    void SetCur_STAMINA(short nValue) {
        this->m_GrowAbility.m_nSTAMINA = nValue;
        if (this->m_GrowAbility.m_nSTAMINA > MAX_STAMINA)
            this->m_GrowAbility.m_nSTAMINA = MAX_STAMINA;
        else if (this->m_GrowAbility.m_nSTAMINA < 0)
            this->m_GrowAbility.m_nSTAMINA = 0;
    }

    virtual void AddCur_EXP(int /*iValue*/) { /* nop */
    } // ȣ��ɰ�� �������� ���۵�.
    void AddCur_BonusPOINT(short nValue) { this->m_GrowAbility.m_nBonusPoint += nValue; }
    void AddCur_SkillPOINT(short nValue) { this->m_GrowAbility.m_nSkillPoint += nValue; }

    void SubCur_BounsPOINT(short nValue) { this->m_GrowAbility.m_nBonusPoint -= nValue; }
    void SubCur_SkillPOINT(short nValue) { this->m_GrowAbility.m_nSkillPoint -= nValue; }

    bool Set_AbilityValue(WORD nType, int iValue);
    void Add_AbilityValue(WORD nType, int iValue);

#ifndef __SERVER
    virtual void ChangeModelInfo(short nType, int iValue) = 0;
#endif

#ifdef __SERVER
    virtual int GetCur_SummonCNT() = 0;
    virtual int GetCur_AbilityValue(WORD nType) = 0;
#else
    int GetCur_AbilityValue(WORD nType) { return Get_AbilityValue(nType); }
    int Get_AbilityValue(WORD nType);
    void RefreshLookUpTable() { m_Inventory.MakeItemIndexList(); }
    virtual void UpdateModelData() = 0 { *(int*)0 = 10; } /// �� ������ ����..
#endif

    bool Use_ITEM(WORD wUseItemNO);
    bool Set_EquipITEM(short nEquipIDX, tagITEM& sITEM);

#ifdef __GAMESERVER
    bool Check_PatEquipCondition(tagITEM& sITEM, short nEquipIdx);
    bool Check_EquipCondition(tagITEM& sITEM, short nEquipIdx);
#else
    bool Check_PatEquipCondition(tagITEM& sITEM);
    bool Check_EquipCondition(tagITEM& sITEM);
#endif

    bool Check_JobCollection(short nClassStbIDX);

    virtual void SetCur_PartITEM(short nPartIdx, tagITEM& sItem) = 0;
    virtual short GetCur_PartITEM(short nPartIdx) = 0;

public:
    __int64 Get_NeedEXP(int iLevel) {
        return (m_GrowAbility.m_lPenalEXP + CCal::Get_NeedRawEXP(iLevel));
    }

#ifndef __SERVER
    void Set_PenalEXP(BYTE btAddPercent) {
        if (this->GetCur_LEVEL() >= 10) {
            // 10�� �̻��̸� ���Ƽ ����..
            int iNeedEXP = static_cast<int32_t>(CCal::Get_NeedRawEXP(m_GrowAbility.m_nLevel));
            int iPenalEXP = (int)(iNeedEXP * btAddPercent / 100.f);
            m_iAppliedPenaltyEXP = iPenalEXP;

            if (m_GrowAbility.m_lEXP >= iPenalEXP) {
                m_GrowAbility.m_lEXP -= iPenalEXP;
            } else {
                iPenalEXP -= static_cast<int32_t>(m_GrowAbility.m_lEXP);
                m_GrowAbility.m_lPenalEXP += iPenalEXP;
                m_GrowAbility.m_lEXP = 0;

                if (m_GrowAbility.m_lPenalEXP > DIV02(iNeedEXP)) {
                    m_iAppliedPenaltyEXP -=
                        (DIV02(iNeedEXP) - static_cast<int32_t>(m_GrowAbility.m_lPenalEXP));
                    m_GrowAbility.m_lPenalEXP = DIV02(iNeedEXP);
                }
            }
        }
    }

    /// ��Ȱ�� ������ų ����ġ
    void Cancel_PenalEXP(BYTE btPercent) {
        if (this->GetCur_LEVEL() >= 10 && m_iAppliedPenaltyEXP > 0) {
            // 10�� �̻��̸� �����ߴ� ���Ƽ ����...
            m_iAppliedPenaltyEXP = (int)(m_iAppliedPenaltyEXP * btPercent / 100.f);
            if (m_iAppliedPenaltyEXP) {
                m_GrowAbility.m_lEXP += m_iAppliedPenaltyEXP;
            }
        }
        m_iAppliedPenaltyEXP = 0;
    }
#endif

    short Get_NeedPoint2AbilityUP(short BasicAbilityType) {
        _ASSERT(BasicAbilityType >= 0 && BasicAbilityType < 6);

        return (short)(m_BasicAbility.m_nBasicA[BasicAbilityType] * 0.2);
    }

    short Get_NeedPoint2SkillUP(short nSkillSLOT);

    //  -------------------------------------------------------------------------------------------
    /// ��ų ���� ���� �Լ�...
    virtual int GetCur_R_WEAPON() = 0;
    virtual int GetCur_L_WEAPON() = 0;
#ifdef __SERVER
    virtual int GetCur_PET_BODY() = 0;
#endif
    virtual DWORD GetCur_IngStatusFLAG() = 0;

    /// ��ų�� �� ���� ã��
    short Skill_FindEmptySlot(short nSkillIDX);

    /// ������ ��ų�� ������ ��´�.
    short Skill_FindLearnedLevel(short nSkillIDX);

    /// ������ ��ų�� ������ ã�´�.
    short Skill_FindLearnedSlot(short nSkillIDX);

    /// �ɷ�ġ ��� �Լ��� ������ ��ų���� ����ϴ� �ɷ�ġ�� �Ҽ��� �Ұ� ������
    /// �ӵ��� ���̱� ���ؼ� ��ų�� ����ϴ� �ɷ�ġ�� ������ �Լ�...
    int Skill_GetAbilityValue(short nAbilityType);

    /// ��ų���� �Ҹ�Ǵ� ��ġ ����...
    int Skill_ToUseAbilityVALUE(short nSKillIDX, short nPropertyIDX);
    bool Skill_UseAbilityValue(short nSkillIDX);

    bool Skill_CheckJOB(short nSkillIDX); /// ����, ������� ���� üũ
    bool Skill_CheckLearnedSKILL(short nSkillIDX); /// ����, ������� �̹� ������ ��ų üũ
    bool Skill_CheckNeedABILITY(short nSkillIDX); /// ����, ������� ����� �ɷ�ġ üũ..

    /// ��ų ó�� ���� ���� üũ �Լ�
    BYTE Skill_LearnCondition(short nSkillIDX);

    /// ��ų ������ ���� üũ �Լ�
    BYTE Skill_LevelUpCondition(short nCurLevelSkillIDX, short nNextLevelSkillIDX);

    /// ��ų ��� ���� üũ �Լ�
    bool Skill_ActionCondition(short nSkillIDX);

    /// ��ų ���� :: �����̳� �������� ȣ���ؾ� �нú� ��ų �����..
    BYTE Skill_LEARN(short nSkillSLOT, short nSkillIDX, bool bSubPOINT = true);

    /// ������ ��ų�� �����Ѵ�.
    bool Skill_DELETE(short nSkillSLOT, short nSkillIDX);

    /// �нú� ��ų ���� �Լ�.
    void InitPassiveSkill(); /// ������ ���� ��ų ����Ÿ ������ �ѹ� ȣ���ؾ���...
    short GetPassiveSkillAttackSpeed() { return m_nPassiveAttackSpeed + m_iAddValue[AT_ATK_SPD]; }

    short GetBuySkillVALUE() { return GetPassiveSkillValue(AT_PSV_BUYING); }
    short GetSellSkillVALUE() { return GetPassiveSkillValue(AT_PSV_SELLING); }

    //  -------------------------------------------------------------------------------------------
    /// ����Ʈ ���� ���� �Լ�...
#ifdef __SERVER
    virtual bool Quest_IsPartyLEADER() = 0; /// ��Ƽ ¯�̳� ?
    virtual int Quest_GetPartyLEVEL() = 0; /// ���� ��Ƽ ����
    virtual int Quest_GetZoneNO() = 0; /// ���� �� ��ȣ
    virtual int Quest_DistanceFrom(int iX, int iY, int iZ) = 0; /// ������ġ���� �Ÿ�
    virtual int Quest_PartyMemberCNT() = 0; /// ��Ƽ �ο���
#else
    virtual bool Quest_IsPartyLEADER() = 0 {
        *(int*)0 = 10;
        return false;
    } /// ��Ƽ ¯�̳� ?
    virtual int Quest_GetPartyLEVEL() = 0 {
        *(int*)0 = 10;
        return -1;
    } /// ���� ��Ƽ ����
    virtual int Quest_GetZoneNO() = 0 {
        *(int*)0 = 10;
        return 0;
    } /// ���� �� ��ȣ
    virtual int Quest_DistanceFrom(int iX, int iY, int iZ) = 0 {
        *(int*)0 = 10;
        return 999999999;
    } /// ������ġ���� �Ÿ�
    virtual int Quest_PartyMemberCNT() = 0 {
        *(int*)0 = 10;
        return 0;
    } /// ��Ƽ �ο���
#endif

    BYTE Quest_GetRegistered(int iQuestIDX);
    short Quest_Append(int iQuestIDX);
    bool Quest_Append(BYTE btSlot, int iQuestIDX);
    bool Quest_Delete(int iQuestIDX);
    bool Quest_Delete(BYTE btSLOT, int iQuestID);
    tagITEM* Quest_FindEquipITEM(WORD wEquipIDX) {
        return (wEquipIDX < MAX_EQUIP_IDX) ? &m_Inventory.m_ItemEQUIP[wEquipIDX] : NULL;
    }
    tagITEM* Quest_FindITEM(tagITEM& sFindITEM) {
        short nFindSLOT = m_Inventory.FindITEM(sFindITEM);
        return (nFindSLOT >= 0) ? &m_Inventory.m_ItemLIST[nFindSLOT] : NULL;
    }
    bool Quest_SubITEM(tagITEM& sSubITEM);

    short Quest_GetEpisodeVAR(WORD wVarIDX) {
        return (wVarIDX < QUEST_EPISODE_VAR_CNT) ? m_Quests.m_nEpisodeVAR[wVarIDX] : 0;
    }
    short Quest_GetJobVAR(WORD wVarIDX) {
        return (wVarIDX < QUEST_JOB_VAR_CNT) ? m_Quests.m_nJobVAR[wVarIDX] : 0;
    }
    short Quest_GetUnionVAR(WORD wVarIDX) {
        return (wVarIDX < QUEST_UNION_VAR_CNT) ? m_Quests.m_nUnionVAR[wVarIDX] : 0;
    }
    short Quest_GetPlanetVAR(WORD wVarIDX) {
        if (wVarIDX < QUEST_PLANET_VAR_CNT)
            return m_Quests.m_nPlanetVAR[wVarIDX];
#ifdef __APPLY_EXTAND_QUEST_VAR
        else if (wVarIDX < QUEST_PLANET_VAR_CNT + QUEST_ETC_VAR_CNT) {
            return m_Quests.m_nEtcVAR[wVarIDX - QUEST_PLANET_VAR_CNT];
        }
#endif
        return 0;
    }

    void Quest_SetEpisodeVAR(WORD wI, short nV) {
        if (wI < QUEST_EPISODE_VAR_CNT)
            m_Quests.m_nEpisodeVAR[wI] = nV;
    }
    void Quest_SetJobVAR(WORD wI, short nV) {
        if (wI < QUEST_JOB_VAR_CNT)
            m_Quests.m_nJobVAR[wI] = nV;
    }
    void Quest_SetUnionVAR(WORD wI, short nV) {
        if (wI < QUEST_UNION_VAR_CNT)
            m_Quests.m_nUnionVAR[wI] = nV;
    }
    void Quest_SetPlanetVAR(WORD wI, short nV) {
        if (wI < QUEST_PLANET_VAR_CNT)
            m_Quests.m_nPlanetVAR[wI] = nV;
#ifdef __APPLY_EXTAND_QUEST_VAR
        else if (wI < QUEST_PLANET_VAR_CNT + QUEST_ETC_VAR_CNT) {
            m_Quests.m_nEtcVAR[wI - QUEST_PLANET_VAR_CNT] = nV;
        }
#endif
    }

#ifdef __SERVER
    virtual bool Reward_WARP(int iZoneNO, tPOINTF& PosGOTO) = 0;
#endif

    virtual bool Quest_CHANGE_SPEED() { return true; }

    virtual bool Add_MoneyNSend(int iAmount, WORD wType) { return true; }
    virtual bool Add_ExpNSend(int iExp) { return true; }
    virtual bool Add_ItemNSend(tagITEM& sITEM) { return true; }
    virtual bool Add_SkillNSend(short nSkillIDX) { return true; }
    virtual bool Sub_SkillNSend(short nSkillIDX) { return true; }
    virtual bool Set_AbilityValueNSend(WORD nType, int iValue) { return true; }
    virtual void Add_AbilityValueNSend(WORD wType, int iValue) { /* nop */
    }

    bool Reward_InitSTATUS(void);
    bool Reward_InitSKILL(void);
    bool Reward_ITEM(tagITEM& sITEM, BYTE btRewardToParty, BYTE btQuestSLOT);
    bool Reward_ABILITY(bool bSetOrAdj, int iType, int iValue, BYTE btRewardToParty);
    bool Reward_CalEXP(BYTE btEquation, int iBaseValue, BYTE btRewardToParty);
    bool Reward_CalMONEY(BYTE btEquation, int iBaseValue, BYTE btRewardToParty, short nDupCNT);
    bool Reward_CalITEM(BYTE btEquation,
        int iBaseValue,
        BYTE btRewardToParty,
        int iItemSN,
        short nItemOP,
        BYTE btQuestSLOT);

#ifdef __GAMESERVER
    virtual void Cal_AruaATTACK() = 0;
    virtual void Cal_AruaHIT() = 0;
    virtual void Cal_AruaAVOID() = 0;
    virtual void Cal_AruaCRITICAL() = 0;
    virtual void Cal_AruaMaxHP() = 0;
    virtual void Cal_AruaMaxMP() = 0;
    virtual void Cal_AruaRunSPD() = 0;

    virtual short Get_WeightRATE() = 0;

    void Apply_2ndJob_Ability(void);
#endif

#ifndef __SERVER
public:
    DWORD m_dwCoolItemStartTime[MAX_USEITEM_COOLTIME_TYPE];
    DWORD m_dwCoolItemEndTime[MAX_USEITEM_COOLTIME_TYPE];
#endif
};


#pragma warning(default : 4201)
