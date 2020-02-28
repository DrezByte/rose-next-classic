#include "stdAFX.h"

#ifdef __SERVER
    #include "NET_Prototype.h"
#else
    #include "../Network/NET_Prototype.h"
    #include "../CObjUSER.h"
#endif

#include "Calculation.h"

using namespace Rose;

#ifdef __SERVER
    #include "ZoneLIST.H"
#else

short
Get_WorldDROP_M() {
    return 100;
}
short
Get_WorldDROP() {
    return 300;
}
short
Get_WorldEXP() {
    return 300;
}
short
Get_WorldREWARD() {
    return 100;
}
#endif

//-------------------------------------------------------------------------------------------------
__int64
CCal::Get_NeedRawEXP(int iLevel) {
    // �ʿ� ����ġ
    if (iLevel > GameConfig::MAX_LEVEL)
        iLevel = GameConfig::MAX_LEVEL;

    // [���� 15������ ���]   �ʿ� ����ġ = { (LV + 3) * (LV + 5 ) * (LV + 10) * 0.7 }
    if (iLevel <= 15)
        return (__int64)((iLevel + 3) * (iLevel + 5) * (iLevel + 10) * 0.7);

    // [���� 50������ ���]   �ʿ� ����ġ = { (LV - 5) * (LV + 2 ) * (LV + 2) * 2.2 }
    if (iLevel <= 50)
        return (__int64)((iLevel - 5) * (iLevel + 2) * (iLevel + 2) * 2.2);

    // [���� 100������ ���]  �ʿ� ����ġ = { (LV - 5) * ( LV +2 ) * (LV -38 ) * 9 }
    if (iLevel <= 100)
        return (__int64)((iLevel - 5) * (iLevel + 2) * (iLevel - 38) * 9);

    // [���� 139������ ���]  �ʿ� ����ġ = { (LV + 27) * (LV +34 ) * (LV + 220) }
    if (iLevel <= 139)
        return (__int64)((iLevel + 27) * (iLevel + 34) * (iLevel + 220));

    // [���� 200������ ���]  �ʿ� ����ġ = { (LV - 15) * (LV +7 ) * (LV - 126) * 41 }
    return (__int64)((iLevel - 15) * (iLevel + 7) * (iLevel - 126) * 41);

    // �ѱ� ����...2005.05.25(����) ~
    if (iLevel <= 60) {
        if (iLevel <= 15) {
            // [���� 15 ������ ���]  �ʿ� ����ġ = { (LV + 3) * (LV + 5 ) * (LV + 10) * 0.7 }
            return (__int64)(((iLevel + 3) * (iLevel + 5) * (iLevel + 10) * 0.7f));
        }

        // [���� 60 ������ ���]  �ʿ� ����ġ = { (LV - 5) * (LV + 2 ) * (LV + 2) * 2.2 }
        return (__int64)(((iLevel - 5) * (iLevel + 2) * (iLevel + 2) * 2.2f));
    }

    if (iLevel <= 113) {
        // [���� 113������ ���]  �ʿ� ����ġ = { (LV - 11) * ( LV ) * (LV + 4) * 2.5 }
        return (__int64)(((iLevel - 11) * (iLevel) * (iLevel + 4) * 2.5f));
    }

    if (iLevel <= 150) {
        // [���� 150������ ���]  �ʿ� ����ġ = { (LV - 31) * (LV - 20 ) * (LV + 4) * 3.8 }
        return (__int64)(((iLevel - 31) * (iLevel - 20) * (iLevel + 4) * 3.8f));
    }

    //	if ( iLevel <= 176 ) {
    if (iLevel <= 189) {
        // [���� 189������ ���]  �ʿ� ����ġ = { (LV - 67) * (LV - 20 ) * (LV - 10) * 6 }
        return (__int64)(((iLevel - 67) * (iLevel - 20) * (iLevel - 10) * 6.f));
    }

    // [���� 200������ ���]  �ʿ� ����ġ = { (LV - 90) * (LV - 120) * (LV - 60) * (LV - 170) * (LV
    // -188)}
    return (
        __int64)((iLevel - 90) * (iLevel - 120) * (iLevel - 60) * (iLevel - 170) * (iLevel - 188));
}

//-------------------------------------------------------------------------------------------------
int
CCal::Get_RewardVALUE(BYTE btEquation, int S_REWARD, CUserDATA* pUSER, short nDupCNT) {
    int iR = 0;

    switch (btEquation) {
        case 0: // ����ġ ���ذ� �켱
            iR = ((S_REWARD + 30) * (pUSER->GetCur_CHARM() + 10) * (::Get_WorldREWARD())
                     * (pUSER->GetCur_FAME() + 20) / (pUSER->GetCur_LEVEL() + 70) / 30000)
                + S_REWARD;
            break;
        case 1: // ����ġ ��������
            iR = S_REWARD * (pUSER->GetCur_LEVEL() + 3)
                * (pUSER->GetCur_LEVEL() + pUSER->GetCur_CHARM() / 2 + 40) * (::Get_WorldREWARD())
                / 10000;
            break;
        case 2: // �� Ƚ��
            iR = S_REWARD * nDupCNT;
            break;
        case 3: // �� ���ذ�
        case 5: // ������ ���ذ�
            iR = ((S_REWARD + 20) * (pUSER->GetCur_CHARM() + 10) * (::Get_WorldREWARD())
                     * (pUSER->GetCur_FAME() + 20) / (pUSER->GetCur_LEVEL() + 70) / 30000)
                + S_REWARD;
            break;
        case 4: // �� ���� ����
            iR = (S_REWARD + 2) * (pUSER->GetCur_LEVEL() + pUSER->GetCur_CHARM() + 40)
                * (pUSER->GetCur_FAME() + 40) * (::Get_WorldREWARD()) / 140000;
            break;
        case 6: // ������ ��������
            iR = ((S_REWARD + 20) * (pUSER->GetCur_LEVEL() + pUSER->GetCur_CHARM())
                     * (pUSER->GetCur_FAME() + 20) * (::Get_WorldREWARD()) / 3000000)
                + S_REWARD;
            break;
    }

    return iR;
}

//-------------------------------------------------------------------------------------------------
bool
CCal::Get_DropITEM(int iLevelDiff,
    CObjMOB* pMobCHAR,
    tagITEM& sITEM,
    int iZoneNO,
    int iDropRate,
    int iCharm) {
    int iDrop_VAR;

    if (iLevelDiff < 0)
        iLevelDiff = 0;
    else if (iLevelDiff >= 10)
        return false;

    if (iLevelDiff < 9)
        iDrop_VAR = (int)((::Get_WorldDROP() + NPC_DROP_ITEM(pMobCHAR->Get_CharNO())
                              - (1 + RANDOM(100)) - ((iLevelDiff + 16) * 3.5f) - 10 + iDropRate)
            * 0.38f); // * ( NPC_DROP_MONEY( pMobCHAR->Get_CharNO() ) + 30 ) / 130;
    else
        iDrop_VAR = (int)((::Get_WorldDROP() + NPC_DROP_ITEM(pMobCHAR->Get_CharNO())
                              - (1 + RANDOM(100)) - ((iLevelDiff + 20) * 5.5f) - 10 + iDropRate)
            * 0.23f); // * ( NPC_DROP_MONEY( pMobCHAR->Get_CharNO() ) + 30 ) / 130;

    if (iDrop_VAR <= 0) {
        // ��� Ȯ�� ���� !!! �����ȵ�.
        return false;
    }

    if (1 + RANDOM(100) <= NPC_DROP_MONEY(pMobCHAR->Get_CharNO())) {
        // ������
        // DROP_MONEY_Q = { (MOP_LV + 20) * (MOP_LV + DROP_ VAR +40) * WORLD_DROP_M / 3200 }
        int iMoney = (pMobCHAR->Get_LEVEL() + 20) * (pMobCHAR->Get_LEVEL() + iDrop_VAR + 40)
            * ::Get_WorldDROP_M() / 3200;
        if (iMoney <= 0)
            return false;

        sITEM.m_cType = ITEM_TYPE_MONEY;
        sITEM.m_uiMoney = iMoney;

        return true;
    }

    int iDropTBL;
    if (NPC_DROP_ITEM(pMobCHAR->Get_CharNO()) - (1 + RANDOM(100)) >= 0) {
        iDropTBL = NPC_DROP_TYPE(pMobCHAR->Get_CharNO());
    } else {
        iDropTBL = iZoneNO;
    }

    int iDropTblIDX = (iDrop_VAR > 30) ? RANDOM(30) : RANDOM(iDrop_VAR);
    int iDropITEM = DROPITEM_ITEMNO(iDropTBL, iDropTblIDX);
    if (iDropITEM <= 1000) {
        if (iDropITEM >= 1 && iDropITEM <= 4) {
            // �ٽ� ���
            iDropTblIDX = 26 + (iDropITEM * 5) + RANDOM(5);
            if (iDropTblIDX >= g_TblDropITEM.m_nColCnt) {
                // ���̺� �÷� ���� �ʰ�...
                return false;
            }
            iDropITEM = DROPITEM_ITEMNO(iDropTBL, iDropTblIDX);
            if (iDropITEM <= 1000) {
                // ���� !
                return false;
            }
        } else {
            // ���� !
            return false;
        }
    }

    ::ZeroMemory(&sITEM, sizeof(sITEM));
    sITEM.m_cType = (BYTE)(iDropITEM / 1000);
    sITEM.m_nItemNo = iDropITEM % 1000;

    int iTEMP;
    if (sITEM.GetTYPE() >= ITEM_TYPE_ETC
        && sITEM.GetTYPE() != ITEM_TYPE_RIDE_PART) { // ITEM_TYPE_RIDE_PART�� �����ؾ� ��
        sITEM.m_uiQuantity =
            1 + ((pMobCHAR->Get_LEVEL() + 10) / 9 + (1 + RANDOM(20)) + iDropRate) / (iDrop_VAR + 4);

        if (sITEM.m_uiQuantity > 10)
            sITEM.m_uiQuantity = 10;
    } else if (sITEM.GetTYPE() >= ITEM_TYPE_USE
        && sITEM.GetTYPE() != ITEM_TYPE_RIDE_PART) { // ITEM_TYPE_RIDE_PART�� �����ؾ� ��
        sITEM.m_uiQuantity = 1;
    } else {
        ;
        short nRareType = ITEM_RARE_TYPE(sITEM.GetTYPE(), sITEM.GetItemNO());
        if (nRareType < 3) {
            nRareType = 0;
        }

        switch (nRareType) {
            case 3: // �븸 ���� ������
                sITEM.m_nGEM_OP = 100 + RANDOM(41);
                break;
            case 1: // ������
                sITEM.m_bHasSocket = 1;
                sITEM.m_bIsAppraisal = 1;
                break;
            case 2: // ���
                if (ITEM_QUALITY(sITEM.GetTYPE(), sITEM.GetItemNO()) + 60 - RANDOM(400) > 0) {
                    sITEM.m_bHasSocket = 1;
                    sITEM.m_bIsAppraisal = 1;
                    break;
                }
            case 0: {
                iTEMP = 1 + RANDOM(100);
                if (sITEM.GetTYPE() != ITEM_TYPE_JEWEL) {
                    // ���� �� stb�� �⺻ ǰ�� ���� ����.
                    int iITEM_OP = (int)(((pMobCHAR->Get_LEVEL() * 0.4f
                                              + (NPC_DROP_ITEM(pMobCHAR->Get_CharNO()) - 35) * 4
                                              + 80 - iTEMP + iCharm)
                                             * 24 / (iTEMP + 13))
                        - 100);
                    if (iITEM_OP > 0) {
                        int iOption = RANDOM(100);
                        if (iOption <= 35) { // 1-6
                            iOption = 1 + RANDOM(7 - 1);
                        } else if (iOption <= 70) { // 7-48
                            iOption = 7 + RANDOM(49 - 7);
                        } else if (iOption <= 85) { // 49-64
                            iOption = 49 + RANDOM(65 - 49);
                        } else if (iOption <= 95) { // 65-72
                            iOption = 65 + RANDOM(73 - 65);
                        } else { // 73-80
                            iOption = 73 + RANDOM(81 - 73);
                        }
                        sITEM.m_nGEM_OP = iOption;
                        sITEM.m_bIsAppraisal = 1;
                    }
                }
                break;
            }
        }
        sITEM.m_bHasSocket = 1;

        // ������ ����
        iTEMP = (int)(ITEM_DURABITY(sITEM.GetTYPE(), sITEM.GetItemNO())
            * (pMobCHAR->Get_LEVEL() * 0.3f + NPC_DROP_ITEM(pMobCHAR->Get_CharNO()) * 2 + 320)
            * 0.5f / (RANDOM(100) + 201));
        if (iTEMP > 100)
            iTEMP = 100;
        sITEM.m_cDurability = iTEMP;

        // ���� ����
        iTEMP = (int)((NPC_DROP_ITEM(pMobCHAR->Get_CharNO()) + 200) * 80 / (31 + RANDOM(100)));
        if (iTEMP > MAX_ITEM_LIFE)
            iTEMP = MAX_ITEM_LIFE;
        sITEM.m_nLife = iTEMP;
        sITEM.m_cGrade = 0;
    }
    return true;
}

//-------------------------------------------------------------------------------------------------
// iGiveDamage = �����ڰ��� ������
__int64
CCal::Get_EXP(CObjCHAR* pAtkCHAR, CObjCHAR* pDefCHAR, int iGiveDamage) {
    __int64 iGAB, iEXP;

    iGAB = pAtkCHAR->Get_LEVEL() - pDefCHAR->Get_LEVEL();
    if (iGiveDamage > pDefCHAR->Get_MaxHP() * 1.15f)
        iGiveDamage = (int)(pDefCHAR->Get_MaxHP() * 1.15f);

    if (iGAB <= 3) {
        iEXP = (__int64)((float)((pDefCHAR->Get_LEVEL() + 3) * pDefCHAR->Get_GiveEXP()
                             * (iGiveDamage + pDefCHAR->Get_MaxHP() / 15.f + 30))
            * ::Get_WorldEXP() / (pDefCHAR->Get_MaxHP()) / 370.f);
    } else if (iGAB >= 4 && iGAB < 9) {
        iEXP = (__int64)((float)((pDefCHAR->Get_LEVEL() + 3) * pDefCHAR->Get_GiveEXP()
                             * (iGiveDamage + pDefCHAR->Get_MaxHP() / 15.f + 30))
            * ::Get_WorldEXP() / pDefCHAR->Get_MaxHP() / (iGAB + 3) / 60.f);
    } else {
        // �븸 6-13 kchs
        iEXP = (__int64)((float)((pDefCHAR->Get_LEVEL() + 3) * pDefCHAR->Get_GiveEXP()
                             * (iGiveDamage + pDefCHAR->Get_MaxHP() / 15.f + 30))
            * ::Get_WorldEXP() / pDefCHAR->Get_MaxHP() / (iGAB + 3) / 180.f);
    }

    if (iEXP < 1)
        iEXP = 1;

    return iEXP;
}

//-------------------------------------------------------------------------------------------------
// ���� ������
int
CCal::Get_SuccessRATE(CObjCHAR* pATK, CObjCHAR* pDEF) // , int &iCriticalSUC )
{
    int iRAND1, iRAND2;
    int iSuccess;

    if (pATK->IsUSER()) {
        if (pDEF->IsUSER()) {
            // PVP�ϰ�� ���� Ȯ��...
            iRAND1 = 1 + RANDOM(100);
            iSuccess = (int)(90 - ((pATK->Get_HIT() + pDEF->Get_AVOID()) / pATK->Get_HIT()) * 40.f
                + iRAND1);

        } else {
            iRAND1 = 1 + RANDOM(50); // 1+RANDOM(100) * 0.6f;
            iRAND2 = 1 + RANDOM(60); // 1+RANDOM(100) * 0.7f;
            iSuccess =
                (int)((pATK->Get_LEVEL() + 10) - pDEF->Get_LEVEL() * 1.1f + (iRAND1 /* *0.6f */));
            if (iSuccess <= 0)
                return 0;

            return (int)(iSuccess
                * (pATK->Get_HIT() * 1.1f - pDEF->Get_AVOID() * 0.93f + iRAND2 /* *0.7f */ + 5
                    + pATK->Get_LEVEL() * 0.2f)
                / 80.f);
        }
    } else {
        iRAND1 = 1 + RANDOM(100);
        iSuccess = 138 - ((float)(pATK->Get_HIT() + pDEF->Get_AVOID()) / pATK->Get_HIT()) * 75.0f
            + iRAND1; // 2005-7-13 kchs
    }

    return iSuccess;
}

//-------------------------------------------------------------------------------------------------
// ũ��Ƽ�� Success �� ���� 2005-7-13 �߰�
int
CCal::Get_CriSuccessRATE(CObjCHAR* pATK) {
    int iCriSuc = 0;

    if (pATK->IsUSER())
        iCriSuc =
            (int)(28
                - ((pATK->Get_CRITICAL() / 2.f + pATK->Get_LEVEL()) / (pATK->Get_LEVEL() + 8)) * 20)
            + 1 + RANDOM(100);
    else
        iCriSuc = 1 + RANDOM(100);

    return iCriSuc;
}

//-------------------------------------------------------------------------------------------------
// �Ϲ� ���� ������ ����...
WORD
CCal::Get_BasicDAMAGE(CObjCHAR* pATK, CObjCHAR* pDEF, WORD wHitCNT, int iSuc) {
    int iDamage, iCriSuc;

    iCriSuc = Get_CriSuccessRATE(pATK);

    // �´� ���� Ȯ��.
    int iHitActRATE = (28 - iCriSuc) * (pATK->Get_ATK() + 20) / (pDEF->Get_DEF() + 5);

    if (iCriSuc < 20) {
        // Critical damage !!!
        if (pATK->IsUSER() && pDEF->IsUSER()) {
            // PVP :: ũ��Ƽ�� ���� ������
            iDamage = (int)(pATK->Get_ATK() * ((float)pDEF->Get_LEVEL() / pATK->Get_LEVEL())
                * (iSuc * 0.05f + 29) * (2.4f * pATK->Get_ATK() - pDEF->Get_DEF() + 180)
                / (1.1f * pDEF->Get_DEF() + pDEF->Get_AVOID() * 0.3f + 50) / 85.f);
        } else {
            // ���� :: ũ��Ƽ�� ���� ������
            iDamage = (int)(pATK->Get_ATK() * (iSuc * 0.05f + 29)
                * (2.4f * pATK->Get_ATK() - pDEF->Get_DEF() + 180)
                / (1.1f * pDEF->Get_DEF() + pDEF->Get_AVOID() * 0.3f + 50) / 85.f);
        }
        // �߰� ������ ��ų ...
        if (FLAG_ING_DUMMY_DAMAGE & pATK->GetIngDurationStateFLAG()) {
            iDamage += (int)(iDamage * SKILL_POWER(pATK->GetIngDurationStateSKILL(ING_DUMMY_DAMAGE))
                / 100.f);
            // iDamage |= DMG_BIT_DUMMY;
        }

        iDamage *= wHitCNT;

        if (iDamage < 10)
            iDamage = 10;
        else {
            if (pATK->IsUSER() && pDEF->IsUSER()) {
                int iMaxDmg = (int)(pDEF->Get_MaxHP() * 0.35f);
                if (iDamage > iMaxDmg)
                    iDamage = iMaxDmg;
            } else if (iDamage > MAX_DAMAGE)
                iDamage = MAX_DAMAGE;
        }

        iDamage |= DMG_BIT_CRITICAL;
    } else {
        // Normal damage
        if (pATK->IsUSER() && pDEF->IsUSER()) {
            iDamage = (int)(pATK->Get_ATK() * ((float)pDEF->Get_LEVEL() / pATK->Get_LEVEL())
                * (iSuc * 0.03f + 26) * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
                / (1.1f * pDEF->Get_DEF() + pDEF->Get_AVOID() * 0.4f + 50) / 126.f);
        } else {
            iDamage = (int)(pATK->Get_ATK() * (iSuc * 0.03f + 26)
                * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
                / (1.1f * pDEF->Get_DEF() + pDEF->Get_AVOID() * 0.4f + 50) / 145.f);
        }
        // �߰� ������ ��ų ...
        if (FLAG_ING_DUMMY_DAMAGE & pATK->GetIngDurationStateFLAG()) {
            iDamage += (int)(iDamage * SKILL_POWER(pATK->GetIngDurationStateSKILL(ING_DUMMY_DAMAGE))
                / 100.f);
            // iDamage |= DMG_BIT_DUMMY;
        }

        iDamage *= wHitCNT;

        if (iDamage < 5)
            iDamage = 5;
        else {
            if (pATK->IsUSER() && pDEF->IsUSER()) {
                int iMaxDmg = (int)(pDEF->Get_MaxHP() * 0.25f);
                if (iDamage > iMaxDmg)
                    iDamage = iMaxDmg;
            } else if (iDamage > MAX_DAMAGE)
                iDamage = MAX_DAMAGE;
        }
    }

    return (iHitActRATE >= 10) ? (DMG_BIT_HITTED | iDamage) : iDamage;
}

//-------------------------------------------------------------------------------------------------
// ���� ������
WORD
CCal::Get_MagicDAMAGE(CObjCHAR* pATK, CObjCHAR* pDEF, WORD wHitCNT, int iSuc) {
    int iDamage, iCriSuc;

    iCriSuc = Get_CriSuccessRATE(pATK);

    // �´� ���� Ȯ��.
    int iHitActRATE = (28 - iCriSuc) * (pATK->Get_ATK() + 20) / (pDEF->Get_RES() + 5);

    if (iCriSuc < 20) {
        // Critical damage !!!
        if (pATK->IsUSER() && pDEF->IsUSER()) { // ���� ũ��Ƽ�� PVP
            iDamage = (int)(pATK->Get_ATK() * ((float)pDEF->Get_LEVEL() / pATK->Get_LEVEL())
                * (iSuc * 0.05f + 25) * (2.4f * pATK->Get_ATK() - pDEF->Get_DEF() + 180)
                / (1.1f * pDEF->Get_RES() + pDEF->Get_AVOID() * 0.3f + 50)
                / 90.f); //  (IROSE 2005.05.13)
        } else {
            iDamage = (int)(pATK->Get_ATK() * (iSuc * 0.05f + 25)
                * (2.4f * pATK->Get_ATK() - pDEF->Get_DEF() + 180)
                / (1.1f * pDEF->Get_RES() + pDEF->Get_AVOID() * 0.3f + 50) / 90.f);
        }

        // �߰� ������ ...
        if (FLAG_ING_DUMMY_DAMAGE & pATK->GetIngDurationStateFLAG()) {
            iDamage += (int)(iDamage * SKILL_POWER(pATK->GetIngDurationStateSKILL(ING_DUMMY_DAMAGE))
                / 100.f);
            // iDamage |= DMG_BIT_DUMMY;
        }

        iDamage *= wHitCNT;

        if (iDamage < 10)
            iDamage = 10;
        else {
            if (pATK->IsUSER() && pDEF->IsUSER()) {
                int iMaxDmg = (int)(pDEF->Get_MaxHP() * 0.35f);
                if (iDamage > iMaxDmg)
                    iDamage = iMaxDmg;
            } else if (iDamage > MAX_DAMAGE)
                iDamage = MAX_DAMAGE;
        }

        iDamage |= DMG_BIT_CRITICAL;
    } else {
        // Normal damage
        if (pATK->IsUSER() && pDEF->IsUSER()) { // ���� �Ϲ� PVP
            iDamage = (int)((float)pATK->Get_ATK() * (pDEF->Get_LEVEL() / pATK->Get_LEVEL())
                * (iSuc * 0.03f + 25) * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
                / (1.1f * pDEF->Get_RES() + pDEF->Get_AVOID() * 0.4f + 50)
                / 120.f); // kchs ���� : 2005-08-12 D_LEV - A_LEV ==> D_LEV / A_LEV
        } else {
            iDamage = (int)(pATK->Get_ATK() * (iSuc * 0.03f + 25)
                * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
                / (1.1f * pDEF->Get_RES() + pDEF->Get_AVOID() * 0.4f + 50) / 120.f);
        }

        // �߰� ������ ...
        if (FLAG_ING_DUMMY_DAMAGE & pATK->GetIngDurationStateFLAG()) {
            iDamage += (int)(iDamage * SKILL_POWER(pATK->GetIngDurationStateSKILL(ING_DUMMY_DAMAGE))
                / 100.f);
            // iDamage |= DMG_BIT_DUMMY;
        }

        iDamage *= wHitCNT;

        if (iDamage < 5)
            iDamage = 5;
        else {
            if (pATK->IsUSER() && pDEF->IsUSER()) {
                int iMaxDmg = (int)(pDEF->Get_MaxHP() * 0.25f);
                if (iDamage > iMaxDmg)
                    iDamage = iMaxDmg;
            } else if (iDamage > MAX_DAMAGE)
                iDamage = MAX_DAMAGE;
        }
    }

    return (iHitActRATE >= 10) ? (DMG_BIT_HITTED | iDamage) : iDamage;
}

//-------------------------------------------------------------------------------------------------
WORD
CCal::Get_DAMAGE(CObjCHAR* pATK, CObjCHAR* pDEF, WORD wHitCNT) {
    int iSuc = Get_SuccessRATE(pATK, pDEF);

    if (iSuc < 20) {
        if ((int)(1 + RANDOM(100) + (pATK->Get_LEVEL() - pDEF->Get_LEVEL()) * 0.6f) < 94)
            return 0;
    }

    wHitCNT = 1;

    if (pATK->IsMagicDAMAGE())
        return Get_MagicDAMAGE(pATK, pDEF, wHitCNT, iSuc);

    return Get_BasicDAMAGE(pATK, pDEF, wHitCNT, iSuc);
}

//-------------------------------------------------------------------------------------------------
int
CCal::Get_WeaponSkillDAMAGE(CObjCHAR* pATK, CObjCHAR* pDEF, short nSkillIDX, int iSuccess) {
    int iDamage;

    // [���� ��ų��] ������ ����
    if (pATK->IsUSER() && pDEF->IsUSER()) {
        // DMG= (SKILL_POW/100) *(D_LV/A_LV) * A_ATT * ( SUC*0.03+26)*(1.8* A_ATT - D_DEF +150) /
        // (1.1* D_DEF + D_AVO *0.4+50)/145
        iDamage = (int)((SKILL_POWER(nSkillIDX) / 100.f)
            * ((float)pDEF->Get_LEVEL() / pATK->Get_LEVEL()) * (pATK->Get_ATK())
            * (iSuccess * 0.03f + 26) * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
            / (1.1f * pDEF->Get_DEF() + pDEF->Get_AVOID() * 0.4f + 50) / 126.f);
    } else {
        // DMG = (SKILL_POW/100 ) * A_ATT * ( SUC*0.03+26)*(1.8* A_ATT - D_DEF +150) / (1.1* D_DEF +
        // D_AVO *0.4+50)/145
        iDamage = (int)((SKILL_POWER(nSkillIDX) / 100.f) * (pATK->Get_ATK())
            * (iSuccess * 0.03f + 26) * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
            / (1.1f * pDEF->Get_DEF() + pDEF->Get_AVOID() * 0.4f + 50) / 126.f);
    }

    return iDamage;
}
int
CCal::Get_MagicSkillDAMAGE(CObjCHAR* pATK, CObjCHAR* pDEF, short nSkillIDX, int iSuccess) {
    int iDamage;

    // [���� ��ų��] ������ ����
    if (pATK->IsUSER() && pDEF->IsUSER()) {
        // DMG = (SKILL_POW/100) *(D_LV/A_LV) * A_ATT *( SUC *0.03+25)*(1.8 *A_ATT - D_DEF
        // +150)/(1.1* D_RES + D_AVO *0.4+50)/140
        iDamage = (int)((SKILL_POWER(nSkillIDX) / 100.f)
            * ((float)pDEF->Get_LEVEL() / pATK->Get_LEVEL()) * (pATK->Get_ATK())
            * (iSuccess * 0.03f + 25) * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
            / (1.1f * pDEF->Get_RES() + pDEF->Get_AVOID() * 0.4f + 50) / 120.f);
    } else {
        // DMG = (SKILL_POW/100)  * A_ATT *( SUC *0.03+25)*(1.8 *A_ATT - D_DEF +150)/(1.1* D_RES +
        // D_AVO *0.4+50)/120
        iDamage = (int)((SKILL_POWER(nSkillIDX) / 100.f) * (pATK->Get_ATK())
            * (iSuccess * 0.03f + 25) * (1.8f * pATK->Get_ATK() - pDEF->Get_DEF() + 150)
            / (1.1f * pDEF->Get_RES() + pDEF->Get_AVOID() * 0.4f + 50) / 120.f);
    }

    return iDamage;
}

WORD
CCal::Get_SkillDAMAGE(CObjCHAR* pATK, CObjCHAR* pDEF, short nSkillIDX, WORD wHitCNT) {
    int iDamage;

    wHitCNT = 1;

    // int iSuccess  = Get_SuccessRATE (pATK, pDEF); // 2005-7-26 kchs ����. �߸��Ǽ� �ٽ� ������
    // ����
    switch (SKILL_DAMAGE_TYPE(nSkillIDX)) {
        case 1: // ���� ��ų
        {
            int iSuccess;
            int iRAND1 = 1 + RANDOM(60);
            int iRAND2 = 1 + RANDOM(70);
            iSuccess = (int)(((pATK->Get_LEVEL() + 20) - pDEF->Get_LEVEL() + (iRAND1 /* *0.6f */))
                * (pATK->Get_HIT() - pDEF->Get_AVOID() * 0.6f + iRAND2 /* *0.7f */ + 10) / 110.f);

            if (iSuccess < 20)
                return 0;

            iDamage = Get_WeaponSkillDAMAGE(pATK, pDEF, nSkillIDX, iSuccess);
            break;
        }
        case 2: // ���� ��ų
        {
            int iSuccess;
            int iRAND1 = 1 + RANDOM(50);
            int iRAND2 = 1 + RANDOM(70);
            iSuccess = (int)(((pATK->Get_LEVEL() + 30) - pDEF->Get_LEVEL() + (iRAND1))
                * (pATK->Get_HIT() - pDEF->Get_AVOID() * 0.56f + iRAND2 + 10) / 110.f);
            iDamage = Get_MagicSkillDAMAGE(pATK, pDEF, nSkillIDX, iSuccess);
            break;
        }
        case 3: // �Ǽ� ��ų
        {
            int iSuccess;
            int iRAND1 = 1 + RANDOM(80);
            int iRAND2 = 1 + RANDOM(50);
            iSuccess = (int)(((pATK->Get_LEVEL() + 10) - pDEF->Get_LEVEL() + (iRAND1))
                * (pATK->Get_HIT() - pDEF->Get_AVOID() * 0.5f + iRAND2 + 50) / 90.f);
            if (iSuccess < 20) {
                if (iSuccess < 6)
                    return 0;
                iDamage =
                    (int)((SKILL_POWER(nSkillIDX) * (SKILL_POWER(nSkillIDX) + pATK->Get_INT() + 80)
                              * ((1 + RANDOM(30)) + pATK->Get_SENSE() * 2 + 290) * 0.2f)
                            / (pDEF->Get_DEF() * 0.2f + pDEF->Get_RES() + 30)
                            / (250 + pDEF->Get_LEVEL() - pATK->Get_LEVEL())
                        + 20);
            } else {
                if (pATK->IsUSER() && pDEF->IsUSER()) {
                    iDamage = (int)(((35 + SKILL_POWER(nSkillIDX))
                                        * (SKILL_POWER(nSkillIDX) + pATK->Get_INT() + 140)
                                        * ((1 + RANDOM(30)) + pATK->Get_SENSE() + 380) * 0.01f)
                            * (400 - pDEF->Get_LEVEL() + pATK->Get_LEVEL())
                            / (pDEF->Get_DEF() * 0.5f + pDEF->Get_RES() * 1.2f
                                + pDEF->Get_AVOID() * 0.4f + 20)
                            / 3400
                        + 40);
                } else {
                    iDamage = (int)(((35 + SKILL_POWER(nSkillIDX))
                                        * (SKILL_POWER(nSkillIDX) + pATK->Get_INT() + 140)
                                        * ((1 + RANDOM(30)) + pATK->Get_SENSE() + 380) * 0.01f)
                            * (150 - pDEF->Get_LEVEL() + pATK->Get_LEVEL())
                            / (pDEF->Get_DEF() * 0.35f + pDEF->Get_RES() * 1.2f
                                + pDEF->Get_AVOID() * 0.4f + 10)
                            / 730
                        + 20);
                }
            }
            break;
        }
        default: {
            // �Ϲ� ���� �������� ���...
            int iSuccess;
            int iRAND1 = 1 + RANDOM(80);
            int iRAND2 = 1 + RANDOM(50);
            iSuccess = (int)(((pATK->Get_LEVEL() + 8) - pDEF->Get_LEVEL() + (iRAND1 /* *0.8 */))
                * (pATK->Get_HIT() - pDEF->Get_AVOID() * 0.6f + iRAND2 /* *0.5 */ + 50) / 90);
            if (iSuccess < 20) {
                if (iSuccess < 10)
                    return 0;
                iDamage = (int)(((SKILL_POWER(nSkillIDX) + 40) * (pATK->Get_ATK() + 40)
                                    * ((1 + RANDOM(30)) + pATK->Get_CRITICAL() * 0.2f + 40))
                        * 0.4f
                        / (pDEF->Get_DEF() + pDEF->Get_RES() * 0.3f + pDEF->Get_AVOID() * 0.4f + 10)
                        / 80
                    + 5);
            } else {
                if (pATK->IsUSER() && pDEF->IsUSER()) {
                    iDamage = (int)(((SKILL_POWER(nSkillIDX) + pATK->Get_CRITICAL() * 0.15f + 40)
                                        * (pATK->Get_ATK())
                                        * ((1 + RANDOM(30)) + pATK->Get_CRITICAL() * 0.32f + 35))
                            * 0.01f * (350 - pDEF->Get_LEVEL() + pATK->Get_LEVEL())
                            / (pDEF->Get_DEF() + pDEF->Get_RES() * 0.3f + pDEF->Get_AVOID() * 0.4f
                                + 35)
                            / 400
                        + 20);
                } else {
                    iDamage = (int)(((SKILL_POWER(nSkillIDX) + pATK->Get_CRITICAL() * 0.15f + 40)
                                        * (pATK->Get_ATK() + 70)
                                        * ((1 + RANDOM(30)) + pATK->Get_CRITICAL() * 0.34f + 35))
                            * 0.01f * (200 - pDEF->Get_LEVEL() + pATK->Get_LEVEL())
                            / (pDEF->Get_DEF() + pDEF->Get_RES() * 0.5f + pDEF->Get_AVOID() * 0.4f
                                + 40)
                            / 200.f
                        + 20);
                }
            }
        }
    }

    // �߰� ������ ...
    if (FLAG_ING_DUMMY_DAMAGE & pATK->GetIngDurationStateFLAG()) {
        iDamage +=
            (int)(iDamage * SKILL_POWER(pATK->GetIngDurationStateSKILL(ING_DUMMY_DAMAGE)) / 100.f);
        // iDamage |= DMG_BIT_DUMMY;
    }

#ifdef __APPLY_2ND_JOB
    if (IsTAIWAN()) {
        // �Ӽ� ���̺� �����Ͽ� ����
        short nSA = SKILL_ATTRIBUTE(nSkillIDX);
        short nNA = pDEF->Get_ATTRIBUTE();
        int iNewDamage = (int)(iDamage * ATTRIBUTE_FORM(nSA, nNA) / 100.f);
        iDamage = iNewDamage;
    }
#endif

    if (iDamage < 5)
        iDamage = 5;

    iDamage *= wHitCNT;
    if (pATK->IsUSER() && pDEF->IsUSER()) {
        int iMaxDmg = (int)(pDEF->Get_MaxHP() * 0.45f);
        if (iDamage > iMaxDmg)
            iDamage = iMaxDmg;
    } // else  ���� pDEF->Get_MaxHP()*0.45f �� ����� MAX_DAMAGE���� Ŭ�� �����Ƿ� else ����...

    if (iDamage > MAX_DAMAGE)
        iDamage = MAX_DAMAGE;

    // ��Ʈ ���� Ȯ��...
    int iHitActRATE;

    iHitActRATE = iDamage * (1 + (RANDOM(100)) + 100) / (pDEF->Get_AVOID() + 40) / 14;

    return (iHitActRATE >= 10) ? (DMG_BIT_HITTED | iDamage) : iDamage;
}

//-------------------------------------------------------------------------------------------------
int
CCal::Get_SkillAdjustVALUE(CObjUSER* pCHAR, short nSkillIDX, short nCol, int iSpellerINT) {
    int iAbilityValue;

#ifndef __SERVER
    iAbilityValue = pCHAR->GetCur_AbilityValue(SKILL_INCREASE_ABILITY(nSkillIDX, nCol));
#else
    iAbilityValue = pCHAR->Get_AbilityValue(SKILL_INCREASE_ABILITY(nSkillIDX, nCol));
#endif

    return (short)(iAbilityValue * SKILL_CHANGE_ABILITY_RATE(nSkillIDX, nCol) / 100.f
        + SKILL_INCREASE_ABILITY_VALUE(nSkillIDX, nCol) * (iSpellerINT + 300) / 315.f);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
