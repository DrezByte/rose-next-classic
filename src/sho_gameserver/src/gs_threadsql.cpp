/*
    $Header: /7HeartsOnline/Server/SHO_GS/Sho_gs_lib/GS_ThreadSQL.cpp 117   05-08-25 5:24p Icarus $
*/
#include "stdafx.h"

#include "CSLList.h"
#include "CThreadGUILD.h"
#include "GS_ListUSER.h"
#include "GS_SocketLSV.h"
#include "GS_ThreadSQL.h"
#include "LIB_gsMAIN.h"
#include "ZoneLIST.h"
#include "classTIME.h"

#include "rose/database/database.h"

#include "nlohmann/json.hpp"

using namespace Rose::Database;

using json = nlohmann::json;

enum BANKTBL_COL_IDX { BANKTBL_ACCOUNT = 0, BANKTBL_ITEMS, BANKTBL_REWARD, BANKTLB_PASSWORD };

GS_CThreadSQL::GS_CThreadSQL(): CSqlTHREAD(true), m_csUserLIST(4000), m_TmpSTR(512) {
    COMPILE_TIME_ASSERT(4 == sizeof(void*));

    COMPILE_TIME_ASSERT(MAX_RIDING_PART == 4);

    COMPILE_TIME_ASSERT(sizeof(tagBasicETC) <= 96); // db field size
    COMPILE_TIME_ASSERT(sizeof(tagBasicINFO) <= 32);
    COMPILE_TIME_ASSERT(sizeof(tagBasicAbility) <= 48);
    COMPILE_TIME_ASSERT(sizeof(tagGrowAbility) <= 384);
    COMPILE_TIME_ASSERT(sizeof(tagSkillAbility) <= 240);
    COMPILE_TIME_ASSERT(sizeof(CInventory) <= 2048);
    COMPILE_TIME_ASSERT(sizeof(tagBankData) <= 2576);
    COMPILE_TIME_ASSERT(sizeof(tagQuestData) < 1000);

    COMPILE_TIME_ASSERT(sizeof(tagITEM) == (6 + 8));
    COMPILE_TIME_ASSERT(sizeof(CInventory) == (139 * 14 + 8)); // 1954

    COMPILE_TIME_ASSERT(sizeof(m_sBE) == sizeof(tagBasicETC));
    COMPILE_TIME_ASSERT(sizeof(tagHotICON) == sizeof(WORD));

    m_bWaiting = false;
    m_HotICON.Init();
    m_sEmptyBANK.Init();
}

__fastcall GS_CThreadSQL::~GS_CThreadSQL() {}

bool
GS_CThreadSQL::Add_SqlPacketWithACCOUNT(classUSER* pUSER, t_PACKET* pPacket) {
    return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX,
        pUSER->Get_ACCOUNT(),
        (BYTE*)pPacket,
        pPacket->m_HEADER.m_nSize);
}
bool
GS_CThreadSQL::Add_SqlPacketWithAVATAR(classUSER* pUSER, t_PACKET* pPacket) {
    return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX,
        pUSER->Get_NAME(),
        (BYTE*)pPacket,
        pPacket->m_HEADER.m_nSize);
}

bool
GS_CThreadSQL::IO_ZoneDATA(CZoneTHREAD* pZONE, bool bSave) {
    sql_ZONE_DATA* pPacket;

    if (bSave) {
        // Á¸ÀÌ »Ç°³Áö¸é¼­ È£ÃâµÉ°æ¿ì Ã³¸®½Ã Á¸ÀÇ µ¥ÀÌÅ¸¸¦ ÂüÁ¶ÇÏ¸é ¾ÈµÊ...
        // ÆÐÅ¶¿¡ º¹»çµÈ »ý¼º...
        pPacket = (sql_ZONE_DATA*)new BYTE[sizeof(sql_ZONE_DATA) + sizeof(tagECONOMY)];
        pPacket->m_nDataSIZE = sizeof(tagECONOMY);
        ::CopyMemory(pPacket->m_btZoneDATA, pZONE->m_Economy.m_pEconomy, pPacket->m_nDataSIZE);
        pPacket->m_btDataTYPE = SQL_ZONE_DATA_ECONOMY_SAVE;
    } else {
        pPacket = new sql_ZONE_DATA;
        pPacket->m_nDataSIZE = 0;
        pPacket->m_btDataTYPE = SQL_ZONE_DATA_ECONOMY_LOAD;
    }
    pPacket->m_wType = SQL_ZONE_DATA;
    pPacket->m_nSize = sizeof(sql_ZONE_DATA) + pPacket->m_nDataSIZE;

    CSqlTHREAD::Add_SqlPACKET(pZONE->Get_ZoneNO(),
        pZONE->Get_NAME(),
        (BYTE*)pPacket,
        pPacket->m_nSize);

    SAFE_DELETE_ARRAY(pPacket);
    return true;
}

bool
GS_CThreadSQL::IO_NpcObjDATA(CObjNPC* pObjNPC, bool bSave) {
    sql_ZONE_DATA* pPacket;

    pPacket = (sql_ZONE_DATA*)new BYTE[sizeof(sql_ZONE_DATA) + sizeof(tagObjVAR)];

    pPacket->m_nDataSIZE = sizeof(tagObjVAR);

    pPacket->m_wType = SQL_ZONE_DATA;
    pPacket->m_nSize = sizeof(sql_ZONE_DATA) + pPacket->m_nDataSIZE;

    pPacket->m_btDataTYPE = (bSave) ? SQL_ZONE_DATA_NPCOBJ_SAVE : SQL_ZONE_DATA_NPCOBJ_LOAD;
    ::CopyMemory(pPacket->m_btZoneDATA, pObjNPC->m_pVAR, pPacket->m_nDataSIZE);

    m_TmpSTR.Printf("%s_NPC_%s",
        CLIB_GameSRV::GetInstance()->config.gameserver.server_name.c_str(),
        pObjNPC->Get_NAME());

    CSqlTHREAD::Add_SqlPACKET(pObjNPC->Get_CharNO(),
        m_TmpSTR.Get(),
        (BYTE*)pPacket,
        pPacket->m_nSize);

    SAFE_DELETE_ARRAY(pPacket);
    return true;
}
bool
GS_CThreadSQL::IO_EventObjDATA(CObjEVENT* pObjEVENT, bool bSave) {
    sql_ZONE_DATA* pPacket;

    pPacket = (sql_ZONE_DATA*)new BYTE[sizeof(sql_ZONE_DATA) + sizeof(tagObjVAR)];

    pPacket->m_nDataSIZE = sizeof(tagObjVAR);

    pPacket->m_wType = SQL_ZONE_DATA;
    pPacket->m_nSize = sizeof(sql_ZONE_DATA) + pPacket->m_nDataSIZE;

    pPacket->m_btDataTYPE = (bSave) ? SQL_ZONE_DATA_EVENTOBJ_SAVE : SQL_ZONE_DATA_EVENTOBJ_LOAD;
    ::CopyMemory(pPacket->m_btZoneDATA, pObjEVENT->m_pVAR, pPacket->m_nDataSIZE);

    m_TmpSTR.Printf("%s_EVT_%s",
        CLIB_GameSRV::GetInstance()->config.gameserver.server_name.c_str(),
        pObjEVENT->Get_NAME());

    CSqlTHREAD::Add_SqlPACKET(pObjEVENT->Get_ID(),
        m_TmpSTR.Get(),
        (BYTE*)pPacket,
        pPacket->m_nSize);
    SAFE_DELETE_ARRAY(pPacket);
    return true;
}

bool
GS_CThreadSQL::Add_BackUpUSER(classUSER* pUSER, BYTE btLogOutMODE) {
    if (pUSER->m_btLogOutMODE != btLogOutMODE) {
        // ÀúÀåÇÒ ÇÊ¿ä ¾ø´Ù.
        return true;
    }

    m_csUserLIST.Lock();
    {
        classDLLNODE<tagSqlUSER>* pNewNODE;
        pNewNODE = new classDLLNODE<tagSqlUSER>;

        // bLogOutÀÌ ¾Æ´Ñ °æ¿ì°¡ ÀÌ¹Ì µî·Ï µÇ¾î ÀÖ´Â °æ¿ì°¡ ÀÖ¾î
        // pUSER->m_bLogOutÀ¸·Î ÆÇ´ÜÇÏ¸é ¾ÈµÈ´Ù.
        pNewNODE->DATA.m_btLogOutMODE = btLogOutMODE;
        pNewNODE->DATA.m_pUSER = pUSER;
        m_AddUserLIST.AppendNode(pNewNODE);
    }
    m_csUserLIST.Unlock();

    m_pEVENT->SetEvent();

    return true;
}

//-------------------------------------------------------------------------------------------------
#define BEGINNER_ZONE_LEVEL 5
#define BEGINNER_ZONE_NO 20

bool
GS_CThreadSQL::UpdateUserRECORD(classUSER* pUSER) {
    // update character DB !!!
    // "UPDATE tblGS_AVATAR SET nZoneNO=xxx, binBasicI=xx, binBasicA=xx WHERE txtNAME=xxx;"
    m_sBE.m_btCharRACE = (BYTE)pUSER->m_nCharRACE;

    if (pUSER->Get_HP() <= 0) {
        // Á¸¹øÈ£°¡ ¾øÀ¸¸é ???
        // Á×¾úÀ¸¸é ÀúÀåµÈ ºÎÈ°Àå¼Ò¿¡¼­ ...
        if (pUSER->m_nZoneNO == BEGINNER_ZONE_NO) {
            m_sBE.m_nZoneNO = BEGINNER_ZONE_NO;
            m_sBE.m_PosSTART = pUSER->m_PosCUR;
        } else {
            m_sBE.m_nZoneNO = pUSER->m_nReviveZoneNO;
            m_sBE.m_PosSTART = pUSER->m_PosREVIVE;
        }
    } else {
        m_sBE.m_nZoneNO = pUSER->m_nZoneNO;
        m_sBE.m_PosSTART = pUSER->m_PosCUR;
    }

    // ºÎÈ° Àå¼Ò...
    m_sBE.m_nReviveZoneNO = pUSER->m_nReviveZoneNO;
    m_sBE.m_PosREVIVE = pUSER->m_PosREVIVE;
    m_sBE.m_btCharSlotNO = pUSER->m_btPlatinumCHAR;

    if (m_sBE.m_nZoneNO < 0 || m_sBE.m_nReviveZoneNO < 0) {
        g_LOG.CS_ODS(0xffff,
            "**** Invalid ZoneNO [ %s ] Race: %d, Zone: %d ReviveZone: %d",
            pUSER->Get_NAME(),
            m_sBE.m_btCharRACE,
            m_sBE.m_nZoneNO,
            m_sBE.m_nReviveZoneNO);
    }

    ::CopyMemory(m_sBE.m_PartITEM, pUSER->m_PartITEM, sizeof(tagPartITEM) * MAX_BODY_PART);
    ::CopyMemory(m_sBE.m_RideITEM, pUSER->m_RideITEM, sizeof(tagPartITEM) * MAX_RIDING_PART);

    pUSER->m_BasicINFO.m_cFaceIDX = (char)pUSER->m_PartITEM[BODY_PART_FACE].m_nItemNo;
    pUSER->m_BasicINFO.m_cHairIDX = (char)pUSER->m_PartITEM[BODY_PART_HAIR].m_nItemNo;

    // Update character data
    std::string query("UPDATE character SET basic_etc=?, basic_info=?, basic_ability=?, "
                      "grow_ability=?, skill_ability=?, inventory=?, quest_data=?, hot_icon=?, "
                      "wish_list=?, level=?, money=?, job_id=?, "
                      "party_id=?, item_serial=?, gender_id=?, face_id=?, hair_id=?, "
                      "map_id=?, respawn_x=?, respawn_y=?, "
                      "town_respawn_id=?, town_respawn_x=?, town_respawn_y=? "
                      "WHERE name=?");

    this->db->bind_binary(1, (uint8_t*)&this->m_sBE, sizeof(tagBasicETC));
    this->db->bind_binary(2, (uint8_t*)&pUSER->m_BasicINFO, sizeof(tagBasicINFO));
    this->db->bind_binary(3, (uint8_t*)&pUSER->m_BasicAbility, sizeof(tagBasicAbility));
    this->db->bind_binary(4, (uint8_t*)&pUSER->m_GrowAbility, sizeof(tagGrowAbility));
    this->db->bind_binary(5, (uint8_t*)&pUSER->m_Skills, sizeof(tagSkillAbility));
    this->db->bind_binary(6, (uint8_t*)&pUSER->m_Inventory, sizeof(CInventory));
    this->db->bind_binary(7, (uint8_t*)&pUSER->m_Quests, sizeof(tagQuestData));
    this->db->bind_binary(8, (uint8_t*)&pUSER->m_HotICONS, sizeof(CHotICONS));
    this->db->bind_binary(9, (uint8_t*)&pUSER->m_WishLIST, sizeof(tagWishLIST));
    this->db->bind_int16(10, pUSER->m_GrowAbility.m_nLevel);
    this->db->bind_int64(11, pUSER->GetCur_MONEY());
    this->db->bind_int16(12, pUSER->m_BasicINFO.m_nClass);
    this->db->bind_int32(13, pUSER->GetPARTY() ? pUSER->m_pPartyBUFF->m_wPartyWSID : 0);
    this->db->bind_int32(14, pUSER->m_dwItemSN);
    this->db->bind_int16(15, this->m_sBE.m_btCharRACE);
    this->db->bind_int16(16, pUSER->m_BasicINFO.m_cFaceIDX);
    this->db->bind_int16(17, pUSER->m_BasicINFO.m_cHairIDX);
    this->db->bind_float(18, this->m_sBE.m_nZoneNO);
    this->db->bind_float(19, this->m_sBE.m_PosSTART.x);
    this->db->bind_float(20, this->m_sBE.m_PosSTART.y);
    this->db->bind_float(21, this->m_sBE.m_nReviveZoneNO);
    this->db->bind_float(22, this->m_sBE.m_PosREVIVE.x);
    this->db->bind_float(23, this->m_sBE.m_PosREVIVE.y);
    this->db->bind_string(24, pUSER->Get_RNAME(), pUSER->m_RName.Length());

    if (!this->db->execute(query)) {
        LOG_ERROR("failed to update charactacter. Character: (%s)", pUSER->Get_RNAME());
        for (const std::string& msg: this->db->get_error_messages()) {
            LOG_WARN(msg.c_str());
        }
        return false;
    }

    if (BANK_CHANGED != pUSER->m_btBankData) {
        return true;
    }

    // Update bank data
    this->db->bind_binary(1, (uint8_t*)&pUSER->m_Bank, sizeof(tagBankData));
    this->db->bind_string(2, pUSER->Get_ACCOUNT(), pUSER->Get_AccountLEN());

    if (!this->db->execute("UPDATE tblGS_BANK SET blobITEMS=? WHERE txtACCOUNT=?")) {
        LOG_ERROR("Failed to update storage. Account: %s", pUSER->Get_NAME());
        for (const std::string& msg: this->db->get_error_messages()) {
            LOG_WARN(msg.c_str());
        }
        return false;
    }

    return true;
}

void
GS_CThreadSQL::Execute() {
    //	this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 1 point above the priority
    // class
    this->SetPriority(THREAD_PRIORITY_HIGHEST); // Priority 2 point above the priority class

    CDLList<tagQueryDATA>::tagNODE* pSqlNODE;
    classDLLNODE<tagSqlUSER>* pUsrNODE;

    LOG_DEBUG("GS_CThreadSQL::Execute() ThreadID: %d(0x%x)", this->ThreadID, this->ThreadID);

    while (TRUE) {
        if (!this->Terminated) {
            m_bWaiting = true;
            m_pEVENT->WaitFor(INFINITE);
            m_bWaiting = false;
        } else {
            int iCnt1, iCnt2;
            this->m_CS.Lock();
            iCnt1 = m_AddPACKET.GetNodeCount();
            this->m_CS.Unlock();

            this->m_csUserLIST.Lock();
            iCnt2 = m_AddUserLIST.GetNodeCount();
            this->m_csUserLIST.Unlock();
            if (iCnt1 <= 0 && iCnt2 <= 0) {
                m_bWaiting = true;
                break;
            }
        }

        // °ÔÀÓ ·Î±× ÀúÀå..
        this->Proc_QuerySTRING();

        this->m_CS.Lock();
        m_RunPACKET.AppendNodeList(&m_AddPACKET);
        m_AddPACKET.Init();
        m_pEVENT->ResetEvent();
        this->m_CS.Unlock();

        m_dwCurTIME = classTIME::GetCurrentAbsSecond();
        for (pSqlNODE = m_RunPACKET.GetHeadNode(); pSqlNODE;) {
            if (Run_SqlPACKET(&pSqlNODE->m_VALUE))
                pSqlNODE = this->Del_SqlPACKET(pSqlNODE);
            else
                pSqlNODE = m_RunPACKET.GetNextNode(pSqlNODE);
        }

        m_csUserLIST.Lock();
        m_RunUserLIST.AppendNodeList(&m_AddUserLIST);
        m_AddUserLIST.Init();
        m_csUserLIST.Unlock();

        while (pUsrNODE = m_RunUserLIST.GetHeadNode()) {
            if (pUsrNODE->DATA.m_pUSER->Get_NAME()) {
                this->UpdateUserRECORD(pUsrNODE->DATA.m_pUSER);
            }

            if (pUsrNODE->DATA.m_btLogOutMODE) {
                if (pUsrNODE->DATA.m_pUSER->Get_ACCOUNT()) {
                    switch (pUsrNODE->DATA.m_btLogOutMODE) {
                        case LOGOUT_MODE_CHARLIST:
                            g_pSockLSV->Send_gsv_CHANGE_CHAR(pUsrNODE->DATA.m_pUSER);
                            break;

                        default:
                            if (pUsrNODE->DATA.m_pUSER->GetPARTY()) {
                                // ºñÁ¤»ó Á¾·á...
                                pUsrNODE->DATA.m_pUSER->m_pPartyBUFF
                                    ->Lock(); // 2004. 9. 19 »õº® 7½Ã10ºÐ :: »ª¼·..
                                pUsrNODE->DATA.m_pUSER->m_pPartyBUFF->OnDisconnect(
                                    pUsrNODE->DATA.m_pUSER);
                                pUsrNODE->DATA.m_pUSER->m_pPartyBUFF->Unlock();
                            }

                            g_pSockLSV->Send_zws_SUB_ACCOUNT(pUsrNODE->DATA.m_pUSER->m_dwWSID,
                                pUsrNODE->DATA.m_pUSER->Get_ACCOUNT());
                    }
                }
                g_pUserLIST->FreeClientSOCKET(pUsrNODE->DATA.m_pUSER);
            }

            m_RunUserLIST.DeleteNFree(pUsrNODE);
        }
    }

    int iCnt = m_AddPACKET.GetNodeCount();
    assert(iCnt == 0);

    LOG_DEBUG("<  < << GS_CThreadSQL::Execute() ThreadID: %d(0x%x)",
        this->ThreadID,
        this->ThreadID);
}

bool
GS_CThreadSQL::Run_SqlPACKET(tagQueryDATA* pSqlPACKET) {
    switch (pSqlPACKET->m_pPacket->m_wType) {
        case CLI_SELECT_CHAR:
            Proc_cli_SELECT_CHAR(pSqlPACKET);
            break;

        case CLI_BANK_LIST_REQ:
            Proc_cli_BANK_LIST_REQ(pSqlPACKET);
            break;

        case SQL_ZONE_DATA: {
            sql_ZONE_DATA* pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
            switch (pSqlZONE->m_btDataTYPE) {
                case SQL_ZONE_DATA_NPCOBJ_LOAD:
                case SQL_ZONE_DATA_EVENTOBJ_LOAD:
                    Proc_LOAD_OBJVAR(pSqlPACKET);
                    break;

                case SQL_ZONE_DATA_ECONOMY_LOAD: // Á¸ ¾²·¹µå°¡ ¾Æ·¡ ÇÔ¼ö¾È¿¡¼­ ½ÃÀÛµÈ´Ù.
                    Proc_LOAD_ZONE_DATA(pSqlPACKET->m_iTAG);
                    break;

                case SQL_ZONE_DATA_ECONOMY_SAVE:
                    Proc_SAVE_ZONE_DATA(pSqlPACKET->m_iTAG, pSqlZONE);
                    break;

                case SQL_ZONE_DATA_NPCOBJ_SAVE:
                case SQL_ZONE_DATA_EVENTOBJ_SAVE:
                    Proc_SAVE_OBJVAR(pSqlPACKET);
                    break;
            }
            break;
        }

        default:
            g_LOG.CS_ODS(0xffff,
                "Undefined sql packet Type: %x, Size: %d \n",
                pSqlPACKET->m_pPacket->m_wType,
                pSqlPACKET->m_pPacket->m_nSize);
    }

    return true;
}

bool
GS_CThreadSQL::Proc_cli_SELECT_CHAR(tagQueryDATA* pSqlPACKET) {
    t_PACKET* pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

#define MAX_AVATAR_NAME 20
    short nOffset = sizeof(cli_SELECT_CHAR), nOutStrLen;
    char* pCharName = Packet_GetStringPtr(pPacket, nOffset, nOutStrLen);
    if (!pCharName || nOutStrLen < 4) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: CharName == NULL \n");
        return false;
    }

    if (nOutStrLen > MAX_AVATAR_NAME) {
        g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_SELECT_CHAR:: CharName == '%s'\n", pCharName);
        return false;
    }

    std::string query =
        "SELECT id, basic_etc, basic_info, basic_ability, grow_ability, skill_ability, quest_data, "
        "inventory, wish_list, hot_icon, party_id, item_serial FROM character WHERE name=\'%s\'";

    // "SELECT * FROM tblGS_AVATAR WHERE txtNAME='xxxx';"
    if (!this->db->QuerySQL((char*)query.c_str(), pCharName)) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }

    if (!this->db->GetNextRECORD()) {
        // 케릭터 없다.
        g_LOG.CS_ODS(LOG_NORMAL, "Char[ %s ] not found ...\n", pCharName);
        return false;
    }

    bool bResult;
    classUSER* pUSER = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (pUSER) {
        classPACKET* pCPacket = Packet_AllocNLock();
        if (pCPacket) {
            pUSER->m_dwDBID = this->db->GetInteger(0);

            pUSER->Set_NAME(pCharName);
            pUSER->Set_RNAME(pCharName);
            g_pUserLIST->Add_CHAR(pUSER);

            tagBasicETC* pBE;
            pBE = (tagBasicETC*)this->db->GetDataPTR(1);

            pUSER->m_nCharRACE = pBE->m_btCharRACE;
            pUSER->m_btPlatinumCHAR = pBE->m_btCharSlotNO;

            CZoneTHREAD* pZONE = g_pZoneLIST->GetZONE(pBE->m_nZoneNO);
            if (pZONE) {
                pUSER->m_nZoneNO = pBE->m_nZoneNO;

                if (pBE->m_PosSTART.x == 0.f || pBE->m_PosSTART.y == 0.f) {
                    /// 샷다운시 존이 모두 삭제되어 가까운 부활 위치를 못찾아 0으로 셋팅磯
                    /// ... 2.26
                    pBE->m_PosSTART = pZONE->Get_StartRevivePOS();
                } else {
                    // 기본 현재 위치의 가장 가까운 부활 장소로...
                    pBE->m_PosSTART = pZONE->Get_RevivePOS(pBE->m_PosSTART);
                    pBE->m_PosSTART.x += (RANDOM(1001) - 500); // 랜덤 5미터..
                    pBE->m_PosSTART.y += (RANDOM(1001) - 500);
                }

                pUSER->m_PosCUR = pBE->m_PosSTART;
            } else {
                // 혹시 버그로 인해 존이 삭제 되었을경우..
                pUSER->m_nZoneNO = AVATAR_ZONE(pBE->m_btCharRACE);
                pZONE = g_pZoneLIST->GetZONE(pUSER->m_nZoneNO);
                if (!pZONE) // 존이 없을 경우 디폴트로 존 1을...
                {
                    pUSER->m_nZoneNO = 1;
                    pZONE = g_pZoneLIST->GetZONE(pUSER->m_nZoneNO);
                }
                pUSER->m_PosCUR = pZONE->Get_StartPOS();
            }

            /// 2005. 08. 24 :: 임시로 클랜필드에서 접속할경우 2번존(주논폴리스) 부활위치로 강제
            /// 이동
            if (pUSER->m_nZoneNO >= 11 && pUSER->m_nZoneNO <= 13) {
                int iJunonPolice = 2;
                CZoneTHREAD* pZone2 = g_pZoneLIST->GetZONE(iJunonPolice);
                pUSER->m_nZoneNO = pZone2->Get_ZoneNO();
                pUSER->m_PosCUR = pZone2->Get_StartRevivePOS();
            } // if ( pUSER->m_nZoneNO >= 11 && pUSER->m_nZoneNO <= 13 )

            // 일정 주기로 사용자 정보 저장하기 위해서..
            pUSER->m_dwBackUpTIME = pZONE->GetCurrentTIME();

            pUSER->m_nReviveZoneNO = pBE->m_nReviveZoneNO;
            pUSER->m_PosREVIVE = pBE->m_PosREVIVE;

            ::CopyMemory(pUSER->m_PartITEM, pBE->m_PartITEM, sizeof(tagPartITEM) * MAX_BODY_PART);

#ifdef __KCHS_BATTLECART__ // __OLD_DATA_COMPATIBLE__
            if (nDataVER < DATA_VER_2) {
                ::CopyMemory(pUSER->m_RideITEM,
                    pBE->m_RideITEM,
                    sizeof(tagPartITEM) * (MAX_RIDING_PART - 1));
                ZeroMemory(&pUSER->m_RideITEM[RIDE_PART_ARMS], sizeof(tagPartITEM));
            } else
                ::CopyMemory(pUSER->m_RideITEM,
                    pBE->m_RideITEM,
                    sizeof(tagPartITEM) * MAX_RIDING_PART);
#else
            ::CopyMemory(pUSER->m_RideITEM, pBE->m_RideITEM, sizeof(tagPartITEM) * MAX_RIDING_PART);
#endif

            BYTE* pDATA;
            pDATA = this->db->GetDataPTR(2);
            ::CopyMemory(&pUSER->m_BasicINFO, pDATA, sizeof(tagBasicINFO));

            pDATA = this->db->GetDataPTR(3);
            ::CopyMemory(&pUSER->m_BasicAbility, pDATA, sizeof(tagBasicAbility));

            pDATA = this->db->GetDataPTR(4);
            ::CopyMemory(&pUSER->m_GrowAbility, pDATA, sizeof(tagGrowAbility));
#ifdef __KCHS_BATTLECART__
            if (pUSER->GetCur_PatHP() > PAT_COOLTIME) // 이렇다는 얘기는 0 - 1 한 잘못 된 데이터
                pUSER->SetCur_PatHP(0);
#endif
            // skill
            pDATA = this->db->GetDataPTR(5);
            ::CopyMemory(&pUSER->m_Skills, pDATA, sizeof(tagSkillAbility));

            // quest
            pDATA = this->db->GetDataPTR(6);
            if (pDATA) {
                ::CopyMemory(&pUSER->m_Quests, pDATA, sizeof(tagQuestData));
                pUSER->m_Quests.CheckExpiredTIME();
            } else
                pUSER->m_Quests.Init();

            // inventory...
            pDATA = this->db->GetDataPTR(7);
            ::CopyMemory(&pUSER->m_Inventory, pDATA, sizeof(CInventory));

            // wish list
            pDATA = this->db->GetDataPTR(8);
            if (pDATA)
                ::CopyMemory(&pUSER->m_WishLIST, pDATA, sizeof(tagWishLIST));
            else
                pUSER->m_WishLIST.Init();

            pUSER->m_i64StartMoney = pUSER->GetCur_MONEY(); // 돈 변화량을 체크하기 위해서...

            // 잘못된 아이템일 경우 삭제...
            tagITEM* pITEM;
            short nI;
            for (nI = EQUIP_IDX_NULL + 1; nI < MAX_EQUIP_IDX; nI++) {
                pITEM = &pUSER->m_Inventory.m_ItemLIST[nI];
                if (!pITEM->GetHEADER())
                    continue;
                if (!pITEM->IsEquipITEM() || !pITEM->IsValidITEM()) {
                    pITEM->Clear();
                    continue;
                }
            }

            for (nI = EQUIP_IDX_FACE_ITEM; nI < INVENTORY_TOTAL_SIZE; nI++) {
                pITEM = &pUSER->m_Inventory.m_ItemLIST[nI];
                if (!pITEM->GetHEADER())
                    continue;

                if (!pITEM->GetTYPE() || pITEM->GetTYPE() > ITEM_TYPE_RIDE_PART) {
                    pITEM->Clear();
                    continue;
                }

                // 아이템 해킹 케릭인지 조사...
                if (pITEM->IsEnableDupCNT()) {
                    if (pITEM->GetQuantity() > MAX_DUP_ITEM_QUANTITY) {
                        pUSER->m_Inventory.m_i64Money = 0;
                        pITEM->Clear();
                        continue;
                    }
                } else if (pITEM->GetOption()) {
                    if (pITEM->GetOption() >= g_TblGEMITEM.m_nDataCnt) {
                        // 창고로 이동시 잘못됐던 버그 아이템이다...
                        pITEM->m_nGEM_OP = 0;
                    }
                }
                if (pITEM->GetItemNO() >= g_pTblSTBs[pITEM->GetTYPE()]->m_nDataCnt) {
                    pITEM->Clear();
                    continue;
                }
            }

            pDATA = this->db->GetDataPTR(9);
            if (pDATA)
                ::CopyMemory(&pUSER->m_HotICONS, pDATA, sizeof(CHotICONS));
            else
                pUSER->m_HotICONS.Init();

            COMPILE_TIME_ASSERT(sizeof(CHotICONS) == sizeof(tagHotICON) * MAX_HOT_ICONS);

            DWORD dwPassMIN = 0;
            DWORD dwDisMIN = 0;

            if (pUSER->GetCur_STAMINA() > MAX_STAMINA)
                pUSER->SetCur_STAMINA(MAX_STAMINA);

            // 스킬 데이타에서 패시브 스킬에서 얻은 능력치들을 정리...
            pUSER->InitPassiveSkill();

            // 패킷을 보내기 전에 능력치를 계산해야 할듯...
            // 아래 함수 실행중 클라이언트에서 패킷이 도착해서
            // 진행되어 질수 있다... 운이 나쁘면 ㅡ,.ㅡ;
            pUSER->UpdateAbility();
            if (pUSER->Get_HP() <= 0) {
                // 죽었다면 30% hp
                pUSER->Set_HP(3 * pUSER->Get_MaxHP() / 10);
            } else if (pUSER->Get_HP() > pUSER->Get_MaxHP())
                pUSER->Set_HP(pUSER->Get_MaxHP());

            pUSER->Set_ShotITEM(); // GS_CThreadSQL::Proc_cli_SELECT_CHAR

            if (pPacket->m_cli_SELECT_CHAR.m_btCharNO) {
                // 클라이언트에게 정보를 보내야 하는가 ???
                pCPacket->m_HEADER.m_wType = GSV_SELECT_CHAR;
                pCPacket->m_HEADER.m_nSize = sizeof(gsv_SELECT_CHAR);

                pCPacket->m_gsv_SELECT_CHAR.m_btCharRACE = pBE->m_btCharRACE;
                pCPacket->m_gsv_SELECT_CHAR.m_nZoneNO = pUSER->m_nZoneNO;
                pCPacket->m_gsv_SELECT_CHAR.m_PosSTART = pUSER->m_PosCUR;
                pCPacket->m_gsv_SELECT_CHAR.m_nReviveZoneNO = pUSER->m_nReviveZoneNO;
                pCPacket->m_gsv_SELECT_CHAR.m_dwUniqueTAG = pUSER->m_dwDBID;

                ::CopyMemory(pCPacket->m_gsv_SELECT_CHAR.m_PartITEM,
                    pUSER->m_PartITEM,
                    sizeof(tagPartITEM) * MAX_BODY_PART);

                ::CopyMemory(&pCPacket->m_gsv_SELECT_CHAR.m_BasicINFO,
                    &pUSER->m_BasicINFO,
                    sizeof(tagBasicINFO));
                ::CopyMemory(&pCPacket->m_gsv_SELECT_CHAR.m_BasicAbility,
                    &pUSER->m_BasicAbility,
                    sizeof(tagBasicAbility));

#pragma COMPILE_TIME_MSG("********** 여기서는 tagGrowAbility도 크기가 바뀌었다 ...")
                ::CopyMemory(&pCPacket->m_gsv_SELECT_CHAR.m_GrowAbility,
                    &pUSER->m_GrowAbility,
                    sizeof(tagGrowAbility));

                ::CopyMemory(&pCPacket->m_gsv_SELECT_CHAR.m_Skill,
                    &pUSER->m_Skills,
                    sizeof(tagSkillAbility));
                ::CopyMemory(&pCPacket->m_gsv_SELECT_CHAR.m_HotICONS,
                    &pUSER->m_HotICONS,
                    sizeof(CHotICONS));
                pCPacket->AppendString(pUSER->Get_NAME());
                COMPILE_TIME_ASSERT(sizeof(gsv_SELECT_CHAR) < MAX_PACKET_SIZE);
                pUSER->SendPacket(pCPacket);

                pUSER->Send_gsv_INVENTORYnQUEST_DATA();

                // 파티번호가 있냐...
                unsigned int iPartyIDX = (unsigned int)this->db->GetInteger(10);
                if (iPartyIDX
                    && dwPassMIN < 7) { // 튕기고 5분안에 재접이면(서번6분), 클라이언트에서 파장이
                                        // 5분되면 자동 강퇴요청 해야됨
                    g_pPartyBUFF->OnConnect(iPartyIDX, pUSER);
                }

                // 아이템 시리얼번호 초기값 갱신...
                DWORD dwDBItemSN = this->db->GetInteger(11);
                if (this->m_dwCurTIME >= dwDBItemSN)
                    pUSER->m_dwItemSN = this->m_dwCurTIME;
                else
                    pUSER->m_dwItemSN = dwDBItemSN;
            } else {
                // 존에 참가하라는 패킷 전송...
                pUSER->m_bRunMODE = pPacket->m_cli_SELECT_CHAR.m_btRunMODE;
                pUSER->m_btRideMODE = pPacket->m_cli_SELECT_CHAR.m_btRideMODE;
                pUSER->Send_gsv_TELEPORT_REPLY(pUSER->m_PosCUR, pUSER->m_nZoneNO);
            }

            Packet_ReleaseNUnlock(pCPacket);

            pUSER->m_Bank.Init();
            pUSER->m_btBankData = BANK_UNLOADED;

            pUSER->m_dwLoginTIME = this->m_dwCurTIME;

            bResult = true;
        } else {
            // 패킷 할당 실패...
            bResult = false;
        }
    } else {
        // 접속 끊겼다.
        bResult = false;
    }
    return bResult;
}

#define MAX_BEGINNER_POS 5
tPOINTF s_BeginnerPOS[MAX_BEGINNER_POS] = {{530500, 539500},
    {568700, 520222},
    {568000, 473400},
    {512100, 469900},
    {499900, 515600}};

#define BEGINNER_ZONE 20
#define ADVENTURE_ZONE 22

bool
GS_CThreadSQL::Proc_cli_BANK_LIST_REQ(tagQueryDATA* pSqlPACKET) {
    t_PACKET* pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

    /*
    if (BANK_REQ_CHANGE_PASSWORD == pPacket->m_cli_BANK_LIST_REQ.m_btREQ) {
        // Ã¢°í ºñ¹ø º¯°æ...
        if (pPacket->m_HEADER.m_nSize > sizeof(cli_BANK_LIST_REQ)) {
            short nOffset = sizeof(cli_BANK_LIST_REQ);
            char* szPassWD = Packet_GetStringPtr(pPacket, nOffset);
            if (szPassWD
                && this->db->ExecSQL(
                       "UPDATE tblGS_AVATAR SET txtPASSWORD=\'%s\' WHERE txtACCOUNT=\'%s\'",
                       szPassWD,
                       pSqlPACKET->m_Name.Get())
                    < 1) {
                classUSER* pUSER = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
                if (pUSER) {
                    pUSER->Send_gsv_BANK_LIST_REPLY(BANK_REPLY_CHANGED_PASSWORD);
                }
            }
        } else {
            if (this->db->ExecSQL(
                    "UPDATE tblGS_AVATAR SET txtPASSWORD=NULL WHERE txtACCOUNT=\'%s\'",
                    pSqlPACKET->m_Name.Get())
                < 1) {
                classUSER* pUSER = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
                if (pUSER) {
                    pUSER->Send_gsv_BANK_LIST_REPLY(BANK_REPLY_CLEARED_PASSWORD);
                }
            }
        }
        return true;
    }
    */

    if (!this->db->QuerySQL("{call gs_SelectBANK(\'%s\')}", pSqlPACKET->m_Name.Get())) {
        /*
            this->db->MakeQuery( "SELECT * FROM tblGS_BANK WHERE txtACCOUNT=",
                                    MQ_PARAM_STR, pSqlPACKET->m_Name.Get(),
                                    MQ_PARAM_END);
            if ( !this->db->QuerySQLBuffer() ) {
        */
        // ???
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }

    if (!this->db->GetNextRECORD()) {
        // LogString(LOG_NORMAL, "Create [ %s ] bank...\n", pSqlPACKET->m_Name.Get() );

        this->db->BindPARAM(1, (BYTE*)&this->m_sEmptyBANK, sizeof(tagBankData));

        // "INSERT tblGS_AVATAR (Account, Name, ZoneNO, BasicINFO) VALUSE( xxx, xxx, xxx, xxx );"
        this->db->MakeQuery("INSERT tblGS_BANK (txtACCOUNT, blobITEMS) VALUES(",
            MQ_PARAM_STR,
            pSqlPACKET->m_Name.Get(),
            MQ_PARAM_ADDSTR,
            ",",
            MQ_PARAM_BINDIDX,
            1,
            MQ_PARAM_ADDSTR,
            ");",
            MQ_PARAM_END);
        if (this->db->ExecSQLBuffer() < 1) {
            // ¸¸µé±â ½ÇÆÐ !!!
            g_LOG.CS_ODS(LOG_NORMAL,
                "SQL Exec ERROR:: INSERT Bank:%s : %s \n",
                pSqlPACKET->m_Name.Get(),
                this->db->GetERROR());
            return true;
        }

        classUSER* pUSER = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
        if (!pUSER || !pUSER->Get_ACCOUNT())
            return false;

        return pUSER->Send_gsv_BANK_ITEM_LIST(true);
    }

    classUSER* pUSER = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (!pUSER || !pUSER->Get_ACCOUNT())
        return false;

    BYTE* pDATA;
    pDATA = this->db->GetDataPTR(BANKTBL_ITEMS);
    if (pDATA) {
        ::CopyMemory(&pUSER->m_Bank, pDATA, sizeof(tagBankData));

        char* szPassword;
        szPassword = this->db->GetStrPTR(BANKTLB_PASSWORD);
        if (szPassword) {
            // ºñ¹Ð¹øÈ£ Ã¼Å©..
            pUSER->m_BankPASSWORD.Set(szPassword);
            if (pPacket->m_HEADER.m_nSize > sizeof(cli_BANK_LIST_REQ)) {
                short nOffset = sizeof(cli_BANK_LIST_REQ);
                char* szPassWD = Packet_GetStringPtr(pPacket, nOffset);
                if (!szPassWD || strcmp(szPassword, szPassWD))
                    return pUSER->Send_gsv_BANK_LIST_REPLY(BANK_REPLY_INVALID_PASSWORD);
            } else {
                // ºñ¹ø ÇÊ¿ä...
                return pUSER->Send_gsv_BANK_LIST_REPLY(BANK_REPLY_NEED_PASSWORD);
            }
        }

        int iRewardMoney = this->db->GetInteger(BANKTBL_REWARD);
        if (iRewardMoney) {
            // º¸»ó±ÝÀÌ ÀÖ´Ù...
            this->db->MakeQuery("UPDATE tblGS_BANK SET intREWARD=",
                MQ_PARAM_INT,
                0,
                MQ_PARAM_ADDSTR,
                "WHERE txtACCOUNT=",
                MQ_PARAM_STR,
                pUSER->Get_ACCOUNT(),
                MQ_PARAM_END);
            if (this->db->ExecSQLBuffer() < 0) {
                // °íÄ¡±â ½ÇÆÐ !!!
                g_LOG.CS_ODS(LOG_NORMAL,
                    "SQL Exec ERROR:: UPDATE Bank money :%s %s \n",
                    pUSER->Get_ACCOUNT(),
                    this->db->GetERROR());
            } else {
                // »ó±Ý ´õÇÔ.
                pUSER->Add_MoneyNSend(iRewardMoney, GSV_REWARD_MONEY);
            }
        }
    }

    return pUSER->Send_gsv_BANK_ITEM_LIST();
}
/*
bool GS_CThreadSQL::Proc_cli_CHANGE_BANK_PASSWORD( tagQueryDATA *pSqlPACKET )
{
    xxxx
}
*/
//-------------------------------------------------------------------------------------------------
#define WSVAR_TBL_BLOB 2
#define ZONE_VAR_ECONOMY "%s_EC_Zone%d"

bool
GS_CThreadSQL::Proc_LOAD_ZONE_DATA(int iZoneNO) {
    CZoneTHREAD* pZONE = g_pZoneLIST->GetZONE(iZoneNO);
    assert(pZONE);

    m_TmpSTR.Printf(ZONE_VAR_ECONOMY,
        CLIB_GameSRV::GetInstance()->config.gameserver.server_name.c_str(),
        iZoneNO);

    this->db->MakeQuery("SELECT * FROM tblWS_VAR WHERE txtNAME=",
        MQ_PARAM_STR,
        m_TmpSTR.Get(),
        MQ_PARAM_END);
    if (!this->db->QuerySQLBuffer()) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }

    if (!this->db->GetNextRECORD()) {
        // insert !!!
        this->db->BindPARAM(1, pZONE->m_Economy.m_pEconomy, sizeof(tagECONOMY));

        this->db->MakeQuery("INSERT tblWS_VAR (txtNAME, dateUPDATE, binDATA) VALUES(",
            MQ_PARAM_STR,
            m_TmpSTR.Get(),
            MQ_PARAM_ADDSTR,
            ",",
            MQ_PARAM_STR,
            "TODO: FIX ME, was: g_pThreadLOG->GetCurDateTimeSTR()",
            MQ_PARAM_ADDSTR,
            ",",
            MQ_PARAM_BINDIDX,
            1,
            MQ_PARAM_ADDSTR,
            ");",
            MQ_PARAM_END);
        if (this->db->ExecSQLBuffer() < 1) {
            g_LOG.CS_ODS(LOG_NORMAL,
                "SQL Exec ERROR:: INSERT %s : %s \n",
                m_TmpSTR.Get(),
                this->db->GetERROR());
            return true;
        }
    } else {
        BYTE* pDATA = this->db->GetDataPTR(WSVAR_TBL_BLOB);

        ::CopyMemory(pZONE->m_Economy.m_pEconomy, pDATA, sizeof(tagECONOMY));
    }

    // Start Zone THREAD !!!!
    pZONE->Resume();

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
GS_CThreadSQL::Proc_SAVE_ZONE_DATA(int iZoneNO, sql_ZONE_DATA* pSqlZONE) {
    m_TmpSTR.Printf(ZONE_VAR_ECONOMY,
        CLIB_GameSRV::GetInstance()->config.gameserver.server_name.c_str(),
        iZoneNO);

    this->db->BindPARAM(1, pSqlZONE->m_btZoneDATA, pSqlZONE->m_nDataSIZE);

    this->db->MakeQuery("UPDATE tblWS_VAR SET dateUPDATE=",
        MQ_PARAM_STR,
        "TODO FIX ME: WAS g_pThreadLOG->GetCurDateTimeSTR()",
        MQ_PARAM_ADDSTR,
        ",binDATA=",
        MQ_PARAM_BINDIDX,
        1,
        MQ_PARAM_ADDSTR,
        "WHERE txtNAME=",
        MQ_PARAM_STR,
        m_TmpSTR.Get(),
        MQ_PARAM_END);
    if (this->db->ExecSQLBuffer() < 0) {
        // °íÄ¡±â ½ÇÆÐ !!!
        g_LOG.CS_ODS(LOG_NORMAL,
            "SQL Exec ERROR:: UPDATE %s %s \n",
            m_TmpSTR.Get(),
            this->db->GetERROR());
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
GS_CThreadSQL::Proc_LOAD_OBJVAR(tagQueryDATA* pSqlPACKET) {
    sql_ZONE_DATA* pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
    /*
        if ( pSqlZONE->m_btDataTYPE == SQL_ZONE_DATA_NPCOBJ_LOAD )
            m_TmpSTR.Printf( ZONE_VAR_NPCOBJ, pSqlPACKET->m_Name.Get() );
        else
            m_TmpSTR.Printf( ZONE_VAR_EVENTOBJ, pSqlPACKET->m_Name.Get() );
    */
    this->db->MakeQuery("SELECT * FROM tblWS_VAR WHERE txtNAME=",
        MQ_PARAM_STR,
        pSqlPACKET->m_Name.Get() /* m_TmpSTR.Get() */,
        MQ_PARAM_END);
    if (!this->db->QuerySQLBuffer()) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        g_LOG.CS_ODS(LOG_NORMAL, "      %s\n", this->db->GetQueryBuffPTR());
        return false;
    }

    if (!this->db->GetNextRECORD()) {
        // insert !!!
        this->db->BindPARAM(1, (BYTE*)pSqlZONE->m_btZoneDATA, pSqlZONE->m_nDataSIZE);

        this->db->MakeQuery("INSERT tblWS_VAR (txtNAME, dateUPDATE, binDATA) VALUES(",
            MQ_PARAM_STR,
            pSqlPACKET->m_Name.Get() /* m_TmpSTR.Get() */,
            MQ_PARAM_ADDSTR,
            ",",
            MQ_PARAM_STR,
           "TODO FIX ME: WAS g_pThreadLOG->GetCurDateTimeSTR()",
            MQ_PARAM_ADDSTR,
            ",",
            MQ_PARAM_BINDIDX,
            1,
            MQ_PARAM_ADDSTR,
            ");",
            MQ_PARAM_END);
        if (this->db->ExecSQLBuffer() < 1) {
            g_LOG.CS_ODS(LOG_NORMAL,
                "SQL Exec ERROR:: INSERT %s : %s \n",
                pSqlPACKET->m_Name.Get() /* m_TmpSTR.Get() */,
                this->db->GetERROR());
            return true;
        }
    } else {
        BYTE* pDATA = this->db->GetDataPTR(WSVAR_TBL_BLOB);

        switch (pSqlZONE->m_btDataTYPE) {
            case SQL_ZONE_DATA_NPCOBJ_LOAD:
                g_pZoneLIST->Init_NpcObjVAR(pSqlPACKET->m_iTAG, pDATA);
                break;
            case SQL_ZONE_DATA_EVENTOBJ_LOAD:
                g_pZoneLIST->Init_EventObjVAR(pSqlPACKET->m_iTAG, pDATA);
                break;
        }
    }

    return true;
}

bool
GS_CThreadSQL::Proc_SAVE_OBJVAR(tagQueryDATA* pSqlPACKET) {
    sql_ZONE_DATA* pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
    /*
        if ( pSqlZONE->m_btDataTYPE == SQL_ZONE_DATA_NPCOBJ_SAVE )
            m_TmpSTR.Printf( ZONE_VAR_NPCOBJ, pSqlPACKET->m_Name.Get() );
        else
            m_TmpSTR.Printf( ZONE_VAR_EVENTOBJ, pSqlPACKET->m_Name.Get() );
    */
    this->db->BindPARAM(1, (BYTE*)pSqlZONE->m_btZoneDATA, pSqlZONE->m_nDataSIZE);

    this->db->MakeQuery("UPDATE tblWS_VAR SET dateUPDATE=",
        MQ_PARAM_STR,
        "TODO FIX ME: WAS g_pThreadLOG->GetCurDateTimeSTR()",
        MQ_PARAM_ADDSTR,
        ",binDATA=",
        MQ_PARAM_BINDIDX,
        1,
        MQ_PARAM_ADDSTR,
        "WHERE txtNAME=",
        MQ_PARAM_STR,
        pSqlPACKET->m_Name.Get() /* m_TmpSTR.Get() */,
        MQ_PARAM_END);
    if (this->db->ExecSQLBuffer() < 0) {
        // °íÄ¡±â ½ÇÆÐ !!!
        g_LOG.CS_ODS(LOG_NORMAL,
            "SQL Exec ERROR:: UPDATE %s %s \n",
            pSqlPACKET->m_Name.Get() /* m_TmpSTR.Get() */,
            this->db->GetERROR());
    }

    return true;
}