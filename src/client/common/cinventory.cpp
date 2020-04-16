#include "stdAFX.h"

#include "IO_STB.h"
#include "CInventory.h"

#ifndef __SERVER
    #include "..\\Object.h"
#endif

/*
static t_EquipINDEX s_EquipIDX[] = {
    EQUIP_IDX_NULL,					// Not Used

    EQUIP_IDX_FACE_ITEM,			// ITEM_TYPE_FACE_ITEM = 1,		// 1	LIST_FACEITEM.stb ¾ó±¼
Àå½Ä EQUIP_IDX_HELMET,				// ITEM_TYPE_HELMET,			// 2	LIST_CAP.stb
    EQUIP_IDX_ARMOR,				// ITEM_TYPE_ARMOR,				// 3	LIST_BODY.stb
    EQUIP_IDX_GAUNTLET,				// ITEM_TYPE_GAUNTLET,			// 4	LIST_ARMS.stb
    EQUIP_IDX_BOOTS,				// ITEM_TYPE_BOOTS,				// 5	LIST_FOOT.stb
    EQUIP_IDX_KNAPSACK,				// ITEM_TYPE_KNAPSACK,				// 6	LIST_BACK.stb

    EQUIP_IDX_NULL,					// Àå½Å±¸ : ¸ñ°ÉÀÌ ¹ÝÁö

    EQUIP_IDX_WEAPON_R,				// ITEM_TYPE_WEAPON,			// 8	LIST_WEAPON.stb		¹«±â
    EQUIP_IDX_WEAPON_L				// ITEM_TYPE_SUBWPN,			// 9	LIST_SUBWPN.stb
} ;

//-------------------------------------------------------------------------------------------------
t_EquipINDEX CInventory::GetEquipInvIDX (tagITEM *pITEM)
{
    if ( pITEM->GetTYPE() >= 1 && pITEM->GetTYPE() < ITEM_TYPE_USE ) {
        if ( pITEM->GetTYPE() == ITEM_TYPE_JEWEL ) {
            return EQUIP_IDX_NECKLACE,
            return EQUIP_IDX_RING,
            return EQUIP_IDX_RING2,
        }

        return ::s_EquipIDX[ pITEM->GetTYPE() ];
    }

    return EQUIP_IDX_NULL;
}
*/

t_InvTYPE CInventory::m_InvTYPE[ITEM_TYPE_MONEY] = {
    /*
    enum t_InvTYPE {
        INV_WEAPON = 0,
        INV_USE,
        INV_ETC,
        IMV_EMOTION,
        MAX_INV_TYPE
    } ;
    */
    MAX_INV_TYPE, //	Not used...

    INV_WEAPON, //	ITEM_TYPE_FACE = 1,				// 1	LIST_FACEITEM.stb	¾ó±¼ Àå½Ä
    INV_WEAPON, //	ITEM_TYPE_HELMET,				// 2	LIST_CAP.stb
    INV_WEAPON, //	ITEM_TYPE_ARMOR,				// 3	LIST_BODY.stb
    INV_WEAPON, //	ITEM_TYPE_GAUNTLET,				// 4	LIST_ARMS.stb
    INV_WEAPON, //	ITEM_TYPE_BOOTS,				// 5	LIST_FOOT.stb
    INV_WEAPON, //	ITEM_TYPE_KNAPSACK,				// 6	LIST_BACK.stb
    INV_WEAPON, //	ITEM_TYPE_JEWEL,				// 7	LIST_JEWEL.stb		Àå½Å±¸ : ¸ñ°ÉÀÌ ¹ÝÁö

    INV_WEAPON, //	ITEM_TYPE_WEAPON,				// 8	LIST_WEAPON.stb		¹«±â
    INV_WEAPON, //	ITEM_TYPE_SUBWPN,				// 9	LIST_SUBWPN.stb

    INV_USE, //	ITEM_TYPE_USE,					// 10	LIST_USEITEM.stb	¼Ò¸ð

    INV_ETC, //	ITEM_TYPE_ETC = ITEM_TYPE_GEM,	// 11						±âÅ¸ : º¸¼®
    INV_ETC, //	ITEM_TYPE_NATURAL,				// 12	LIST_NATURAL.stb
    INV_ETC, //	ITEM_TYPE_QUEST,				// 13	LIST_QUESTITEM.stb
    INV_RIDING, //	ITEM_TYPE_SPECIAL,				// 14	xxx
};

//-------------------------------------------------------------------------------------------------
void
CInventory::Clear() {
    m_i64Money = 0;
    ::ZeroMemory(m_ItemLIST, sizeof(m_ItemLIST));
#ifndef __SERVER
    ::ZeroMemory(m_btIndexLIST, sizeof(BYTE) * INVENTORY_TOTAL_SIZE);
#endif
}

#ifndef __SERVER
//-------------------------------------------------------------------------------------------------
//
/// ItemLIST ¸¦ ±â¹ÝÀ¸·Î IndexLIST ¸¦ ¸¸µç´Ù.
void
CInventory::MakeItemIndexList() {
    int i, j, iIndexListCount = 0;

    memset(m_btIndexLIST, 0, sizeof(m_btIndexLIST));

    for (i = 0; i < INVENTORY_TOTAL_SIZE; i++)
        m_btIndexLIST[i] = i;

    return;

    /// °¢ ¾ÆÀÌÅÛ Å¸ÀÔ¿¡ ´ëÇØ
    for (i = 0; i < MAX_INV_TYPE; i++) {
        iIndexListCount = 0;

        for (j = 0; j < INVENTORY_PAGE_SIZE; j++) {

            /// ¹º°¡ ÀÌÀÌÅÛÀÌ ÀÖ´Ù¸é
            if (m_ItemPAGE[i][j].m_cType != 0) {
                m_btIndexPAGE[i][iIndexListCount] = (MAX_EQUIP_IDX + i * INVENTORY_PAGE_SIZE) + j;
                iIndexListCount++;
            }
        }
    }
}

/// ½ÇÁ¦ ¾ÆÀÌÅÛ ÀÎµ¦½º·Î ÂüÁ¶Å×ÀÌºí ÀÎµ¦½º¸¦ ±¸ÇÑ´Ù.
short
CInventory::GetLookupIndexFromRealIndex(short nRealIndex) {
    int i, j;

    return nRealIndex;

    /// °¢ ¾ÆÀÌÅÛ Å¸ÀÔ¿¡ ´ëÇØ
    for (i = 0; i < MAX_INV_TYPE; i++) {
        for (j = 0; j < INVENTORY_PAGE_SIZE; j++) {
            if (m_btIndexPAGE[i][j] == nRealIndex) {
                return (MAX_EQUIP_IDX + i * INVENTORY_PAGE_SIZE) + j;
            }
        }
    }
    return -1;
}

//-------------------------------------------------------------------------------------------------
/// Lookup Table ¸¦ ÂüÁ¶ÇØ¼­ ½ÇÁ¦ ¾ÆÀÌÅÛÀ» ¾ò´Â´Ù.
bool
CInventory::IDX_GetITEM(short nLookUpIndexNO, tagITEM& OutITEM) {
    _ASSERT(nLookUpIndexNO >= 0 && nLookUpIndexNO < INVENTORY_TOTAL_SIZE);

    if (m_btIndexLIST[nLookUpIndexNO] > 0) {
        OutITEM = m_ItemLIST[m_btIndexLIST[nLookUpIndexNO]];

        if (OutITEM.GetTYPE())
            return true;
    }
    return false;
}

/// Lookup Table ¸¦ ÂüÁ¶ÇØ¼­ ½ÇÁ¦ ÀÎµ¦½º¸¦ ¾ò±âÀ§ÇÑ IDX_GetITEM ÇÔ¼ö¸¦ È£ÃâÇÑ´Ù.
bool
CInventory::IDX_GetITEM(short nInvTYPE, short nPageIndexNO, tagITEM& OutITEM) {
    _ASSERT(nInvTYPE >= 0 && nInvTYPE < MAX_EQUIP_IDX);
    _ASSERT(nPageIndexNO >= 0 && nPageIndexNO < INVENTORY_PAGE_SIZE);

    short nLookUpIndexNO = (MAX_EQUIP_IDX + nInvTYPE * INVENTORY_PAGE_SIZE) + nPageIndexNO;

    return IDX_GetITEM(nLookUpIndexNO, OutITEM);
}

//
//-------------------------------------------------------------------------------------------------
#endif // __SERVER

// 2005. 06. 15  ¹ÚÁöÈ£
// iEquit index ¸¦ ¾ÆÀÌÅÛ Å¸ÀÔÀ¸·Î º¯°æÇÑ´Ù.
short
CInventory::GetBodyPartToItemType(short nEquipSlot) {

    short nBodyPartIDX = MAX_BODY_PART;
    switch (nEquipSlot) {
        case BODY_PART_GOGGLE:
            nBodyPartIDX = ITEM_TYPE_FACE_ITEM;
            break;
        case BODY_PART_HELMET:
            nBodyPartIDX = ITEM_TYPE_HELMET;
            break;
        case BODY_PART_ARMOR:
            nBodyPartIDX = ITEM_TYPE_ARMOR;
            break;
        case BODY_PART_GAUNTLET:
            nBodyPartIDX = ITEM_TYPE_GAUNTLET;
            break;
        case BODY_PART_BOOTS:
            nBodyPartIDX = ITEM_TYPE_BOOTS;
            break;
        case BODY_PART_WEAPON_R:
            nBodyPartIDX = ITEM_TYPE_WEAPON;
            break;
        case BODY_PART_WEAPON_L:
            nBodyPartIDX = ITEM_TYPE_SUBWPN;
            break;

        case BODY_PART_KNAPSACK:
            nBodyPartIDX = ITEM_TYPE_KNAPSACK;
            break;
    }

    return nBodyPartIDX;
}

short
CInventory::GetBodyPartByEquipSlot(short nEquipSlot) {
    short nBodyPartIDX = MAX_BODY_PART;
    switch (nEquipSlot) {
        case EQUIP_IDX_FACE_ITEM:
            nBodyPartIDX = BODY_PART_FACE_ITEM;
            break;
        case EQUIP_IDX_HELMET:
            nBodyPartIDX = BODY_PART_HELMET;
            break;
        case EQUIP_IDX_ARMOR:
            nBodyPartIDX = BODY_PART_ARMOR;
            break;
        case EQUIP_IDX_KNAPSACK:
            nBodyPartIDX = BODY_PART_KNAPSACK;
            break;
        case EQUIP_IDX_GAUNTLET:
            nBodyPartIDX = BODY_PART_GAUNTLET;
            break;
        case EQUIP_IDX_BOOTS:
            nBodyPartIDX = BODY_PART_BOOTS;
            break;
        case EQUIP_IDX_WEAPON_R:
            nBodyPartIDX = BODY_PART_WEAPON_R;
            break;
        case EQUIP_IDX_WEAPON_L:
            nBodyPartIDX = BODY_PART_WEAPON_L;
            break;
    }

    /// Àåºñ ¾ÆÀÌÅÛÀÌ ¾Æ´Ï´Ù.
    return nBodyPartIDX;
}

//-------------------------------------------------------------------------------------------------
/// Real table ¿¡¼­ ¾ÆÀÌÅÛÀ» ¾ò´Â´Ù.
tagITEM
CInventory::LST_GetITEM(short nListNO) {
    _ASSERT(nListNO >= 0 && nListNO < INVENTORY_TOTAL_SIZE);

    return m_ItemLIST[nListNO];
}

/// Real table ¿¡¼­ ¾ÆÀÌÅÛÅ¸ÀÔ°ú, ÆäÀÌÁö ¹øÈ£·Î( LST_GetITEM( real list no ) È£Ãâ )¾ÆÀÌÅÛÀ» ¾ò´Â´Ù.
tagITEM
CInventory::LST_GetITEM(t_InvTYPE InvTYPE, short nPageListNO) {
    _ASSERT(InvTYPE >= 0 && InvTYPE < MAX_EQUIP_IDX);
    _ASSERT(nPageListNO >= 0 && nPageListNO < INVENTORY_PAGE_SIZE);

    short nListNO = (MAX_EQUIP_IDX + InvTYPE * INVENTORY_PAGE_SIZE) + nPageListNO;

    return LST_GetITEM(nListNO);
}

//-------------------------------------------------------------------------------------------------
/// Real table°ú Lookup table ¿¡ °¢ÀÚÀÇ ÀÎµ¦½º·Î ¾ÆÀÌÅÛ µî·Ï.
bool
CInventory::IDX_SetITEM(short nIndexNO, short nListNO, tagITEM& sITEM) {
#ifndef __SERVER
    m_btIndexLIST[nIndexNO] = (BYTE)nListNO;
#endif

    m_ItemLIST[nListNO] = sITEM;
    return true;
}

bool
CInventory::IDX_SetITEM(t_InvTYPE InvTYPE, short nPageListNO, short nTotListNO, tagITEM& sITEM) {
    _ASSERT(InvTYPE >= 0 && InvTYPE < MAX_EQUIP_IDX);
    _ASSERT(nPageListNO >= 0 && nPageListNO < INVENTORY_PAGE_SIZE);
    _ASSERT(nTotListNO >= 0 && nTotListNO < INVENTORY_TOTAL_SIZE);

    short nIndexNO = (MAX_EQUIP_IDX + InvTYPE * INVENTORY_PAGE_SIZE) + nPageListNO;

    return IDX_SetITEM(nIndexNO, nTotListNO, sITEM);
}
bool
CInventory::IDX_SetITEM(t_InvTYPE IdxInvTYPE,
    short nIdxPageListNO,
    t_InvTYPE LstInvTYPE,
    short nLstPageListNO,
    tagITEM& sITEM) {
    _ASSERT(IdxInvTYPE >= 0 && IdxInvTYPE < MAX_EQUIP_IDX);
    _ASSERT(nIdxPageListNO >= 0 && nIdxPageListNO < INVENTORY_PAGE_SIZE);

    _ASSERT(LstInvTYPE >= 0 && LstInvTYPE < MAX_EQUIP_IDX);
    _ASSERT(nLstPageListNO >= 0 && nLstPageListNO < INVENTORY_TOTAL_SIZE);

    short nIndexNO = (MAX_EQUIP_IDX + IdxInvTYPE * INVENTORY_PAGE_SIZE) + nIdxPageListNO;
    short nListNO = (MAX_EQUIP_IDX + LstInvTYPE * INVENTORY_PAGE_SIZE) + nLstPageListNO;

    return IDX_SetITEM(nIndexNO, nListNO, sITEM);
}

//-------------------------------------------------------------------------------------------------
short
CInventory::GetWEIGHT(short nListNO) {
    tagITEM* pITEM = &this->m_ItemLIST[nListNO];

    if (0 == pITEM->GetTYPE()) {
        return 0;
    }

    if (pITEM->IsEnableDupCNT()) {
        return ITEM_WEIGHT(pITEM->m_cType, pITEM->m_nItemNo) * pITEM->GetQuantity();
    }

    return ITEM_WEIGHT(pITEM->m_cType, pITEM->m_nItemNo);
}

//-------------------------------------------------------------------------------------------------
// iItemNO == 03005 (¹«»ç¼ö·Ãº¹)
void
CInventory::SetInventory(short nListNO, int iItem, int iQuantity) {
    if (0 == iItem)
        return;

    tagITEM sITEM;
    sITEM.Init(iItem, iQuantity);
    /*
        ::ZeroMemory( &sITEM, sizeof(sITEM) );
        sITEM.m_cType    = (char)(iItemNO / 1000);
        sITEM.m_nItemNo  = iItemNO % 1000;

        t_InvTYPE InvTYPE = m_InvTYPE[ sITEM.m_cType ];
        if ( INV_WEAPON == InvTYPE ) {
            sITEM.m_cResmelt = 0;
            sITEM.m_cQuality = ITEM_QUALITY( sITEM.m_cType, sITEM.m_nItemNo );
        } else {
            sITEM.m_uiQuantity = iQuantity;
        }
    */
    m_ItemLIST[nListNO] = sITEM;
}

//-------------------------------------------------------------------------------------------------
/// Server function
short
CInventory::AppendITEM(tagITEM& sITEM, short& nCurWeight) {
    _ASSERT(sITEM.GetTYPE());

    if (sITEM.IsEmpty()) {
        return -1;
    }

    if (ITEM_TYPE_MONEY == sITEM.m_cType) {
        // µ·ÀÌ´Ù..
        m_i64Money += sITEM.m_uiMoney;
        return 0;
    }

    t_InvTYPE InvTYPE = m_InvTYPE[sITEM.m_cType];
    if (sITEM.IsEnableDupCNT()) {
        for (short nI = 0; nI < INVENTORY_PAGE_SIZE; nI++) {
            // °°Àº ±âÅ¸ ¾ÆÀÌÅÛÀÌ¶óµµ ¾ÆÀÌÅÛ Å¸ÀÔÀÌ Æ²¸°°Íµé·Î ±¸¼ºµÊ...
            if (this->m_ItemPAGE[InvTYPE][nI].GetHEADER() == sITEM.GetHEADER()) {
                if (this->m_ItemPAGE[InvTYPE][nI].GetQuantity() + sITEM.GetQuantity()
                    <= MAX_DUP_ITEM_QUANTITY) {
                    // ´õÇßÀ» °æ¿ì ÃÖ´ë °¹¼ö°¡ ³Ñ¾î °¡¸é »õ ½º·Ô¿¡´Ù ÇÒ´ç.
                    nCurWeight +=
                        (ITEM_WEIGHT(sITEM.m_cType, sITEM.m_nItemNo) * sITEM.GetQuantity());

                    this->m_ItemPAGE[InvTYPE][nI].m_uiQuantity += sITEM.GetQuantity();
                    sITEM.m_uiQuantity = this->m_ItemPAGE[InvTYPE][nI].GetQuantity();

                    _ASSERT(INVENTORY_TOTAL_SIZE
                        > nI + MAX_EQUIP_IDX + (InvTYPE * INVENTORY_PAGE_SIZE));

                    return nI + MAX_EQUIP_IDX + (InvTYPE * INVENTORY_PAGE_SIZE);
                }
            }
        }
    }

    // Áßº¹µÉ¼ö ¾ø´Â Àåºñ¾ÆÀÌÅÛ, PAT¾ÆÀÌÅÛ ÀÌ³ª º¸À¯ÇÑ ¾ÆÀÌÅÛÁß¿¡ °°Àº ¾ÆÀÌÅÛÀÌ ¾ø´Â °æ¿ì...ºó½½·Ô
    // ÇÒ´ç...
    short nInvIDX = GetEmptyInventory(InvTYPE);
    if (nInvIDX >= 0) {
        AppendITEM(nInvIDX, sITEM, nCurWeight);
        return nInvIDX;
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
#ifndef __SERVER // ¼­¹ö¿¡¼± »ç¿ë ¾ÈÇÔ.
/// Client fucntion
short
CInventory::Add_CatchITEM(short nListRealNO, tagITEM& sITEM, short& nCurWeight) {
    if (ITEM_TYPE_MONEY == sITEM.m_cType) {
        // µ·¾ÆÀÌÅÛÀÏ °æ¿ì ÇöÀç µ·°ú ´õÇÑ´Ù..
        m_i64Money += sITEM.m_uiMoney;
        return 0;
    }

    if (nListRealNO >= INVENTORY_TOTAL_SIZE) {
        return -1;
    }

    if (m_ItemLIST[nListRealNO].m_dwITEM) {
        nCurWeight -= this->GetWEIGHT(nListRealNO);
    }

    if (m_ItemLIST[nListRealNO].GetTYPE() == sITEM.GetTYPE()) {
        // ´õÇÔ...
        m_ItemLIST[nListRealNO].m_uiQuantity += sITEM.m_uiQuantity;
    } else {
        // ±³Ã¼...
        m_ItemLIST[nListRealNO] = sITEM;
    }
    nCurWeight += this->GetWEIGHT(nListRealNO);

    /// Ãß°¡µÈ ¾ÆÀÌÅÛ¿¡ ´ëÇÑ Loopup table °»½Å
    MakeItemIndexList();
    g_pAVATAR->m_HotICONS.UpdateHotICON();

    return nListRealNO;
}
#endif

//-------------------------------------------------------------------------------------------------
/// Client & Server function
/// Real table ¿¡ ¾ÆÀÌÅÛÀ» Ãß°¡ÇÏ°í, Lookup table °»½Å
short
CInventory::AppendITEM(short nListRealNO, tagITEM& sITEM, short& nCurWeight) {
    _ASSERT(sITEM.GetTYPE());

    if (sITEM.IsEmpty()) {
        return -1;
    }

    if (ITEM_TYPE_MONEY == sITEM.m_cType) {
        // µ·¾ÆÀÌÅÛÀÏ °æ¿ì ÇöÀç µ·°ú ´õÇÑ´Ù..
        m_i64Money += sITEM.m_uiMoney;
        return 0;
    }

    if (nListRealNO >= INVENTORY_TOTAL_SIZE) {
        return -1;
    }

    // µ·À» Á¦¿ÜÇÑ ¾ÆÀÌÅÛÀº ¼­¹ö¿¡¼­ ¹ÞÀº ¾ÆÀÌÅÛÀ¸·Î ±³Ã¼...
    if (m_ItemLIST[nListRealNO].m_dwITEM) {
        nCurWeight -= this->GetWEIGHT(nListRealNO);
    }

    m_ItemLIST[nListRealNO] = sITEM;
    nCurWeight += this->GetWEIGHT(nListRealNO);

#ifndef __SERVER // ¼­¹ö¿¡¼± »ç¿ë ¾ÈÇÔ.
    /// Ãß°¡µÈ ¾ÆÀÌÅÛ¿¡ ´ëÇÑ Loopup table °»½Å
    MakeItemIndexList();
    g_pAVATAR->m_HotICONS.UpdateHotICON();
#endif

    return nListRealNO;
}

//-------------------------------------------------------------------------------------------------
/// Real Index ·Î ¾ÆÀÌÅÛÀ» ½½·Ô¿¡¼­ ºñ¿î´Ù.
void
CInventory::DeleteITEM(WORD wListRealNO) {
    m_ItemLIST[wListRealNO].Clear();

#ifndef __SERVER // ¼­¹ö¿¡¼± »ç¿ë ¾ÈÇÔ.
    short nLookUpIndex = GetLookupIndexFromRealIndex(wListRealNO);
    if (nLookUpIndex < 0) {
        //_ASSERT( 0 && "nLookUpIndex < 0" );
        return;
    }

    m_btIndexLIST[nLookUpIndex] = 0;

#endif
}

//-------------------------------------------------------------------------------------------------
// ÀÎº¥Åä¸®¿¡¼­ sITEMÀ» »«ÈÄ nCurWEIGHT¸¦ °»½ÅÇÑ´Ù.
void
CInventory::SubtractITEM(short nListNO, tagITEM& sITEM, short& nCurWEIGHT) {
    if (ITEM_TYPE_MONEY == sITEM.m_cType) {
        m_i64Money -= sITEM.m_uiMoney;
        return;
    }

    if (m_ItemLIST[nListNO].GetTYPE()) {
        nCurWEIGHT -= m_ItemLIST[nListNO].Subtract(sITEM);
    }
}
/*
// iQuantity°¹¼ö ¸¸Å­ Á¦°Å ÇÑ´Ù.
void CInventory::SubtractITEM (short nListNO, int iQuantity, short &nCurWEIGHT)
{
    tagITEM SubITEM = m_ItemLIST[ nListNO ];

    if ( SubITEM.IsEnableDupCNT() ) {
        // Áßº¹µÈ °¹¼ö¸¦ °®´Â ¾ÆÀÌÅÛÀÌ´Ù.
        SubITEM.m_uiQuantity = iQuantity;
    }

    nCurWEIGHT -= m_ItemLIST[ nListNO ].Subtract( SubITEM );
}
*/

short
CInventory::FindITEM(tagITEM& sITEM) {
    t_InvTYPE InvTYPE = m_InvTYPE[sITEM.m_cType];

    for (short nI = 0; nI < INVENTORY_PAGE_SIZE; nI++) {
        if (this->m_ItemPAGE[InvTYPE][nI].GetHEADER() == sITEM.GetHEADER()) {
            return nI + MAX_EQUIP_IDX + (InvTYPE * INVENTORY_PAGE_SIZE);
        }
    }
    return -1;
}

//-------------------------------------------------------------------------------------------------
/// @bug m_dwITEM == 0 À¸·Î¸¸ ÀÌ ½½·ÔÀÌ ºñ¾ú´Ù°í ÇÒ¼ö ÀÖ´Â°¡?
short
CInventory::GetEmptyInventory(short nInvPAGE) {
    _ASSERT(nInvPAGE >= INV_WEAPON && nInvPAGE < MAX_INV_TYPE);

    for (short nI = 0; nI < INVENTORY_PAGE_SIZE; nI++) {
        if (m_ItemLIST[MAX_EQUIP_IDX + (nInvPAGE * INVENTORY_PAGE_SIZE) + nI].m_cType == 0) {
            _ASSERT(INVENTORY_TOTAL_SIZE > nI + MAX_EQUIP_IDX + (nInvPAGE * INVENTORY_PAGE_SIZE));

            return nI + MAX_EQUIP_IDX + (nInvPAGE * INVENTORY_PAGE_SIZE);
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
short
CInventory::GetEmptyInvenSlotCount(t_InvTYPE InvType) {
    short nCount = 0;

    _ASSERT(InvType >= INV_WEAPON && InvType < MAX_INV_TYPE);

    for (short nl = 0; nl < INVENTORY_PAGE_SIZE; ++nl) {
        if (m_ItemPAGE[InvType][nl].GetTYPE() == 0)
            ++nCount;
    }
    return nCount;
}

//-------------------------------------------------------------------------------------------------
/// ¼­¹ö¿¡°Ô ÀÎº¥Åä¸® º¯°æ¿äÃ»À» ÇÏ±â Àü¿¡ ÀÎº¥Åä¸®°¡ FullÀÌ µÇ´Â°¡¸¦ CheckÇÏ±â À§ÇÑ Methodµé
/// ½ÇÁ¦ Inventory Á¶ÀÛ¿¡ »ç¿ëÇÏÁö ¸»°Í - 2004 / 11 /18 - nAvy
/// ½ÇÁ¦ ¼­¹ö¿¡¼­ÀÇ µ¿ÀÛ°ú À¯»çÇÏ°Ô ÇÏ±â À§ÇØ¼­ AppendITEM, FindITEM, SubtractITEMÀ» »ç¿ëÇÏ´Â
/// ¹æ½ÄÀ¸·Î ¹Ù²ñ.
//#ifndef __SERVER
// bool CInventory::Remove( tagITEM Item )
//{
//	if( !Item.IsEnableDupCNT() )
//	{
//		short InvenIndex = FindITEM( Item );
//		if( InvenIndex < 0 )///Not Found
//			return false;
//
//		ZeroMemory( &m_ItemLIST[ InvenIndex ], sizeof( tagITEM ) );
//	}
//	else
//	{
//		tagITEM TempItem = Item;
//		t_InvTYPE InvTYPE = m_InvTYPE[ TempItem.GetTYPE() ];
//
//		for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++)
//		{
//			if ( m_ItemPAGE[ InvTYPE ][ nI ].IsEqual( TempItem.GetTYPE(), TempItem.GetItemNO() ) )
//			{
//				if( m_ItemPAGE[ InvTYPE ][ nI ].GetQuantity() == TempItem.GetQuantity() )
//				{
//					ZeroMemory( &m_ItemPAGE[ InvTYPE ][ nI ], sizeof( tagITEM ) );
//					ZeroMemory( &TempItem, sizeof( tagITEM ) );
//					break;
//				}
//				else if( m_ItemPAGE[ InvTYPE ][ nI ].GetQuantity() > TempItem.GetQuantity() )
//				{
//					m_ItemPAGE[ InvTYPE ][ nI ].m_uiQuantity -= TempItem.GetQuantity();
//					ZeroMemory( &TempItem, sizeof( tagITEM ) );
//					break;
//				}
//				else
//				{
//					ZeroMemory( &m_ItemPAGE[ InvTYPE ][ nI ], sizeof( tagITEM ) );
//					TempItem.m_uiQuantity -= m_ItemPAGE[ InvTYPE ][ nI ].GetQuantity();
//				}
//			}
//		}
//
//		if( TempItem.GetQuantity() > 0 )
//			return false;
//	}
//	return true;
//}
//
// bool CInventory::Append( tagITEM& Item )
//{
//	short InvenIndex = 0;
//	if( !Item.IsEnableDupCNT() )
//	{
//		t_InvTYPE InvTYPE = m_InvTYPE[ Item.GetTYPE() ];
//		InvenIndex = GetEmptyInventory( InvTYPE );
//		if( InvenIndex < 0 )
//			return false;
//
//		memcpy( &m_ItemLIST[ InvenIndex ], &Item, sizeof( tagITEM ));
//	}
//	else
//	{
//		tagITEM TempItem  = Item;
//		t_InvTYPE InvTYPE = m_InvTYPE[ TempItem.GetTYPE() ];
//
////		while( TempItem.GetQuantity() > 0 )
//		{
//			InvenIndex = FindEnableAppendDupCNTItem( TempItem );
//			if( InvenIndex >= 0 )///Not Found
////				break;
//			{
//				if( m_ItemLIST[ InvenIndex ].GetQuantity() + TempItem.GetQuantity() <=
// MAX_DUP_ITEM_QUANTITY )
//				{
//					m_ItemLIST[ InvenIndex ].m_uiQuantity += TempItem.GetQuantity();
//					ZeroMemory( &TempItem, sizeof( tagITEM ) );
//					//break;
//				}
//				else
//				{
//					TempItem.m_uiQuantity -= MAX_DUP_ITEM_QUANTITY - m_ItemLIST[
// InvenIndex].GetQuantity(); 					m_ItemLIST[ InvenIndex ].m_uiQuantity =
// MAX_DUP_ITEM_QUANTITY;
//				}
//			}
//		}
//
//		if( TempItem.GetQuantity() > 0 )////Append is Not Finished
//		{
//			for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++)
//			{
//				if ( m_ItemPAGE[ InvTYPE ][ nI ].IsEmpty() )
//				{
//					if( TempItem.GetQuantity() <= MAX_DUP_ITEM_QUANTITY )
//					{
//						m_ItemPAGE[ InvTYPE ][ nI ] = TempItem;
//						ZeroMemory( &TempItem, sizeof( tagITEM ) );
//						break;
//					}
//					else
//					{
//						m_ItemPAGE[ InvTYPE ][ nI ] = TempItem;
//						m_ItemPAGE[ InvTYPE ][ nI ].m_uiQuantity = MAX_DUP_ITEM_QUANTITY;
//						TempItem.m_uiQuantity -= MAX_DUP_ITEM_QUANTITY;
//					}
//				}
//			}
//		}
//
//		if( TempItem.GetQuantity() > 0 )/// Append is Failed
//			return false;
//	}
//	return true;
//}
//
/////ÀÎº¥Åä¸®¿¡¼­ °³¼ö Áßº¹°¡´ÉÇÑ ¾ÆÀÌÅÛÀ» ´õÇÒ¶§ ´õÇÒ °ø°£ÀÌ ³²¾Æ ÀÖ´Â ¾ÆÀÌÅÛÀÇ ½½·ÔÀ» Ã£´Â´Ù.
/////ºó½½·ÔÀº Á¦¿ÜÇÑ´Ù.
// short CInventory::FindEnableAppendDupCNTItem( tagITEM& Item )
//{
//	if( !Item.IsEnableDupCNT() )
//		return -1;
//
//	t_InvTYPE InvTYPE = m_InvTYPE[ Item.GetTYPE() ];
//
//	for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++)
//	{
//		if( m_ItemPAGE[ InvTYPE ][ nI ].IsEqual( Item.GetTYPE(), Item.GetItemNO() ) && m_ItemPAGE[
// InvTYPE ][ nI ].GetQuantity() < MAX_DUP_ITEM_QUANTITY ) 			return MAX_EQUIP_IDX + ( InvTYPE
// * INVENTORY_PAGE_SIZE ) + nI;
//	}
//	return -1;
//}
/// #endif
/*-----------------------------------------------------------------------------------------------------*/
