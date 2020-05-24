
#include "stdAFX.h"

#include "CObjAVT.h"
#include "GS_ThreadZONE.h"

CObjAVT::CObjAVT() {
    m_pTargetNODE = new classDLLNODE<CObjAVT*>;
    m_pTargetNODE->DATA = this;
}
CObjAVT::~CObjAVT() {
    SAFE_DELETE(m_pTargetNODE);
}

void
CObjAVT::Update_SPEED() {
    float fSpeed = this->Cal_RunSPEED();
    this->m_nRunSPEED = floor_int(fSpeed);

    short nWeaponSpeed;
    if (this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN) {
        this->m_fRunAniSPEED = Cal_RunAniSPEED(m_nRunSPEED);

        short nWeaponItemNo = GetPartITEM(BODY_PART_WEAPON_R);
        nWeaponSpeed = WEAPON_ATTACK_SPEED(nWeaponItemNo) + 5;

        fSpeed = 1500.f / nWeaponSpeed + m_iAddValue[AT_ATK_SPD];
        this->m_nPassiveAttackSpeed = this->GetPassiveSkillAttackSpeed(fSpeed, nWeaponItemNo);
        this->m_nAtkAniSPEED = (short)(fSpeed + this->m_nPassiveAttackSpeed);
    } else {
        m_fRunAniSPEED = (m_nRunSPEED + 500) / 1000.f;

        nWeaponSpeed = PAT_ITEM_ATK_SPD(m_RideITEM[RIDE_PART_ARMS].m_nItemNo) + 5;
        fSpeed = 1500.f / nWeaponSpeed;
        m_nAtkAniSPEED = floor_int(fSpeed);
        m_nAtkAniSPEED += this->m_iAddValue[AT_ATK_SPD];
    }

    Cal_AruaAtkSPD();
}

//-------------------------------------------------------------------------------------------------
int
CObjAVT::Get_AttackRange(short nSkillIDX) {
    /// ½ºÅ³¿¡ °ø°Ý °Å¸®°¡ ÀÔ·ÂµÇ¾î ÀÖ´Ù¸é ½ºÅ³°Å¸® ¾Æ´Ï¸é ¹«±â °Å¸®..
    if (nSkillIDX && SKILL_DISTANCE(nSkillIDX)) {
        return SKILL_DISTANCE(nSkillIDX);
    }

    if (this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN) {
        /// ¾îÅÃ ·¹ÀÎÁö¸¦ ±¸ÇÑ´Ù. ¿À¸¥ÂÊ ¹«±â°¡ ÀÖÀ¸¸é ±× ¹«±âÀÇ ·¹ÀÎÁö ¸®ÅÏ.
        if (Get_R_WEAPON()) {
            return (int)(WEAPON_ATTACK_RANGE(this->Get_R_WEAPON()) + (Get_SCALE() * 120));
        }
    } else {
        // if ( this->m_nRideItemIDX[ RIDE_PART_ARMS ] ) {
        if (this->m_RideITEM[RIDE_PART_ARMS].m_nItemNo) {
            // ÀåÂøµÈ ¹«±â°¡ ÀÖ´Ù.
            // return (int) ( PAT_ITEM_ATK_RANGE( this->m_nRideItemIDX[RIDE_PART_ARMS] ) + (
            // Get_SCALE() * 120 ) );
            return (int)(PAT_ITEM_ATK_RANGE(this->m_RideITEM[RIDE_PART_ARMS].m_nItemNo)
                + (Get_SCALE() * 120));
        }
    }

    // Default attack range...
    return (int)(Def_AttackRange() + (this->Get_SCALE() * 120));
}

bool
CObjAVT::Make_gsv_ADD_OBJECT(classPACKET* pCPacket) {
    this->Init_ADD_CHAR(pCPacket);

    pCPacket->m_HEADER.m_wType = GSV_AVT_CHAR;
    pCPacket->m_HEADER.m_nSize = sizeof(gsv_AVT_CHAR);

    pCPacket->m_gsv_AVT_CHAR.m_btCharRACE = (BYTE)this->m_nCharRACE;
    pCPacket->m_gsv_AVT_CHAR.m_nRunSpeed =
        this->GetOri_RunSPEED(); // ÆÐ½Ãºê¿¡ÀÇÇØ¼­¸¸ º¸Á¤/ Áö¼Óº¸Á¤ Á¦¿ÜµÈ°ª.
    pCPacket->m_gsv_AVT_CHAR.m_nPsvAtkSpeed = this->GetPsv_ATKSPEED();
    pCPacket->m_gsv_AVT_CHAR.m_btWeightRate = this->m_btWeightRate;
    pCPacket->m_gsv_AVT_CHAR.m_dwSubFLAG = this->m_IngSTATUS.m_dwSubStatusFLAG;

    ::CopyMemory(pCPacket->m_gsv_AVT_CHAR.m_PartITEM,
        this->m_PartITEM,
        sizeof(tagPartITEM) * MAX_BODY_PART);

    pCPacket->m_gsv_AVT_CHAR.m_sShotItem[SHOT_TYPE_ARROW].m_wShotITEM =
        this->m_Inventory.m_ItemSHOT[SHOT_TYPE_ARROW].GetHEADER();
    pCPacket->m_gsv_AVT_CHAR.m_sShotItem[SHOT_TYPE_BULLET].m_wShotITEM =
        this->m_Inventory.m_ItemSHOT[SHOT_TYPE_BULLET].GetHEADER();
    pCPacket->m_gsv_AVT_CHAR.m_sShotItem[SHOT_TYPE_THROW].m_wShotITEM =
        this->m_Inventory.m_ItemSHOT[SHOT_TYPE_THROW].GetHEADER();

    ::CopyMemory(pCPacket->m_gsv_AVT_CHAR.m_RidingITEM,
        this->m_RideITEM,
        sizeof(tagPartITEM) * MAX_RIDING_PART);

    pCPacket->m_gsv_AVT_CHAR.m_nJOB = this->GetCur_JOB();
    pCPacket->m_gsv_AVT_CHAR.m_btLEVEL = this->GetCur_LEVEL();

    /*
    if ( this->m_btRideMODE )
        pCPacket->m_tag_ADD_CHAR.m_btMoveMODE = this->m_btRideMODE;
    */
    pCPacket->AppendString(this->Get_RNAME());

    this->Add_ADJ_STATUS(pCPacket);

    pCPacket->AppendString(this->Get_NAME());

    // °³ÀÎ »óÁ¡ »óÅÂ¸é »óÁ¡ Å¸ÀÌÆ²...
    if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_STORE_MODE)) {
#ifdef __INC_PLATINUM
        short nStoreSkin = this->m_GrowAbility.GetStoreSKIN(this->GetCurAbsSEC());
        pCPacket->AppendData(&nStoreSkin, sizeof(short));
#endif
        pCPacket->AppendString(this->Get_StoreTITLE());
    } else if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_INTRO_CHAT)) {
        pCPacket->AppendString(this->Get_StoreTITLE());
    }

    if (this->GetClanID()) {
        pCPacket->AppendData(&this->m_CLAN, sizeof(tag_CLAN_ID));
        pCPacket->AppendString(this->GetClanNAME());
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
tagMOTION*
CObjAVT::Get_MOTION(short nActionIdx) {
#ifdef _DEBUG
    if (nActionIdx >= g_TblAniTYPE.m_nDataCnt) {
        assert(0 && "nActionIdx >= g_TblAniTYPE.m_nDataCnt");
        return NULL;
    }
#endif

    short nFileIDX, nType;
    tagMOTION* pMOTION;

    if (!this->m_btRideMODE) {
        // ¿À¸¥¼Õ ¹«±â Á¾·ù¿¡µû¶ó...
        // nFileIDX = FILE_MOTION( WEAPON_MOTION_TYPE(this->m_nRWeaponIDX), nActionIdx );
        nFileIDX = FILE_MOTION(WEAPON_MOTION_TYPE(this->m_sRWeaponIDX.m_nItemNo), nActionIdx);
        nType = this->IsFemale();
    } else {
        nFileIDX = FILE_MOTION(this->m_nPatAniCOL, nActionIdx);
        nType = 0;
    }

    if (0 == nFileIDX) {
        // ¸ð¼ÇÀÌ ¾øÀ¸¸é ¸Ç¼Õ ¸ð¼ÇÀ¸·Î ´ëÃ¼ ??? :: 0¹ø ÆÄÀÏ ¾ø´Âµ¥ ??..
        // nFileIDX = FILE_MOTION( 0, nActionIdx );
        return NULL;
    }

    // TODO:: °°Àº µ¿ÀÛ¿¡ ´ëÇØ¼­´Â ³²/¿© ¸ðµÎ ÇÁ·¹ÀÓÀÌ °°´Ù.
    // Á¾Á·ÀÌ Æ²¸®¸é ???
    pMOTION = g_MotionFILE.IDX_GetMOTION(nFileIDX, nType);
    if (pMOTION) {
        pMOTION->m_nActionIdx = nActionIdx;
    }
    return pMOTION;
}

//-------------------------------------------------------------------------------------------------
static short s_nEquipIdxToPartNO[] = {
    -1, // 0
    BODY_PART_FACE_ITEM, // EQUIP_IDX_FACE_ITEM = 1,
    BODY_PART_HELMET, // EQUIP_IDX_HELMET,
    BODY_PART_ARMOR, // EQUIP_IDX_ARMOR,
    BODY_PART_KNAPSACK, // EQUIP_IDX_KNAPSACK,
    BODY_PART_GAUNTLET, // EQUIP_IDX_GAUNTLET,
    BODY_PART_BOOTS, // EQUIP_IDX_BOOTS,	// 5
    BODY_PART_WEAPON_R, // EQUIP_IDX_WEAPON_R,
    BODY_PART_WEAPON_L, // EQUIP_IDX_WEAPON_L,
    -1, // EQUIP_IDX_NECKLACE,
    -1, // EQUIP_IDX_RING,
};
void
CObjAVT::SetPartITEM(short nEquipInvIDX) {
    COMPILE_TIME_ASSERT(EQUIP_IDX_FACE_ITEM == 1);
    COMPILE_TIME_ASSERT(EQUIP_IDX_HELMET == 2);
    COMPILE_TIME_ASSERT(EQUIP_IDX_ARMOR == 3);
    COMPILE_TIME_ASSERT(EQUIP_IDX_KNAPSACK == 4);
    COMPILE_TIME_ASSERT(EQUIP_IDX_GAUNTLET == 5);
    COMPILE_TIME_ASSERT(EQUIP_IDX_BOOTS == 6);
    COMPILE_TIME_ASSERT(EQUIP_IDX_WEAPON_R == 7);
    COMPILE_TIME_ASSERT(EQUIP_IDX_WEAPON_L == 8);
    COMPILE_TIME_ASSERT(EQUIP_IDX_NECKLACE == 9);
    COMPILE_TIME_ASSERT(EQUIP_IDX_RING == 10);

    short nPartNO = s_nEquipIdxToPartNO[nEquipInvIDX];
    if (nPartNO > 0) {
        this->SetPartITEM(nPartNO, m_Inventory.m_ItemLIST[nEquipInvIDX]);
    }

    if (this->GetCur_MOVE_MODE() <= MOVE_MODE_RUN)
        this->UpdateAbility(); // ÆÄÆ® ¾ÆÀÌÅÛ ¼³Á¤
}
void
CObjAVT::SetRideITEM(short nRideInvIDX) {
    //	m_nRideItemIDX[ nRideInvIDX ] = this->m_Inventory.m_ItemRIDE[ nRideInvIDX ].GetItemNO();

    m_RideITEM[nRideInvIDX].m_nItemNo = this->m_Inventory.m_ItemRIDE[nRideInvIDX].GetItemNO();
    m_RideITEM[nRideInvIDX].m_nGEM_OP = this->m_Inventory.m_ItemRIDE[nRideInvIDX].GetGemNO();
    m_RideITEM[nRideInvIDX].m_cGrade = this->m_Inventory.m_ItemRIDE[nRideInvIDX].GetGrade();
    m_RideITEM[nRideInvIDX].m_bHasSocket = this->m_Inventory.m_ItemRIDE[nRideInvIDX].m_bHasSocket;

    if (this->GetCur_MOVE_MODE() == MOVE_MODE_DRIVE)
        this->UpdateAbility(); // Å¾½Â ¾ÆÀÌÅÛ ¼³Á¤
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::Set_TargetIDX(int iTargetIndex, bool bCheckHP) {
    if (Get_TargetIDX() == iTargetIndex)
        return;

    CObjCHAR* pTarget;
    if (Get_TargetIDX()) {
        pTarget = (CObjCHAR*)this->Get_TargetOBJ(
            true); // HP Ã¼Å©µÈ CAI_OBJ¸¦ »ó¼Ó¹ÞÀº CObjCHAR°¡ ³Ñ¾î ¿Â´Ù.
        if (pTarget) {
            pTarget->Sub_FromTargetLIST(this);
        }
    }

    this->CObjTARGET::Set_TargetIDX(iTargetIndex);

    if (Get_TargetIDX()) {
        pTarget = (CObjCHAR*)this->Get_TargetOBJ(
            bCheckHP); // HP Ã¼Å©µÈ CAI_OBJ¸¦ »ó¼Ó¹ÞÀº CObjCHAR°¡ ³Ñ¾î ¿Â´Ù.
        if (pTarget) {
            pTarget->Add_ToTargetLIST(this);
            this->Send_gsv_HP_REPLY(Get_TargetIDX(), pTarget->Get_HP());
        } else {
            // Ä³¸¯ÅÍ°¡ ¾Æ´Ï°Å³ª Á×¾ú´Ù...
            CObjTARGET::Set_TargetIDX(0);
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool
CObjAVT::SetCMD_TOGGLE(BYTE btTYPE, bool bForce) {
#ifdef __BLOCK_WHEN_SKILL
    // 2005.08.03 : icarus :: Á¶°Ç °Ë»ö¾øÀÌ °­Á¦ Åä±Û ½ÃÅ³¼ö ÀÖµµ·Ï ¼öÁ¤...
    if (!bForce && Get_ActiveSKILL()) {
        return true;
    }
#endif

    switch (btTYPE) {
        case TOGGLE_TYPE_RUN: {
            if (this->m_btRideMODE) {
                // Å¾½ÂÇÏ°í ??
                return true;
            }
            if (this->m_btWeightRate >= WEIGHT_RATE_WALK) {
                // ¶Û·Á±¸ ?
                return true;
            }

            this->m_bRunMODE = !this->m_bRunMODE;

            if (Get_STATE() == CS_MOVE) {
                this->m_fCurMoveSpeed = this->Get_MoveSPEED();
                this->Reset_MoveVEC();
            }
            btTYPE = TOGGLE_TYPE_DRIVE + this->Get_MoveMODE();
            break;
        }

        case TOGGLE_TYPE_SIT: {
            if (this->m_btRideMODE) {
                // Å¾½ÂÇÏ°í ??
                return true;
            }

            switch (this->Get_COMMAND()) {
                case CMD_STOP:
                    CObjAI::SetCMD_SIT();
                    break;

                case CMD_SIT:
                    CObjAI::SetCMD_STAND();
                    this->Send_gsv_SET_HPnMP(0x03); // hp, mp µÑ´Ù.
                    break;

                default:
                    // ¾ÉÀ»¼ö ¾ø´Â °æ¿ì...
                    return true;
            }
            break;
        }

        case TOGGLE_TYPE_DRIVE: {
            if (this->m_btRideMODE) {
                // ³»¸²..
                if (this->m_iLinkedCartObjIDX) {
                    classUSER* pUSER = g_pObjMGR->Get_UserOBJ(this->m_iLinkedCartObjIDX);
                    if (RIDE_MODE_GUEST == pUSER->GetCur_RIDE_MODE()) {
                        // ÅÂ¿ü´ø ¼Õ´Ôµµ ³»¸®°Ô...
                        pUSER->m_btRideMODE = 0;
                        pUSER->m_btRideATTR = RIDE_ATTR_NORMAL;
                    }
                    pUSER->m_iLinkedCartObjIDX = 0;
                }

                this->m_btRideMODE = 0;
                this->m_btRideATTR = RIDE_ATTR_NORMAL;
                this->m_iLinkedCartObjIDX = 0;
                this->m_IngSTATUS.ClearAllGOOD();
            } else {
                // ½ÂÂ÷, Á¶°Ç Ã¼Å©,
                for (short nI = 0; nI < MAX_RIDING_PART - 2; nI++) {
                    if (ITEM_TYPE_RIDE_PART != this->m_Inventory.m_ItemRIDE[nI].GetTYPE()) {
                        return true;
                    }
                }

                if (ZONE_RIDING_REFUSE_FLAG(this->GetZONE()->Get_ZoneNO())) {
                    int iType = ITEM_TYPE(ITEM_TYPE_RIDE_PART,
                        this->m_Inventory.m_ItemRIDE[RIDE_PART_BODY].GetItemNO());
                    if ((iType % 3) & ZONE_RIDING_REFUSE_FLAG(this->GetZONE()->Get_ZoneNO())) {
                        return true;
                    }
                }

#ifdef __KCHS_BATTLECART__
                if (this->GetCur_PatHP() <= 0) // Ä«Æ®Ã¼·ÂÀÌ 0ÀÌ¸é Ä«Æ®¸¦ ¼ÒÈ¯ÇÒ ¼ö ¾ø´Ù
                    return true;
#endif

                this->Dec_EngineLife(); // ½ÂÂ÷½Ã ¿¬·á °¨¼Ò
                this->m_btRideMODE = RIDE_MODE_DRIVE;

                if (PET_TYPE_CASTLE_GEAR01
                    == PAT_ITEM_PART_TYPE(this->m_Inventory.m_ItemRIDE[RIDE_PART_BODY].GetItemNO()))
                    this->m_btRideATTR = RIDE_ATTR_CASTLE;
#ifdef FRAROSE
                else if (PET_TYPE_MOUNT01 >= PAT_ITEM_PART_TYPE(
                             this->m_Inventory.m_ItemRIDE[RIDE_PART_BODY].GetItemNO())
                    && PET_TYPE_MAX <= PAT_ITEM_PART_TYPE(
                           this->m_Inventory.m_ItemRIDE[RIDE_PART_BODY].GetItemNO())) {
                    switch (PAT_ITEM_PART_TYPE(
                        this->m_Inventory.m_ItemRIDE[RIDE_PART_BODY].GetItemNO())) {
                        case PET_TYPE_MOUNT01:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT01;
                            break;
                        case PET_TYPE_MOUNT02:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT02;
                            break;
                        case PET_TYPE_MOUNT03:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT03;
                            break;
                        case PET_TYPE_MOUNT04:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT04;
                            break;
                        case PET_TYPE_MOUNT05:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT05;
                            break;
                        case PET_TYPE_MOUNT06:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT06;
                            break;
                        case PET_TYPE_MOUNT07:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT07;
                            break;
                        case PET_TYPE_MOUNT08:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT08;
                            break;
                        case PET_TYPE_MOUNT09:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT09;
                            break;
                        case PET_TYPE_MOUNT10:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT10;
                            break;
                        case PET_TYPE_MOUNT11:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT11;
                            break;
                        case PET_TYPE_MOUNT12:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT12;
                            break;
                        case PET_TYPE_MOUNT13:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT13;
                            break;
                        case PET_TYPE_MOUNT14:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT14;
                            break;
                        case PET_TYPE_MOUNT15:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT15;
                            break;
                        case PET_TYPE_MOUNT16:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT16;
                            break;
                        case PET_TYPE_MOUNT17:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT17;
                            break;
                        case PET_TYPE_MOUNT18:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT18;
                            break;
                        case PET_TYPE_MOUNT19:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT19;
                            break;
                        case PET_TYPE_MOUNT20:
                            this->m_btRideATTR = RIDE_ATTR_MOUNT20;
                            break;
                    }
                }
#endif
                else
                    this->m_btRideATTR = RIDE_ATTR_CART;

                // Å¾½Â½Ã ¸ðµç À¯¸® »óÅÂ ÇØÁö...
                this->m_IngSTATUS.ClearAllGOOD();
            }
            this->m_dwRecoverTIME = 0;

            this->UpdateAbility(); // Å¾½Â Åä±Û...
            btTYPE = TOGGLE_TYPE_DRIVE + this->Get_MoveMODE();
            return this->Send_gsv_TOGGLE(btTYPE, true);
        }
    }

    // except TOGGLE_TYPE_DRIVE
    this->Send_gsv_TOGGLE(btTYPE, false);

    return true;
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::SetCur_SEX(char cValue) {
    this->m_nCharRACE &= 0x0fffe; // ÇÏÀ§ºñÆ® ³¯¸®°í~
    this->m_nCharRACE |= (cValue & 0x01); // ÇÏÀ§ºñÆ® ¼³Á¤ÇÏ°í~
}

int
CObjAVT::GetCur_FUEL() {
    tagITEM* pEngine = &m_Inventory.m_ItemRIDE[RIDE_PART_ENGINE];
    if (ITEM_TYPE_RIDE_PART != pEngine->GetTYPE())
        return 0;
    return pEngine->m_nLife;
}
void
CObjAVT::SubCur_FUEL(short nValue) {
    tagITEM* pEngine = &m_Inventory.m_ItemRIDE[RIDE_PART_ENGINE];
    if (ITEM_TYPE_RIDE_PART != pEngine->GetTYPE())
        return;

    if (pEngine->m_nLife > nValue)
        pEngine->m_nLife -= nValue;
    else {
        // ¼ö¸í 0µÆ´Ù...
        pEngine->m_nLife = 0;
        this->Send_gsv_SET_ITEM_LIFE(INVENTORY_RIDE_ITEM0 + RIDE_PART_ENGINE, 0);
        // this->SetCMD_STOP ();
    }
}

int
CObjAVT::Get_AbilityValue(WORD wType) {
    switch (wType) {
        // case AT_BIRTHSTONE :	break;
        case AT_SEX:
            return (GetCur_RACE() & 0x01);
        case AT_RACE:
            return (GetCur_RACE() / 0x02);
        case AT_CLASS:
            return GetCur_JOB();
        case AT_UNION:
            return GetCur_UNION();
        case AT_RANK:
            return GetCur_RANK();
        case AT_FAME:
            return GetCur_FAME();

        case AT_FACE:
            return GetCur_FACE();
        case AT_HAIR:
            return GetCur_HAIR();

        case AT_STR:
            return GetCur_STR();
        case AT_DEX:
            return GetCur_DEX();
        case AT_INT:
            return GetCur_INT();
        case AT_CON:
            return GetCur_CON();
        case AT_CHARM:
            return GetCur_CHARM();
        case AT_SENSE:
            return GetCur_SENSE();
        case AT_HP:
            return GetCur_HP();
        case AT_MP:
            return GetCur_MP();
        case AT_ATK:
            return GetCur_ATK();
        case AT_DEF:
            return GetCur_DEF();
        case AT_HIT:
            return GetCur_HIT();
        case AT_RES:
            return GetCur_RES();
        case AT_AVOID:
            return GetCur_AVOID();
        case AT_SPEED:
            return (int)GetCur_MOVE_SPEED();
        case AT_ATK_SPD:
            return GetCur_ATK_SPD();
        case AT_WEIGHT:
            return GetCur_WEIGHT();
        case AT_CRITICAL:
            return Get_CRITICAL();

        case AT_SAVE_MP:
            return GetCur_SaveMP();
        case AT_DROP_RATE:
            return GetCur_DropRATE();

        case AT_FAME_G:
            return GetCur_FameG();
        case AT_FAME_B:
            return GetCur_FameB();

        case AT_STAMINA:
            return GetCur_STAMINA();

        case AT_GUILD_NO:
            return GetClanID();
        case AT_GUILD_POS:
            return GetClanPOS();
        case AT_GUILD_SCORE:
            return GetClanSCORE();

            // case AT_RECOVER_HP :
            // case AT_RECOVER_MP :	break;

        case AT_EXP:
            return GetCur_EXP();
        case AT_LEVEL:
            return GetCur_LEVEL();
        case AT_BONUSPOINT:
            return GetCur_BonusPOINT();
        case AT_SKILLPOINT:
            return GetCur_SkillPOINT();

        case AT_PK_FLAG:
            return GetCur_PK_FLAG();
        case AT_TEAM_NO:
            return Get_TeamNO();

        case AT_HEAD_SIZE:
            return GetCur_HeadSIZE();
        case AT_BODY_SIZE:
            return GetCur_BodySIZE();

        case AT_MONEY:
            if (GetCur_MONEY() > MAX_INT)
                return MAX_INT;
            return (int)GetCur_MONEY();

        case AT_MAX_HP:
            return GetCur_MaxHP();
        case AT_MAX_MP:
            return GetCur_MaxMP();

        case AT_CURRENT_PLANET:
            if (this->GetZONE())
                return ZONE_PLANET_NO(this->GetZONE()->Get_ZoneNO());
            return -1;

        default:
            if (wType >= AT_UNION_POINT1 && wType <= AT_UNION_POINT10)
                return GetCur_UnionPOINT(wType - AT_UNION_POINT1 + 1);
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::Stamp_AttackTIME() {
    this->m_dwAttackTIME = this->GetZONE()->GetTimeGetTIME();
}

int
CObjAVT::Save_Damage(int iAttackerIDX, int iDamage) {
    this->Stamp_AttackTIME();

    // ¹æ¾î±¸ ¼ö¸í °¨¼Ò
    int iRand = 1 + RANDOM(400);
    if (iRand >= 101)
        return 0;

    tagITEM* pItem;
    short nInvIDX;

    if (this->Get_MoveMODE() == MOVE_MODE_DRIVE) {
        if (iRand >= 51)
            nInvIDX = INVENTORY_RIDE_ITEM0 + RIDE_PART_BODY; // ¹Ùµð
        else if (iRand >= 21)
            nInvIDX = INVENTORY_RIDE_ITEM0 + RIDE_PART_LEG; // ·¹±×
        else
        // if ( iRand >= 16 )
        {
            nInvIDX = INVENTORY_RIDE_ITEM0 + RIDE_PART_ARMS; // ¿þÆù
        }
        // else				nInvIDX =  INVENTORY_RIDE_ITEM0 + RIDE_PART_ENGINE;	// ¿£Áø
    } else {
        pItem = &m_Inventory.m_ItemLIST[EQUIP_IDX_WEAPON_L];
        if (pItem->GetLife()
            && WEAPON_ITEM_SHIELD == ITEM_TYPE(pItem->GetTYPE(), pItem->GetItemNO())) {
            // ¹æÆÐ ÀÖÀ½
            if (iRand > 60)
                nInvIDX = EQUIP_IDX_WEAPON_L; // ¹æÆÐ, ¿Þ¼Õ¹«±â
            else if (iRand > 30)
                nInvIDX = EQUIP_IDX_ARMOR; // °©¿Ê
            else if (iRand > 22)
                nInvIDX = EQUIP_IDX_BOOTS; // ½Å¹ß
            else if (iRand > 16)
                nInvIDX = EQUIP_IDX_HELMET; // ¸Ó¸®
            else if (iRand > 9)
                nInvIDX = EQUIP_IDX_KNAPSACK; // µî
            else if (iRand > 5)
                nInvIDX = EQUIP_IDX_GAUNTLET; // Àå°©
            else
                nInvIDX = EQUIP_IDX_FACE_ITEM; // ¾ó±¼(¾Ç¼¼»ç¸®)
        } else {
            // ¹æÆÐ ¾øÀ½
            if (iRand > 64)
                nInvIDX = EQUIP_IDX_ARMOR; // °©¿Ê
            else if (iRand > 46)
                nInvIDX = EQUIP_IDX_BOOTS; // ½Å¹ß
            else if (iRand > 31)
                nInvIDX = EQUIP_IDX_GAUNTLET; // Àå°©
            else if (iRand > 15)
                nInvIDX = EQUIP_IDX_KNAPSACK; // µî
            else if (iRand > 7)
                nInvIDX = EQUIP_IDX_HELMET; // ¸Ó¸®
            else
                nInvIDX = EQUIP_IDX_FACE_ITEM; // ¾ó±¼(¾Ç¼¼»ç¸®)
        }
    }

    pItem = &m_Inventory.m_ItemLIST[nInvIDX];
    if (0 == pItem->GetHEADER() || pItem->GetLife() <= 0)
        return 0;

    iRand = 1 + RANDOM(120) - (pItem->GetDurability() + 10 - (int)(iDamage * 0.1f));
    if (iRand >= 0) {
        // ³»±¸µµ 1 °¨¼Ò, 10´ÜÀ§ º¯È­°¡ ÀÖÀ¸¸é Å¬¶óÀÌ¾ðÆ®¿¡ Àü¼Û
        if (pItem->GetLife() > 1) {
            pItem->m_nLife--;
            if (pItem->GetLife() % 10 == 0) {
                // º¯°æµÈ ¼ö¸í Àü¼Û...
                this->Send_gsv_SET_ITEM_LIFE(nInvIDX, pItem->GetLife());
            }
        } else {
            pItem->m_nLife = 0;
            this->Send_gsv_SET_ITEM_LIFE(nInvIDX, 0);

            short nCurSpeed = this->GetOri_RunSPEED();
            this->UpdateAbility(); // ¹æ¾î±¸ ¼ö¸í = 0
            if (nCurSpeed != this->GetOri_RunSPEED())
                this->Send_gsv_SPEED_CHANGED(false);
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::Dec_EngineLife() {
    tagITEM* pEngine = &m_Inventory.m_ItemRIDE[RIDE_PART_ENGINE];
    if (ITEM_TYPE_RIDE_PART != pEngine->GetTYPE())
        return;

    int iUseValue = PAT_ITEM_USE_FUEL_RATE(pEngine->GetItemNO());
    if (pEngine->m_nLife > iUseValue) {
        int iDiv10 = pEngine->m_nLife / 10;
        pEngine->m_nLife -= iUseValue;
        if (iDiv10 != (pEngine->m_nLife / 10))
            this->Send_gsv_SET_ITEM_LIFE(INVENTORY_RIDE_ITEM0 + RIDE_PART_ENGINE,
                pEngine->GetLife());
    } else {
        // ¼ö¸í 0µÆ´Ù...
        pEngine->m_nLife = 0;
        this->Send_gsv_SET_ITEM_LIFE(INVENTORY_RIDE_ITEM0 + RIDE_PART_ENGINE, 0);
        this->SetCMD_STOP();
    }
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::Dec_WeaponLife() {
    tagITEM* pWeapon;
    short nInvIDX;

    if (this->GetCur_MOVE_MODE() == MOVE_MODE_DRIVE) {
        this->Dec_EngineLife(); // °ø°Ý½Ã ¸¶´Ù ¿£Áø ¿¬·á °¨¼Ò...

        // PAT
        nInvIDX = INVENTORY_RIDE_ITEM0 + RIDE_PART_ARMS;
    } else {
        nInvIDX = EQUIP_IDX_WEAPON_R;
    }

    pWeapon = &m_Inventory.m_ItemLIST[nInvIDX];
    if (0 == pWeapon->GetHEADER() || pWeapon->GetLife() <= 0)
        return;

    if (RANDOM(710) + 1 - (pWeapon->GetDurability() + 600) >= 0) {
        // ¼ö¸í 1 °¨¼Ò
        if (pWeapon->GetLife() > 1) {
            pWeapon->m_nLife--;
            if (pWeapon->GetLife() % 10 == 0) {
                // º¯°æµÈ ¼ö¸í Àü¼Û...
                this->Send_gsv_SET_ITEM_LIFE(nInvIDX, pWeapon->GetLife());
            }
        } else {
            pWeapon->m_nLife = 0;
            this->Send_gsv_SET_ITEM_LIFE(nInvIDX, 0);

            short nCurSpeed = this->GetOri_RunSPEED();
            this->UpdateAbility(); // ¹«±â ¼ö¸í = 0
            if (nCurSpeed != this->GetOri_RunSPEED())
                this->Send_gsv_SPEED_CHANGED(false);
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool
CObjAVT::Send_gsv_GODDNESS_MODE(BYTE btOnOff) {
    classPACKET* pCPacket = Packet_AllocNLock();
    if (!pCPacket)
        return false;

    pCPacket->m_HEADER.m_wType = GSV_GODDNESS_MODE;
    pCPacket->m_HEADER.m_nSize = sizeof(gsv_GODDNESS_MODE);
    pCPacket->m_gsv_GODDNESS_MODE.m_btOnOff = btOnOff;
    pCPacket->m_gsv_GODDNESS_MODE.m_wObjectIDX = this->Get_INDEX();

    this->GetZONE()->SendPacketToSectors(this, pCPacket);

    Packet_ReleaseNUnlock(pCPacket);

    return true;
}

//-------------------------------------------------------------------------------------------------
//#define	ARUA_RATE_ATK	0.1f
//#define	ARUA_RATE_HIT	0.1f
//#define	ARUA_RATE_AVD	0.1f

#define ARUA_RATE_ATK_SPD 0.2f
#define ARUA_RATE_RUN_SPD 0.2f
#define ARUA_RATE_CRI 0.2f
#define ARUA_RATE_MAX_HP 0.2f
#define ARUA_RATE_MAX_MP 0.2f
#define ARUA_RATE_RCV_HP 0.5f
#define ARUA_RATE_RCV_MP 0.5f
#define ARUA_RATE_ATK 0.2f
#define ARUA_RATE_RES 0.2f

void
CObjAVT::Cal_AruaAtkSPD(void) {
    // if ( this->m_IngSTATUS.IsSubSET( FLAG_SUB_ARUA_FAIRY ) ) {
    //	this->m_IngSTATUS.m_nAruaAtkSPD = (short)( this->GetOri_ATKSPEED() * ARUA_RATE_ATK_SPD );
    //} else
    //	this->m_IngSTATUS.m_nAruaAtkSPD = 0;
}
void
CObjAVT::Cal_AruaRunSPD(void) {
    if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_ARUA_FAIRY)) {
        this->m_IngSTATUS.m_nAruaRunSPD =
            (short)(this->GetOri_RunSPEED() * ARUA_RATE_RUN_SPD); // 0.2f
    } else
        this->m_IngSTATUS.m_nAruaRunSPD = 0;
}

void
CObjAVT::Cal_AruaMaxHP(void) {
    if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_ARUA_FAIRY)) {
        this->m_IngSTATUS.m_nAruaMaxHP = (short)(this->GetOri_MaxHP() * ARUA_RATE_MAX_HP); // 0.2f
    } else
        this->m_IngSTATUS.m_nAruaMaxHP = 0;
}
void
CObjAVT::Cal_AruaMaxMP(void) {
    if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_ARUA_FAIRY)) {
        this->m_IngSTATUS.m_nAruaMaxMP = (short)(this->GetOri_MaxMP() * ARUA_RATE_MAX_MP); // 0.2f
    } else
        this->m_IngSTATUS.m_nAruaMaxMP = 0;
}

void
CObjAVT::Cal_AruaATTACK(void) {
    if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_ARUA_FAIRY)) {
        this->m_IngSTATUS.m_nAruaATK = (short)(this->GetOri_ATK() * ARUA_RATE_ATK);
    } else
        this->m_IngSTATUS.m_nAruaATK = 0;
}
void
CObjAVT::Cal_AruaHIT(void) {
    // if ( this->m_IngSTATUS.IsSubSET( FLAG_SUB_ARUA_FAIRY ) ) {
    //	this->m_IngSTATUS.m_nAruaHIT = (short)( GetOri_HIT() * ARUA_RATE_HIT );
    //} else
    //	this->m_IngSTATUS.m_nAruaHIT = 0;
}
void
CObjAVT::Cal_AruaAVOID(void) {
    // if ( this->m_IngSTATUS.IsSubSET( FLAG_SUB_ARUA_FAIRY ) ) {
    //	this->m_IngSTATUS.m_nAruaAVOID = (short)( GetOri_AVOID() * ARUA_RATE_AVD );
    //} else
    //	this->m_IngSTATUS.m_nAruaAVOID = 0;
}
void
CObjAVT::Cal_AruaCRITICAL(void) {
    if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_ARUA_FAIRY)) {
        this->m_IngSTATUS.m_nAruaCRITICAL = (short)(GetOri_CRITICAL() * ARUA_RATE_CRI);
    } else
        this->m_IngSTATUS.m_nAruaCRITICAL = 0;
}

void
CObjAVT::Cal_AruaRES(void) {
    if (this->m_IngSTATUS.IsSubSET(FLAG_SUB_ARUA_FAIRY)) {
        this->m_IngSTATUS.m_nAruaRES = (short)(this->GetOri_DEF() * ARUA_RATE_RES);
    } else
        this->m_IngSTATUS.m_nAruaRES = 0;
}

bool
CObjAVT::Add_Goddess(void) {
    this->m_IngSTATUS.SetSubFLAG(FLAG_SUB_ARUA_FAIRY);

    Cal_AruaMaxHP();
    Cal_AruaMaxMP();

    Cal_AruaRunSPD();
    Cal_AruaAtkSPD();

    Cal_AruaATTACK();
    Cal_AruaHIT();
    Cal_AruaAVOID();

    Cal_AruaCRITICAL();
    Cal_AruaRES();

    return this->Send_gsv_GODDNESS_MODE(1);
}
bool
CObjAVT::Del_Goddess(void) {
    DWORD dwIsSet = this->m_IngSTATUS.IsSubSET(FLAG_SUB_ARUA_FAIRY);

    this->m_IngSTATUS.DelArua();
    if (this->GetCur_HP() > this->Get_MaxHP())
        this->SetCur_HP(Get_MaxHP());
    if (this->GetCur_MP() > this->Get_MaxMP())
        this->SetCur_MP(Get_MaxMP());

    return dwIsSet ? this->Send_gsv_GODDNESS_MODE(0) : true;
}

//-------------------------------------------------------------------------------------------------
#define CHECK_GODDNESS_CALL_TIME (60 * 60 * 1000)

#define CHECK_PAT_COOL_TIME (1000)
#define PAT_DECREASE_COOLTIME_PER_SEC (1 * 1000) // 1ÃÊ½Ä °¨¼Ò½ÃÄÑ¼­..

void
CObjAVT::Check_PerFRAME(DWORD dwPassTIME) {
    m_dwGoddnessTIME += dwPassTIME;
    m_dwRecoverTIME += dwPassTIME;

    switch (this->Get_MoveMODE()) {
        case MOVE_MODE_RIDEON:
            break;
        case MOVE_MODE_DRIVE:
            if (m_dwRecoverTIME >= USE_FUEL_CHEC_TIME) {
                m_dwRecoverTIME -= USE_FUEL_CHEC_TIME;
                this->Dec_EngineLife();
            }
            break;
        default: // HP / MP È¸º¹
        {
            DWORD dwCheckTime = RECOVER_STATE_CHECK_TIME;
            if (m_dwRecoverTIME >= dwCheckTime) {
                m_dwRecoverTIME -= dwCheckTime;

                // if ( this->GetCur_STAMINA() >= YELLOW_STAMINA )
                {
                    // ½ºÅ×¹Ì³Ê Ã¼Å©...
                    int iAdd;
                    if (CMD_SIT == Get_COMMAND()) {
                        iAdd = this->Get_RecoverHP(RECOVER_STATE_SIT_ON_GROUND);
                        this->Add_HP(iAdd);

                        iAdd = this->Get_RecoverMP(RECOVER_STATE_SIT_ON_GROUND);
                        this->Add_MP(iAdd);
                    } else {
                        iAdd =
                            (int)((this->GetAdd_RecoverHP() + (this->Get_CON() + 40) / 6.f) / 6.f);
                        this->Add_HP(iAdd);
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::Resurrection(short nSkillIDX) {
    this->m_IngSTATUS.Reset(false);
    this->m_dwRecoverTIME = 0;

    this->Del_ActiveSKILL();
    this->Clear_SummonCNT(); // ºÎÈ°½Ã...
    this->CObjAI::SetCMD_STOP();

    // Á×À»¶§ °¨¼ÒµÈ °æÇèÄ¡ º¹±¸
    this->Cancel_PenalEXP(SKILL_POWER(nSkillIDX));
    // ¸®Á®·º¼Ç ½ºÅ³¿¡ µû¶ó °æÄ¡ º¹±¸·®ÀÌ Æ²·ÁÁö´Ï ÇöÀç °æÄ¡ Àü¼Û :: 2005. 03. 25
    this->Send_gsv_SETEXP(0);

    this->Set_HP(3 * this->GetCur_MaxHP() / 10);

    // ÆÄÆ¼ÀÏ °æ¿ì ÆÄÆ¼¿ø¿¡°Ô ÇÇ Àü¼Û...
    if (this->GetPARTY()) {
        this->GetPARTY()->Change_ObjectIDX((classUSER*)this);
    }
}

//-------------------------------------------------------------------------------------------------
bool
CObjAVT::Is_ALLIED(CAI_OBJ* pDestOBJ) {
    if (0 == (this->GetZONE()->GetGlobalFLAG() & ZONE_FLAG_PK_ALLOWED)) {
        if (pDestOBJ->Get_ObjTYPE() >= OBJ_AVATAR) {
            // PKÇÃ·¹±×°¡ ¼³Á¤ ¾ÈµÆÀ» °æ¿ì... ´ë»óÀÌ »ç¿ëÀÚ¸é ¹«Á¶°Ç ¾Æ±ºÀ¸·Î °£ÁÖ..
            return true;
        }
    }

    return CAI_OBJ::Is_ALLIED(pDestOBJ);
}

void
CObjAVT::SetCur_UNION(char cValue) {
    if (cValue >= 0 && cValue < MAX_UNION_COUNT) {
        this->m_BasicINFO.m_cUnion = cValue;
    }
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::SetCur_UnionPOINT(BYTE btUnionIDX, short nValue) {
    if (btUnionIDX >= 1) {
        this->m_GrowAbility.m_nUnionPOINT[btUnionIDX - 1] = nValue;
    }
}

//-------------------------------------------------------------------------------------------------
void
CObjAVT::UpdateCartGuest() {
    if (Is_CartDriver()) {
        classUSER* pGuest = g_pObjMGR->Get_UserOBJ(this->m_iLinkedCartObjIDX);
        if (!pGuest || this->GetZONE() != pGuest->GetZONE()) {
            this->m_iLinkedCartObjIDX = 0;
            this->m_iLinkedCartUsrIDX = 0;
            return;
        }

        // ¼Õ´Ô À§Ä¡ º¸Á¤~
        pGuest->m_PosCUR = this->m_PosCUR;
        this->GetZONE()->UpdateSECTOR(pGuest);
    }
}

//-------------------------------------------------------------------------------------------------
#ifdef __KCHS_BATTLECART__
bool
CObjAVT::CanDoPatSkill(short nSkillIDX) {
    if (!this->Get_RideMODE())
        return false;

    if (SKILL_AVAILBLE_STATUS(nSkillIDX) < 2) // 2º¸´Ù Å©¸é Ä«Æ® ¾Æ´Ï¸é °ø¿ë ½ºÅ³
        return false;

    if (m_RideITEM[RIDE_PART_ARMS].m_nItemNo <= 0) // ARMS ÆÄÆ®¿¡ ¹«±â°¡ ÀåÂøµÇ¾î ÀÖ¾î¾ß ÇÑ´Ù.
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------------
// btOnOff : 0 : Ä«Æ®¼ÒÈ¯ ºÒ°¡´É(ÄðÅ¸ÀÓ°ª ³¯¶ó°¨) , 1 : Ä«Æ® ¼ÒÈ¯ °¡´É
bool
CObjAVT::Send_gsv_PATSTATE_CHAGE(BYTE btOnOff, DWORD dwCoolTIME) {
    classPACKET* pCPacket = Packet_AllocNLock();
    if (!pCPacket)
        return false;

    pCPacket->m_HEADER.m_wType = GSV_PATSTATE_CHANGE;
    pCPacket->m_HEADER.m_nSize = sizeof(gsv_PATSTATE_CHANGE);
    pCPacket->m_gsv_PATSTATE_CHANGE.m_btOnOff = btOnOff;
    pCPacket->m_gsv_PATSTATE_CHANGE.m_dwCoolTIME = btOnOff ? 0 : 30 * 60 * 1000 /*PAT_COOLTIME*/;
    pCPacket->m_gsv_PATSTATE_CHANGE.m_nMaxPatHP = btOnOff ? GetCur_PatMaxHP() : 0;
    pCPacket->m_gsv_PATSTATE_CHANGE.m_wObjectIDX = this->Get_INDEX();

    this->GetZONE()->SendPacketToSectors(this, pCPacket);

    Packet_ReleaseNUnlock(pCPacket);

    if (!btOnOff) {
        this->m_btRideATTR = RIDE_ATTR_NORMAL;
        this->m_btRideMODE = 0; // ¸ÕÀú ÇÏÂ÷¸ðµå·Î..
        UpdateAbility();
        this->Send_gsv_TOGGLE(TOGGLE_TYPE_DRIVE + this->Get_MoveMODE(),
            true); // Å¬¶ó¿¡¼­´Â ÀÌ°Ô ÇÊ¿äÇÏ´Ù.. ÀÌ°Å ¾øÀ¸¸é Å¬¶ó¿¡¼­ ¼Óµµ ¸ø ¸ÂÃá´Ù.

        this->SetCMD_STOP(); // 2005.08.03 : icarus :: Á¤Áö¸í·ÉÀ¸·Î
    }

    return true;
}
#endif

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
