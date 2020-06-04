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

#include "rose/common/game_config.h"
#include "rose/database/database.h"

#include "nlohmann/json.hpp"

using namespace Rose;
using namespace Rose::Common;
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

#define BEGINNER_ZONE_NO 20

bool
GS_CThreadSQL::UpdateUserRECORD(classUSER* user) {
    const char* char_name = user->Get_NAME();

    // Begin the database transaction
    QueryResult trans_res = this->db_pg.query("BEGIN", {});
    if (!trans_res.is_ok()) {
        LOG_ERROR("Failed to begin transaction when saving character '%s': %s",
            char_name,
            trans_res.error_message());
        return false;
    }

    // Update the character info
    std::string query(
        "UPDATE character SET gender_id=$1, job_id=$2, face_id=$3, hair_id=$4, level=$5, exp=$6, "
        "hp=$7, mp=$8, stamina=$9, max_hp=$10, max_mp=$11, max_stamina=$12, str=$13, dex=$14, "
        "intt=$15, con=$16, cha=$17, sen=$18, stat_points=$19, skill_points=$20 , money=$21, "
        "storage_money=$22, map_id=$23, respawn_x=$24, respawn_y=$25, town_respawn_id=$26, "
        "town_respawn_x=$27, town_respawn_y=$28, union_id=$29, skills=$30 "
        "WHERE id=$31");

    json skills_json = json::array();
    for (size_t i = 0; i < MAX_LEARNED_SKILL_CNT; ++i) {
        skills_json.push_back(user->m_Skills.m_nSkillINDEX[i]);
    }

    QueryResult char_res = this->db_pg.query(query,
        {
            std::to_string(user->m_nCharRACE),
            std::to_string(user->m_BasicINFO.m_nClass),
            std::to_string(user->m_BasicINFO.m_cFaceIDX),
            std::to_string(user->m_BasicINFO.m_cHairIDX),
            std::to_string(user->m_GrowAbility.m_nLevel),
            std::to_string(user->m_GrowAbility.m_lEXP),
            std::to_string(user->m_GrowAbility.m_nHP),
            std::to_string(user->m_GrowAbility.m_nMP),
            std::to_string(user->m_GrowAbility.m_nSTAMINA),
            std::to_string(user->Get_MaxHP()),
            std::to_string(user->Get_MaxMP()),
            std::to_string(MAX_STAMINA),
            std::to_string(user->m_BasicAbility.m_nSTR),
            std::to_string(user->m_BasicAbility.m_nDEX),
            std::to_string(user->m_BasicAbility.m_nINT),
            std::to_string(user->m_BasicAbility.m_nCON),
            std::to_string(user->m_BasicAbility.m_nCHARM),
            std::to_string(user->m_BasicAbility.m_nSENSE),
            std::to_string(user->m_GrowAbility.m_nBonusPoint),
            std::to_string(user->m_GrowAbility.m_nSkillPoint),
            std::to_string(user->m_Inventory.m_i64Money),
            std::to_string(user->m_Bank.m_i64ZULY),
            std::to_string(user->m_nZoneNO),
            std::to_string(user->m_PosCUR.x),
            std::to_string(user->m_PosCUR.y),
            std::to_string(user->m_nReviveZoneNO),
            std::to_string(user->m_PosREVIVE.x),
            std::to_string(user->m_PosREVIVE.y),
            std::to_string(user->m_BasicINFO.m_cUnion),
            skills_json.dump(),
            std::to_string(user->m_dwDBID),
        });

    if (!char_res.is_ok()) {
        LOG_ERROR("Failed to save character info for '%s': %s",
            user->Get_NAME(),
            char_res.error_message());

        trans_res = this->db_pg.query("ROLLBACK", {});
        if (!trans_res.is_ok()) {
            LOG_ERROR("Failed to rollback transaction when saving character '%s': %s",
                char_name,
                trans_res.error_message());
        }

        return false;
    }

    std::vector<int> delete_list;

    // Update inventory info
    std::string bulk;
    for (size_t i = 0; i < INVENTORY_TOTAL_SIZE; ++i) {
        tagITEM& item = user->m_Inventory.m_ItemLIST[i];
        if (item.GetTYPE() == 0 || item.IsEmpty() || !item.IsValidITEM()) {
            delete_list.push_back(i);
            continue;
        }

        uint16_t gem_id = 0;
        uint16_t grade = 0;
        uint16_t durability = 0;
        uint16_t lifespan = 0;
        bool is_appraisal = false;
        bool has_socket = false;
        bool is_crafted = false;
        uint16_t quantity = 1;

        if (item.IsEnableDupCNT()) {
            quantity = item.GetQuantity();
        } else {
            gem_id = item.GetGemNO();
            grade = item.GetGrade();
            durability = item.GetDurability();
            lifespan = item.GetLife();
            is_appraisal = item.IsAppraisal();
            has_socket = item.HasSocket();
            is_crafted = item.IsCreated();
        }

        bulk += fmt::format(
            "INSERT INTO item (uuid, game_data_id, type_id, stat_id, grade, durability, "
            "lifespan, appraisal, socket, crafted) "
            "VALUES ('{0}', {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}) "
            "ON CONFLICT (uuid) "
            "DO UPDATE SET uuid='{0}', game_data_id={1}, type_id={2}, stat_id={3}, grade={4}, "
            "durability={5}, lifespan={6}, appraisal={7}, socket={8}, crafted={9};",
            item.uuid.to_string(),
            std::to_string(item.GetItemNO()),
            std::to_string(item.GetTYPE()),
            std::to_string(gem_id),
            std::to_string(grade),
            std::to_string(durability),
            std::to_string(lifespan),
            PG_BOOL(is_appraisal),
            PG_BOOL(has_socket),
            PG_BOOL(is_crafted));

        bulk += fmt::format(
            "INSERT INTO inventory (owner_id, slot, quantity, item_id) "
            "VALUES ({0}, {1}, {2}, (SELECT id FROM item WHERE uuid='{3}')) "
            "ON CONFLICT (owner_id, slot) "
            "DO UPDATE SET quantity={2}, item_id=(SELECT id FROM item WHERE uuid='{3}');",
            user->m_dwDBID,
            i,
            quantity,
            item.uuid.to_string());
    }

    bulk += fmt::format("DELETE FROM item WHERE id IN (SELECT item_id FROM inventory WHERE owner_id={} AND slot IN ({}));",
        user->m_dwDBID,
        value_list(delete_list));

    QueryResult bulk_res = this->db_pg.batch(bulk);
    if (!bulk_res.is_ok()) {
        LOG_ERROR("Failed to update items for character '%s': %s",
            char_name,
            bulk_res.error_message());

        trans_res = this->db_pg.query("ROLLBACK", {});
        if (!trans_res.is_ok()) {
            LOG_ERROR("Failed to rollback transaction when creating '%s': %s",
                char_name,
                trans_res.error_message());
        }
        return false;
    }

    // Commit the transaction
    trans_res = this->db_pg.query("COMMIT", {});
    if (!trans_res.is_ok()) {
        LOG_ERROR("Failed to commit transaction when saving character '%s': %s",
            char_name,
            trans_res.error_message());
        return false;
    }

    // TODO: Storage
    // OH BOY

    // TODO: Quest data
    // JSON

    // TODO: Wish list
    // JSON

    // TODO: Hoticons
    // JSON

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

    short nOffset = sizeof(cli_SELECT_CHAR);
    short nOutStrLen = 0;
    char* pCharName = Packet_GetStringPtr(pPacket, nOffset, nOutStrLen);
    if (!pCharName || nOutStrLen < 4) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: CharName == NULL \n");
        return false;
    }
#define MAX_AVATAR_NAME 20

    if (nOutStrLen > MAX_AVATAR_NAME) {
        g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_SELECT_CHAR:: CharName == '%s'\n", pCharName);
        return false;
    }

    std::string char_name(pCharName);
    if (char_name.empty() || char_name.size() > GameStaticConfig::MAX_CHARACTER_NAME) {
        return false;
    }

    classUSER* user = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (!user) {
        return false;
    }

    std::string account_username = user->Get_ACCOUNT();

    std::string query =
        "SELECT id, gender_id, job_id, face_id, hair_id, level, exp, hp, mp, stamina, max_hp, "
        "max_mp, "
        "max_stamina, str, dex, intt, con, cha, sen, stat_points, skill_points, money, "
        "storage_money, map_id, "
        "respawn_x, respawn_y, town_respawn_id, town_respawn_x, town_respawn_y, union_id, skills "
        "FROM character "
        "WHERE account_username=$1 AND name=$2";

    enum CharCol {
        COL_ID = 0,
        COL_GENDER_ID,
        COL_JOB_ID,
        COL_FACE_ID,
        COL_HAIR_ID,
        COL_LEVEL,
        COL_EXP,
        COL_HP,
        COL_MP,
        COL_STAMINA,
        COL_MAX_HP,
        COL_MAX_MP,
        COL_MAX_STAMINA,
        COL_STR,
        COL_DEX,
        COL_INT,
        COL_CON,
        COL_CHA,
        COL_SEN,
        COL_STAT_POINTS,
        COL_SKILL_POINTS,
        COL_MONEY,
        COL_STORAGE_MONEY,
        COL_MAP_ID,
        COL_RESPAWN_X,
        COL_RESPAWN_Y,
        COL_TOWN_RESPAWN_ID,
        COL_TOWN_RESPAWN_X,
        COL_TOWN_RESPAWN_Y,
        COL_UNION_ID,
        COL_SKILLS,
    };

    QueryResult char_res = this->db_pg.query(query, {account_username, char_name});
    if (!char_res.is_ok()) {
        LOG_ERROR("Failed to get character '%s' for account '%s': %s",
            account_username.c_str(),
            char_name.c_str(),
            char_res.error_message());
        return false;
    }

    if (char_res.row_count != 1) {
        return false;
    }

    QueryResult item_res = this->db_pg.query(
        "SELECT inventory.slot, inventory.quantity, item.uuid, "
        "item.game_data_id, item.type_id, item.stat_id, item.grade, "
        "item.durability, item.lifespan, item.appraisal, item.socket, item.crafted "
        "FROM inventory INNER JOIN item ON inventory.item_id = item.id "
        "WHERE inventory.owner_id = $1",
        {char_res.get_string(0, COL_ID)});

    enum InvCol {
        INV_COL_SLOT,
        INV_COL_QUANTITY,
        INV_COL_UUID,
        INV_COL_GAME_DATA_ID,
        INV_COL_TYPE_ID,
        INV_COL_STAT_ID,
        INV_COL_GRADE,
        INV_COL_DURABILITY,
        INV_COL_LIFESPAN,
        INV_COL_APPRAISAL,
        INV_COL_SOCKET,
        INV_COL_CRAFTED,
    };

    if (!item_res.is_ok()) {
        LOG_ERROR("Inventory query failed for character '%s': %s", item_res.error_message());
        return false;
    }

    tagBasicINFO basic_info;
    basic_info.Init(0, 0, 0);
    basic_info.m_cFaceIDX = char_res.get_int32(0, COL_FACE_ID);
    basic_info.m_cHairIDX = char_res.get_int32(0, COL_HAIR_ID);
    basic_info.m_nClass = char_res.get_int32(0, COL_JOB_ID);
    basic_info.m_cUnion = char_res.get_int32(0, COL_UNION_ID);

    tagBasicETC basic_etc;
    basic_etc.Init();
    basic_etc.m_btCharRACE = char_res.get_int32(0, COL_GENDER_ID);
    basic_etc.m_nZoneNO = char_res.get_int32(0, COL_MAP_ID);
    basic_etc.m_PosSTART.x = char_res.get_float(0, COL_RESPAWN_X);
    basic_etc.m_PosSTART.y = char_res.get_float(0, COL_RESPAWN_Y);
    basic_etc.m_nReviveZoneNO = char_res.get_int32(0, COL_TOWN_RESPAWN_ID);
    basic_etc.m_PosREVIVE.x = char_res.get_float(0, COL_TOWN_RESPAWN_X);
    basic_etc.m_PosREVIVE.y = char_res.get_float(0, COL_TOWN_RESPAWN_Y);

    basic_etc.m_PartITEM[BODY_PART_FACE].m_nItemNo = basic_info.m_cFaceIDX;
    basic_etc.m_PartITEM[BODY_PART_HAIR].m_nItemNo = basic_info.m_cHairIDX;

    tagBasicAbility basic_ability;
    basic_ability.Init();
    basic_ability.m_nSTR = char_res.get_int32(0, COL_STR);
    basic_ability.m_nDEX = char_res.get_int32(0, COL_DEX);
    basic_ability.m_nINT = char_res.get_int32(0, COL_INT);
    basic_ability.m_nCON = char_res.get_int32(0, COL_CON);
    basic_ability.m_nCHARM = char_res.get_int32(0, COL_CHA);
    basic_ability.m_nSENSE = char_res.get_int32(0, COL_SEN);

    tagGrowAbility grow_ability;
    grow_ability.Init();
    grow_ability.m_nLevel = char_res.get_int32(0, COL_LEVEL);
    grow_ability.m_lEXP = char_res.get_int32(0, COL_EXP);
    grow_ability.m_nHP = char_res.get_int32(0, COL_HP);
    grow_ability.m_nMP = char_res.get_int32(0, COL_MP);
    grow_ability.m_nBonusPoint = char_res.get_int32(0, COL_STAT_POINTS);
    grow_ability.m_nSkillPoint = char_res.get_int32(0, COL_SKILL_POINTS);
    grow_ability.m_nSTAMINA = char_res.get_int32(0, COL_STAMINA);

    // TODO: In grow_ability
    // short m_nUnionPOINT[MAX_UNION_COUNT];
    // m_btBodySIZE / m_btHeadSIZE -- Would be fun to get these working!
    // m_lPenalEXP -- What is this?
    // m_nPKFlag -- What is this?

    tagSkillAbility skill_ability;
    skill_ability.Init();

    json j = j.parse(char_res.get_string(0, COL_SKILLS));
    if (!j.is_array()) {
        LOG_ERROR("Skills column for character '%s' is not a valid json array.");
        return false;
    }

    size_t skill_count = min(j.size(), MAX_LEARNED_SKILL_CNT);
    for (size_t idx = 0; idx < skill_count; ++idx) {
        skill_ability.m_nSkillINDEX[idx] = j[idx];
    }

    // TODO: Populate quest data
    tagQuestData quest_data;
    quest_data.Init();
    quest_data.CheckExpiredTIME();

    CInventory inventory;
    inventory.Clear();
    inventory.m_i64Money = char_res.get_int64(0, COL_MONEY);

    // TODO: Populate wish list
    tagWishLIST wish_list;
    wish_list.Init();

    // TODO: Populate hotbar icons
    CHotICONS hotbar_icons;
    hotbar_icons.Init();

    for (size_t row_idx = 0; row_idx < item_res.row_count; ++row_idx) {
        int slot = item_res.get_int32(row_idx, INV_COL_SLOT);

        if (slot <= 0) {
            continue;
        }

        const size_t part_idx = inventory2part(slot);
        if (part_idx >= BODY_PART_HELMET && part_idx < MAX_BODY_PART) {
            tagPartITEM& part = basic_etc.m_PartITEM[part_idx];
            part.m_nItemNo = item_res.get_int32(row_idx, INV_COL_GAME_DATA_ID);
            part.m_cGrade = item_res.get_int32(row_idx, INV_COL_GRADE);
            part.m_nGEM_OP = item_res.get_int32(row_idx, INV_COL_STAT_ID);
            part.m_bHasSocket = item_res.get_bool(row_idx, INV_COL_SOCKET) ? 1 : 0;
        }

        const size_t ride_part_idx = inventory2ride(slot);
        if (ride_part_idx >= RIDE_PART_BODY && ride_part_idx < MAX_RIDING_PART) {
            tagPartITEM& part = basic_etc.m_RideITEM[ride_part_idx];
            part.m_nItemNo = item_res.get_int32(row_idx, INV_COL_GAME_DATA_ID);
        }

        tagITEM item;
        item.Init(0, 1);
        item.uuid = Rose::Util::UUID::from_string(item_res.get_string(row_idx, INV_COL_UUID));
        item.m_nItemNo = item_res.get_int32(row_idx, INV_COL_GAME_DATA_ID);
        item.m_cType = item_res.get_int32(row_idx, INV_COL_TYPE_ID);

        if (item.IsEnableDupCNT()) {
            item.m_uiQuantity = item_res.get_int32(row_idx, INV_COL_QUANTITY);
        } else {
            item.m_bCreated = item_res.get_bool(row_idx, INV_COL_CRAFTED);
            item.m_bHasSocket = item_res.get_bool(row_idx, INV_COL_SOCKET);
            item.m_bIsAppraisal = item_res.get_bool(row_idx, INV_COL_APPRAISAL);
            item.m_cDurability = item_res.get_int32(row_idx, INV_COL_DURABILITY);
            item.m_cGrade = item_res.get_int32(row_idx, INV_COL_GRADE);
            item.m_nLife = item_res.get_int32(row_idx, INV_COL_LIFESPAN);
            item.m_nGEM_OP = item_res.get_int32(row_idx, INV_COL_STAT_ID);
        }
        inventory.m_ItemLIST[slot] = item;
    }

    CZoneTHREAD* zone = g_pZoneLIST->GetZONE(basic_etc.m_nZoneNO);
    if (zone) {
        if (basic_etc.m_PosSTART.x == 0.0f || basic_etc.m_PosSTART.y == 0.0f) {
            basic_etc.m_PosSTART = zone->Get_StartRevivePOS();
        } else {
            basic_etc.m_PosSTART = zone->Get_RevivePOS(basic_etc.m_PosSTART);
            basic_etc.m_PosSTART.x += (RANDOM(1001) - 500);
            basic_etc.m_PosSTART.y += (RANDOM(1001) - 500);
        }
    } else {
        basic_etc.m_nZoneNO = AVATAR_ZONE(basic_etc.m_btCharRACE);
        CZoneTHREAD* zone2 = g_pZoneLIST->GetZONE(basic_etc.m_nZoneNO);
        if (!zone) {
            basic_etc.m_nZoneNO = 1;
            zone = g_pZoneLIST->GetZONE(basic_etc.m_nZoneNO);
        }
        basic_etc.m_PosSTART = zone->Get_StartPOS();
    }

    user->Set_NAME(pCharName);
    user->Set_RNAME(pCharName);
    user->m_btPlatinumCHAR = false;
    user->m_dwDBID = char_res.get_int32(0, COL_ID);
    user->m_i64StartMoney = char_res.get_int32(0, COL_MONEY);
    user->m_nCharRACE = basic_etc.m_btCharRACE;
    user->m_nZoneNO = basic_etc.m_nZoneNO;
    user->m_PosCUR = basic_etc.m_PosSTART;
    user->m_nReviveZoneNO = basic_etc.m_nReviveZoneNO;
    user->m_PosREVIVE = basic_etc.m_PosREVIVE;
    user->m_dwBackUpTIME = zone->GetCurrentTIME();
    user->m_dwItemSN = std::chrono::system_clock::now().time_since_epoch().count();

    g_pUserLIST->Add_CHAR(user);

    std::copy(&basic_etc.m_PartITEM[0],
        &basic_etc.m_PartITEM[0] + MAX_BODY_PART,
        &user->m_PartITEM[0]);

    std::copy(&basic_etc.m_RideITEM[0],
        &basic_etc.m_RideITEM[0] + MAX_RIDING_PART,
        &user->m_RideITEM[0]);

    user->m_BasicINFO.Init(0, 0, 0);
    user->m_BasicAbility.Init();
    user->m_GrowAbility.Init();
    user->m_Skills.Init();
    user->m_Quests.Init();
    user->m_Inventory.Clear();
    user->m_WishLIST.Init();
    user->m_HotICONS.Init();

    std::memcpy(&user->m_BasicINFO, &basic_info, sizeof(tagBasicINFO));
    std::memcpy(&user->m_BasicAbility, &basic_ability, sizeof(tagBasicAbility));
    std::memcpy(&user->m_GrowAbility, &grow_ability, sizeof(tagGrowAbility));
    std::memcpy(&user->m_Skills, &skill_ability, sizeof(tagSkillAbility));
    std::memcpy(&user->m_Quests, &quest_data, sizeof(tagQuestData));
    std::memcpy(&user->m_Inventory, &inventory, sizeof(CInventory));
    std::memcpy(&user->m_WishLIST, &wish_list, sizeof(tagWishLIST));
    std::memcpy(&user->m_HotICONS, &hotbar_icons, sizeof(CHotICONS));

    // Client doesn't need stat options if it hasn't been appraised yet
    for (size_t idx = 0; idx < INVENTORY_TOTAL_SIZE; ++idx) {
        tagITEM& item = user->m_Inventory.m_ItemLIST[idx];
        if (item.IsAppraisal()) {
            item.m_nGEM_OP = 0;
        }
    }

    user->InitPassiveSkill();
    user->UpdateAbility();
    user->Set_ShotITEM();

    if (user->GetCur_STAMINA() > MAX_STAMINA) {
        user->SetCur_STAMINA(MAX_STAMINA);
    }

    if (user->Get_HP() <= 0) {
        user->Set_HP(3 * user->Get_MaxHP() / 10);
    } else if (user->Get_HP() > user->Get_MaxHP()) {
        user->Set_HP(user->Get_MaxHP());
    }

    std::shared_ptr<classPACKET> p = std::make_shared<classPACKET>();
    if (pPacket->m_cli_SELECT_CHAR.m_btCharNO) {
        p->m_HEADER.m_wType = GSV_SELECT_CHAR;
        p->m_HEADER.m_nSize = sizeof(gsv_SELECT_CHAR);

        p->m_gsv_SELECT_CHAR.m_btCharRACE = basic_etc.m_btCharRACE;
        p->m_gsv_SELECT_CHAR.m_nZoneNO = user->m_nZoneNO;
        p->m_gsv_SELECT_CHAR.m_PosSTART = user->m_PosCUR;
        p->m_gsv_SELECT_CHAR.m_nReviveZoneNO = user->m_nReviveZoneNO;
        p->m_gsv_SELECT_CHAR.m_dwUniqueTAG = user->m_dwDBID;

        std::copy(&basic_etc.m_PartITEM[0],
            &basic_etc.m_PartITEM[0] + MAX_BODY_PART,
            &p->m_gsv_SELECT_CHAR.m_PartITEM[0]);

        std::memcpy(&p->m_gsv_SELECT_CHAR.m_BasicINFO, &basic_info, sizeof(tagBasicINFO));
        std::memcpy(&p->m_gsv_SELECT_CHAR.m_BasicAbility, &basic_ability, sizeof(tagBasicAbility));
        std::memcpy(&p->m_gsv_SELECT_CHAR.m_GrowAbility, &grow_ability, sizeof(tagGrowAbility));
        std::memcpy(&p->m_gsv_SELECT_CHAR.m_Skill, &skill_ability, sizeof(tagSkillAbility));
        std::memcpy(&p->m_gsv_SELECT_CHAR.m_HotICONS, &hotbar_icons, sizeof(CHotICONS));

        p->AppendString(user->Get_NAME());

        user->SendPacket(p.get());
        user->Send_gsv_INVENTORYnQUEST_DATA();
    } else {
        user->m_bRunMODE = pPacket->m_cli_SELECT_CHAR.m_btRunMODE;
        user->m_btRideMODE = pPacket->m_cli_SELECT_CHAR.m_btRideMODE;
        user->Send_gsv_TELEPORT_REPLY(user->m_PosCUR, user->m_nZoneNO);
    }

    user->m_Bank.Init();
    user->m_btBankData = BANK_UNLOADED;

    user->m_dwLoginTIME = std::chrono::system_clock::now().time_since_epoch().count();

    return true;
}

#define BEGINNER_ZONE 20

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