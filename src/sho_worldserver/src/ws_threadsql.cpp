#include "stdAFX.h"

#include "CThreadGUILD.h"
#include "CThreadLOG.h"
#include "CWS_Client.h"
#include "IO_STB.h"
#include "WS_ThreadSQL.h"
#include "WS_ZoneLIST.h"

#include "rose/common/game_config.h"
#include "rose/common/game_types.h"
#include "rose/common/util.h"
#include "rose/database/database.h"

#include "nlohmann/json.hpp"

#include <regex>

using namespace Rose;
using namespace Rose::Common;
using namespace Rose::Database;
using namespace Rose::Network;
using namespace Rose::Util;

using json = nlohmann::json;

#define MAX_AVATAR_NAME 20

#define DELETE_CHAR_WAIT_TIME (60 * 60) //	60분

#define DATA_VER_2 2

CWS_ThreadSQL::CWS_ThreadSQL(): CSqlTHREAD(true) {
    COMPILE_TIME_ASSERT(sizeof(tagGrowAbility) <= 384);
#ifdef __KCHS_BATTLECART__
    COMPILE_TIME_ASSERT(sizeof(tagGrowAbility) == 383);
    COMPILE_TIME_ASSERT(sizeof(CInventory) == (140 * 14 + 8)); // 1954
    COMPILE_TIME_ASSERT(MAX_RIDING_PART == 5);
#else
    COMPILE_TIME_ASSERT(sizeof(tagGrowAbility) == 383);
    COMPILE_TIME_ASSERT(sizeof(CInventory) == (139 * 14 + 8)); // 1954
    COMPILE_TIME_ASSERT(MAX_RIDING_PART == 4);
#endif

    m_sGA.Init();
    m_sSA.Init();
    //	m_sQD.Init ();
    //	m_HotICON.Init ();
    m_sEmptyBANK.Init();

    m_pDefaultBE = NULL;
    m_pDefaultINV = NULL;

    m_nDefaultDataCNT = g_TblAVATAR.m_nDataCnt;
    if (m_nDefaultDataCNT > 0) {
        m_pDefaultBE = new tagBasicETC[m_nDefaultDataCNT];
        m_pDefaultINV = new CInventory[m_nDefaultDataCNT];
        m_pDefaultBA = new tagBasicAbility[m_nDefaultDataCNT];

        short nR, nJ;
        for (nR = 0; nR < m_nDefaultDataCNT; nR++) {
            m_pDefaultBA[nR].m_nSTR = AVATAR_STR(nR);
            m_pDefaultBA[nR].m_nDEX = AVATAR_DEX(nR);
            m_pDefaultBA[nR].m_nINT = AVATAR_INT(nR);
            m_pDefaultBA[nR].m_nCON = AVATAR_CON(nR);
            m_pDefaultBA[nR].m_nCHARM = AVATAR_CHARM(nR);
            m_pDefaultBA[nR].m_nSENSE = AVATAR_SENSE(nR);

            m_pDefaultBE[nR].Init();
            m_pDefaultBE[nR].m_nZoneNO = AVATAR_ZONE(nR);

            _ASSERT(m_pDefaultBE[nR].m_nZoneNO > 0);

            m_pDefaultINV[nR].Clear();
            m_pDefaultINV[nR].m_i64Money = AVATAR_MONEY(nR);

            // 초기 장작 아이템...
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_FACE_ITEM, AVATAR_FACEITEM(nR));
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_HELMET, AVATAR_HELMET(nR));
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_ARMOR, AVATAR_ARMOR(nR));
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_KNAPSACK, AVATAR_BACKITEM(nR));
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_GAUNTLET, AVATAR_GAUNTLET(nR));
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_BOOTS, AVATAR_BOOTS(nR));
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_WEAPON_R, AVATAR_WEAPON(nR));
            m_pDefaultINV[nR].SetInventory(EQUIP_IDX_WEAPON_L, AVATAR_SUBWPN(nR));

            // 초기 장비 아이템
            for (nJ = 0; nJ < 10; nJ++)
                m_pDefaultINV[nR].SetInventory(
                    (INV_WEAPON * INVENTORY_PAGE_SIZE) + nJ + MAX_EQUIP_IDX,
                    AVATAR_ITEM_WEAPON(nR, nJ));

            // 초기 소모 아이템
            for (nJ = 0; nJ < 5; nJ++) {
                m_pDefaultINV[nR].SetInventory((INV_USE * INVENTORY_PAGE_SIZE) + nJ + MAX_EQUIP_IDX,
                    AVATAR_ITEM_USE(nR, nJ),
                    AVATAR_ITEM_USECNT(nR, nJ));
            }

            // 초기 기타 아이템
            for (nJ = 0; nJ < 5; nJ++) {
                m_pDefaultINV[nR].SetInventory((INV_ETC * INVENTORY_PAGE_SIZE) + nJ + MAX_EQUIP_IDX,
                    AVATAR_ITEM_ETC(nR, nJ),
                    AVATAR_ITEM_ETCCNT(nR, nJ));
            }

            //	m_pDefaultBE[ nR ].m_nPartItemIDX[ BODY_PART_FACE		] = m_pDefaultINV[ nR
            //].m_ItemEQUIP[ nI ].m_nItemNo; 	m_pDefaultBE[ nR ].m_nPartItemIDX[ BODY_PART_HAIR
            //] = m_pDefaultINV[ nR ].m_ItemEQUIP[ nI ].m_nItemNo;
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_HELMET,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_HELMET]);
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_ARMOR,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_ARMOR]);
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_GAUNTLET,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_GAUNTLET]);
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_BOOTS,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_BOOTS]);
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_FACE_ITEM,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_FACE_ITEM]);
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_KNAPSACK,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_KNAPSACK]);
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_WEAPON_R,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_WEAPON_R]);
            m_pDefaultBE[nR].SetPartITEM(BODY_PART_WEAPON_L,
                m_pDefaultINV[nR].m_ItemEQUIP[EQUIP_IDX_WEAPON_L]);
        }
    }
}
__fastcall CWS_ThreadSQL::~CWS_ThreadSQL() {
    SAFE_DELETE_ARRAY(m_pDefaultBA);
    SAFE_DELETE_ARRAY(m_pDefaultBE);
    SAFE_DELETE_ARRAY(m_pDefaultINV);
}

void
CWS_ThreadSQL::Execute() {
    this->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL); // Priority 1 point above the priority class

    CDLList<tagQueryDATA>::tagNODE* pSqlNODE;

    LOG_TRACE("Start CWS_ThreadSQL::Execute(), ThreadID: %d(0x%x)", this->ThreadID, this->ThreadID);

    while (TRUE) {
        if (!this->Terminated) {
            m_pEVENT->WaitFor(INFINITE);
        } else {
            int iReaminCNT;
            this->m_CS.Lock();
            iReaminCNT = m_AddPACKET.GetNodeCount();
            this->m_CS.Unlock();

            if (iReaminCNT <= 0)
                break;
        }

        this->m_CS.Lock();
        m_RunPACKET.AppendNodeList(&m_AddPACKET);
        m_AddPACKET.Init();
        m_pEVENT->ResetEvent();
        this->m_CS.Unlock();

        for (pSqlNODE = m_RunPACKET.GetHeadNode(); pSqlNODE;) {
            if (Run_SqlPACKET(&pSqlNODE->m_VALUE))
                pSqlNODE = this->Del_SqlPACKET(pSqlNODE);
            else
                pSqlNODE = m_RunPACKET.GetNextNode(pSqlNODE);
        }

        this->tick();
    }

    int iCnt = m_AddPACKET.GetNodeCount();
    _ASSERT(iCnt == 0);

    LOG_TRACE("End CWS_ThreadSQL::Execute(), ThreadID: %d(0x%x)", this->ThreadID, this->ThreadID);
}

bool
CWS_ThreadSQL::Add_SqlPacketWithACCOUNT(CWS_Client* pUSER, t_PACKET* pPacket) {
    return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX,
        pUSER->Get_ACCOUNT(),
        (BYTE*)pPacket,
        pPacket->m_HEADER.m_nSize);
}

bool
CWS_ThreadSQL::Add_SqlPacketWithAVATAR(CWS_Client* pUSER, t_PACKET* pPacket) {
    return CSqlTHREAD::Add_SqlPACKET((int)pUSER->m_iSocketIDX,
        pUSER->Get_NAME(),
        (BYTE*)pPacket,
        pPacket->m_HEADER.m_nSize);
}

bool
CWS_ThreadSQL::Run_SqlPACKET(tagQueryDATA* pSqlPACKET) {
    switch (pSqlPACKET->m_pPacket->m_wType) {
        case CLI_CHAR_LIST:
            Proc_cli_CHAR_LIST(pSqlPACKET);
            break;

        case CLI_SELECT_CHAR:
            Proc_cli_SELECT_CHAR(pSqlPACKET);
            break;

        case CLI_DELETE_CHAR:
            Proc_cli_DELETE_CHAR(pSqlPACKET);
            break;

        case CLI_MEMO:
            Proc_cli_MEMO(pSqlPACKET);
            break;

        case SQL_ZONE_DATA: {
            sql_ZONE_DATA* pSqlZONE = (sql_ZONE_DATA*)pSqlPACKET->m_pPacket;
            switch (pSqlZONE->m_btDataTYPE) {
                case SQL_ZONE_DATA_WORLDVAR_SAVE:
                    Proc_SAVE_WORLDVAR(pSqlZONE);
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

struct tagDelCHAR {
    DWORD m_dwDBID;
    CStrVAR m_Name;
};

bool
CWS_ThreadSQL::Proc_cli_CHAR_LIST(tagQueryDATA* pSqlPACKET) {

    const char* stmt = "SELECT id, name, level, job_id, gender_id, face_id, hair_id, delete_by "
                       "FROM character WHERE "
                       "account_username=$1";

    std::string username = pSqlPACKET->m_Name.Get();
    QueryResult res = this->db_pg.query(stmt, {username});
    if (!res.is_ok()) {
        std::string msg = this->db_pg.last_error_message();
        LOG_ERROR("Error getting character list for account %s: %s", username.c_str(), msg.c_str());
        return false;
    }

    classPACKET pCPacket = classPACKET();
    pCPacket.m_HEADER.m_wType = WSV_CHAR_LIST;
    pCPacket.m_HEADER.m_nSize = sizeof(wsv_CHAR_LIST);

    size_t char_count = min(res.row_count, GameStaticConfig::MAX_CHARACTERS);
    pCPacket.m_wsv_CHAR_LIST.m_btCharCNT = 0;

    std::vector<std::string> delete_list;
    DateTime now = std::chrono::system_clock::now();

    for (size_t idx = 0; idx < char_count; ++idx) {
        std::string char_id = res.get_string(idx, 0);
        std::string char_name = res.get_string(idx, 1);
        int level = res.get_int32(idx, 2);
        int job_id = res.get_int32(idx, 3);
        int gender_id = res.get_int32(idx, 4);
        int face_id = res.get_int32(idx, 5);
        int hair_id = res.get_int32(idx, 6);
        DateTime delete_by = res.get_datetime(idx, 7);

        int64_t delete_seconds_remaining = 0;

        bool delete_by_null = res.get_null(idx, 7);
        if (!delete_by_null) {
            if (delete_by <= now) {
                delete_list.push_back(char_id);
                continue;
            }

            delete_seconds_remaining =
                std::chrono::duration_cast<std::chrono::seconds>(delete_by - now).count();
        }

        tagCHARINFO char_info;
        char_info.m_btCharRACE = gender_id;
        char_info.m_nJOB = job_id;
        char_info.m_nLEVEL = level;
        char_info.m_dwRemainSEC = delete_seconds_remaining;
        char_info.m_btIsPlatinumCHAR = false;

        tagPartITEM equipment[MAX_BODY_PART];
        std::fill_n(equipment, MAX_BODY_PART, tagPartITEM{});

        equipment[BODY_PART_FACE].m_nItemNo = face_id;
        equipment[BODY_PART_HAIR].m_nItemNo = hair_id;

        QueryResult equip_res =
            this->db_pg.query("SELECT equipment.slot, item.game_data_id"
                              " FROM equipment INNER JOIN item ON equipment.item_id = item.id"
                              " WHERE equipment.character_id = $1",
                {char_id});

        if (!equip_res.is_ok()) {
            LOG_DEBUG("Equipment query failed: %s", equip_res.error_message());
            continue;
        }

        for (size_t row_idx = 0; row_idx < equip_res.row_count; ++row_idx) {
            int slot = equip_res.get_int32(row_idx, 0);
            int game_data_id = equip_res.get_int32(row_idx, 1);

            if (slot >= 0 && slot < MAX_BODY_PART) {
                equipment[slot].m_nItemNo = game_data_id;
            }
        }

        pCPacket.AppendString((char*)char_name.c_str());
        pCPacket.AppendData(&char_info, sizeof(tagCHARINFO));
        pCPacket.AppendData(equipment, sizeof(tagPartITEM) * MAX_BODY_PART);
        pCPacket.m_wsv_CHAR_LIST.m_btCharCNT += 1;
    }

    CWS_Client* pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (pFindUSER) {
        pFindUSER->m_nPlatinumCharCNT = 0;
    }
    g_pUserLIST->SendPacketToSocketIDX(pSqlPACKET->m_iTAG, pCPacket);

    if (delete_list.size() > 0) {
        std::string q = "DELETE FROM character WHERE id IN ($1)";

        QueryResult delete_res = this->db_pg.query(q, delete_list);
        if (!delete_res.is_ok()) {
            LOG_ERROR("Failed to delete character(s): %s", delete_res.error_message());
        }
    }

    return true;
}

bool
CWS_ThreadSQL::Proc_cli_SELECT_CHAR(tagQueryDATA* pSqlPACKET) {
    t_PACKET* packet = (t_PACKET*)pSqlPACKET->m_pPacket;

    short offset = sizeof(cli_SELECT_CHAR);
    short strlen = 0;
    char* char_name = Packet_GetStringPtr(packet, offset, strlen);
    if (!char_name) {
        return false;
    }

    if (strlen == 0 || strlen > GameStaticConfig::MAX_CHARACTER_NAME) {
        return false;
    }

    const char* stmt = "SELECT id, account_username, map_id FROM character WHERE name=$1";
    QueryResult res = this->db_pg.query(stmt, {char_name});
    if (!res.is_ok()) {
        LOG_ERROR("Failed to select character with name %s: %s", char_name, res.error_message());
        return false;
    }

    if (!res.row_count == 1) {
        return false;
    }

    CWS_Client* user = (CWS_Client*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (!user) {
        return false;
    }

    std::string char_id = res.get_string(0, 0);
    std::string account_username = res.get_string(0, 1);
    int map_id = res.get_int32(0, 2);

    if (_strcmpi(user->Get_ACCOUNT(), account_username.c_str()) != 0) {
        return false;
    }

    g_pUserLIST->Add_CHAR(user, char_name);

    user->m_dwDBID = std::stoi(char_id);
    user->ClanINIT();

    if (!user->Send_wsv_MOVE_SERVER(map_id)) {
        return false;
    }

    const char* mail_stmt = "SELECT COUNT(*) FROM character_mail WHERE to_character_id=$1";
    QueryResult mail_res = this->db_pg.query(mail_stmt, {char_id});
    if (!mail_res.is_ok()) {
        LOG_ERROR("Failed to get mail count for character %s: %s",
            char_name,
            mail_res.error_message());
        return false;
    }

    g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REQ_RECEIVED_CNT, mail_res.get_int32(0, 0));

    return true;
}

bool
CWS_ThreadSQL::Proc_cli_DELETE_CHAR(tagQueryDATA* pSqlPACKET) {
    t_PACKET* pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

    short nOffset = sizeof(cli_DELETE_CHAR), nOutStrLen;
    char* pCharName = Packet_GetStringPtr(pPacket, nOffset, nOutStrLen);
    if (!pCharName || !pSqlPACKET->m_Name.Get()) {
        return false;
    }

    if (nOutStrLen > MAX_AVATAR_NAME) {
        g_LOG.CS_ODS(LOG_NORMAL, "Proc_cli_SELECT_CHAR:: CharName == '%s'\n", pCharName);
        return false;
    }

    {
        bool m_Injected = false;
        for (int i = 0; i < strlen(pCharName); i++) {
            if (!m_Injected) {
                if (pCharName[i] == '\'') {
                    if (pCharName[i + 1] == ';') {
                        g_LOG.CS_ODS(LOG_NORMAL,
                            "Proc_cli_DELETE_CHAR: 714 SQL Injection Recv'd and filtered\n");
                        m_Injected = true;
                        pCharName[i] = '\0';
                    }
                }
            } else {
                pCharName[i] = 0x00;
            }
        }
    }

    DWORD dwCurAbsSEC = 0, dwReaminSEC = 0;

    if (pPacket->m_cli_DELETE_CHAR.m_bDelete) {
        // 삭제 대기
        DWORD dwDelWaitTime;

        dwDelWaitTime = DELETE_CHAR_WAIT_TIME;
        dwCurAbsSEC = classTIME::GetCurrentAbsSecond() + dwDelWaitTime;
        dwReaminSEC = dwDelWaitTime;
    }

    // TODO: Instantly delete the character if they are the master of a clan???
    if (this->db->QuerySQL((char*)"{call ws_ClanCharGET(\'%s\')}", pCharName)) {
        if (this->db->GetNextRECORD()) {
            // 클랜 있다.
            int iClanPOS = this->db->GetInteger(2);
            if (iClanPOS >= GPOS_MASTER) {
                classUSER* pFindUSER = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
                if (pFindUSER) {
                    classPACKET pCPacket = classPACKET();
                    pCPacket.m_HEADER.m_wType = WSV_DELETE_CHAR;
                    pCPacket.m_HEADER.m_nSize = sizeof(wsv_DELETE_CHAR);

                    pCPacket.m_wsv_DELETE_CHAR.m_dwDelRemainTIME = 0xffffffff;
                    pCPacket.AppendString(pCharName);
                    pFindUSER->Send_Start(pCPacket);
                }
                return true;
            }
        }
    }

    if (this->db->ExecSQL((char*)"UPDATE character SET delete_by_int=%u WHERE account_name=\'%s\' "
                                 "AND name=\'%s\'",
            dwCurAbsSEC,
            pSqlPACKET->m_Name.Get(),
            pCharName)
        < 1) {
        // 오류 또는 삭제된것이 없다.
        g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR:: %s \n", this->db->GetERROR());
    }

    CWS_Client* pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (pFindUSER) {
        g_pThreadLOG->When_CharacterLOG(pFindUSER, pCharName, NEWLOG_DEL_START_CHAR);

        classPACKET pCPacket = classPACKET();
        pCPacket.m_HEADER.m_wType = WSV_DELETE_CHAR;
        pCPacket.m_HEADER.m_nSize = sizeof(wsv_DELETE_CHAR);

        pCPacket.m_wsv_DELETE_CHAR.m_dwDelRemainTIME = dwReaminSEC;
        pCPacket.AppendString(pCharName);

        pFindUSER->Send_Start(pCPacket);
    }

    return true;
}

bool
CWS_ThreadSQL::Load_WORLDVAR(int16_t* buffer, size_t count) {
    const char* stmt = "SELECT data FROM worldvar WHERE name = $1";

    QueryResult res = this->db_pg.query(stmt, {WORLD_VAR});
    if (!res.is_ok()) {
        std::string msg = this->db_pg.last_error_message();
        LOG_ERROR("Failed to load world var: %s", msg.c_str());
        return false;
    }

    if (res.row_count > 1) {
        LOG_ERROR("Found too many world var rows: %d", res.row_count);
        return false;
    }

    if (res.row_count == 0) {
        // Pad by 4-bytes which is "time" in CWorldVar
        int16_t* new_buffer = new int16_t[count + 2];
        new_buffer[0] = 0;
        new_buffer[1] = 0;
        std::memcpy(&new_buffer[2], buffer, count);
        this->Save_WorldVAR(reinterpret_cast<uint8_t*>(new_buffer), (count + 2) * sizeof(int16_t));

        LOG_INFO("World var not found, default values saved");
        return true;
    }

    json j = json::parse(res.get_string(0, 0));
    size_t c = min(j.size(), count);

    for (size_t i = 0; i < c; ++i) {
        buffer[i] = j[i].get<int16_t>();
    }

    return true;
}

bool
CWS_ThreadSQL::Save_WorldVAR(BYTE* pVarBUFF, short nBuffLEN) {
    sql_ZONE_DATA* pPacket;

    pPacket = (sql_ZONE_DATA*)new BYTE[sizeof(sql_ZONE_DATA) + nBuffLEN];

    pPacket->m_nDataSIZE = nBuffLEN;

    pPacket->m_wType = SQL_ZONE_DATA;
    pPacket->m_nSize = sizeof(sql_ZONE_DATA) + pPacket->m_nDataSIZE;

    pPacket->m_btDataTYPE = SQL_ZONE_DATA_WORLDVAR_SAVE;
    ::CopyMemory(pPacket->m_btZoneDATA, pVarBUFF, pPacket->m_nDataSIZE);

    CSqlTHREAD::Add_SqlPACKET(0, (char*)WORLD_VAR, (BYTE*)pPacket, pPacket->m_nSize);
    SAFE_DELETE_ARRAY(pPacket);

    return true;
}

bool
CWS_ThreadSQL::Proc_SAVE_WORLDVAR(sql_ZONE_DATA* pSqlZONE) {
    // First bytes are "time" in CWorldVar, we want to get just the var list
    size_t offset = 4;

    size_t count = (pSqlZONE->m_nDataSIZE - offset) / sizeof(int16_t);
    if (count <= 0) {
        return false;
    }

    if (count > MAX_WORLD_VAR_CNT) {
        count = MAX_WORLD_VAR_CNT;
    }

    int16_t* buffer = reinterpret_cast<int16_t*>(pSqlZONE->m_btZoneDATA + offset);

    json j = json::array();
    for (size_t i = 0; i < count; ++i) {
        j.push_back(buffer[i]);
    }

    const char* stmt = "INSERT INTO worldvar (name, data) VALUES ($1, $2)"
                       "ON CONFLICT (name) DO UPDATE SET data=$2";

    QueryResult res = this->db_pg.query(stmt, {WORLD_VAR, j.dump()});
    if (!res.is_ok()) {
        std::string msg = this->db_pg.last_error_message();
        LOG_ERROR("Failed to save WORLDVAR: %s", msg.c_str());
        return false;
    }

    return true;
}

bool
CWS_ThreadSQL::Proc_cli_MEMO(tagQueryDATA* pSqlPACKET) {
    t_PACKET* pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

    std::string char_name = pSqlPACKET->m_Name.Get();
    if (char_name.empty()) {
        return false;
    }

    classUSER* user = (classUSER*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (!user) {
        return false;
    }
    std::string user_id = std::to_string(user->Get_DBID());

    switch (pPacket->m_cli_MEMO.m_btTYPE) {
        case MEMO_REQ_RECEIVED_CNT: {
            const char* stmt = "SELECT COUNT(*) FROM character, character_mail WHERE "
                               "character.id=$1 AND character.id=character_mail.to_character_id";

            QueryResult res = this->db_pg.query(stmt, {user_id});
            if (!res.is_ok()) {
                LOG_ERROR("Failed to get mail count for character %s: %s",
                    char_name.c_str(),
                    res.error_message());
                return false;
            }
            g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG,
                MEMO_REPLY_RECEIVED_CNT,
                res.get_int32(0, 0));
            return true;
        }

        case MEMO_REQ_CONTENTS: {
            const char* stmt = "SELECT cm.id, cm.sent, cm.message, "
                               "from_char.name as from_character_name "
                               "FROM character_mail AS cm "
                               "LEFT JOIN character as from_char ON from_char.id = cm.from_character_id "
                               "LEFT JOIN character as to_char ON to_char.id = cm.to_character_id "
                               "WHERE to_char.id = $1";

            QueryResult res = this->db_pg.query(stmt, {user_id});
            if (!res.is_ok()) {
                LOG_ERROR("Failed to get mail for character %s: %s",
                    user->Get_DBID(),
                    res.error_message());
                return false;
            }

            classPACKET packet = classPACKET();
            packet.m_HEADER.m_wType = WSV_MEMO;
            packet.m_HEADER.m_nSize = sizeof(wsv_MEMO);
            packet.m_wsv_MEMO.m_btTYPE = MEMO_REPLY_CONTENTS;

            std::vector<std::string> delete_list;
            for (size_t idx = 0; idx < res.row_count; ++idx) {
                std::string mail_id = res.get_string(idx, 0);
                DateTime sent = res.get_datetime(idx, 1);
                std::string message = res.get_string(idx, 2);
                std::string from = res.get_string(idx, 3);

                int32_t* pDW = reinterpret_cast<int32_t*>(&packet.m_pDATA[packet.m_HEADER.m_nSize]);
                packet.m_HEADER.m_nSize += 4;
                *pDW = time_since_win_epoch(sent).count();
                packet.AppendString(const_cast<char*>(from.c_str()));
                packet.AppendString(const_cast<char*>(message.c_str()));

                if (packet.m_HEADER.m_nSize > MAX_PACKET_SIZE - 270) {
                    // 꽉찼다... 전송
                    g_pUserLIST->SendPacketToSocketIDX(pSqlPACKET->m_iTAG, packet);

                    packet = classPACKET();
                    packet.m_HEADER.m_wType = WSV_MEMO;
                    packet.m_HEADER.m_nSize = sizeof(wsv_MEMO);
                    packet.m_wsv_MEMO.m_btTYPE = MEMO_REPLY_CONTENTS;
                }

                delete_list.push_back(mail_id);
            }

            g_pUserLIST->SendPacketToSocketIDX(pSqlPACKET->m_iTAG, packet);

            // Delete mail sent to the client from the database
            if (delete_list.size() > 0) {
                const char* delete_stmt = "DELETE FROM character_mail WHERE id IN ($1)";
                QueryResult delete_res = this->db_pg.query(delete_stmt, {delete_list});
                if (!delete_res.is_ok()) {
                    LOG_ERROR("Failed to delete mail for character %s: %s",
                        char_name.c_str(),
                        delete_res.error_message());
                    return false;
                }
            }
            return true;
        }

        case MEMO_REQ_SEND: {
            short offset = sizeof(cli_MEMO);
            char* target_char = Packet_GetStringPtr(pPacket, offset);

            if (!target_char || strlen(target_char) < 1) {
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_INVALID_TARGET);
                return true;
            }

            const char* stmt = "SELECT id FROM character WHERE character.name=$1";
            QueryResult res = this->db_pg.query(stmt, {target_char});
            if (!res.is_ok()) {
                LOG_ERROR("Failed to get character id for character %s: %s",
                    target_char,
                    res.error_message());
                return false;
            }

            if (res.row_count == 0) {
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_INVALID_TARGET);
                return true;
            }

            std::string target_char_id = res.get_string(0, 0);

            char* message = Packet_GetStringPtr(pPacket, offset);
            if (!message || strlen(message) < 2) {
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_INVALID_CONTENT);
                return true;
            }

            const char* count_stmt =
                "SELECT COUNT(*) FROM character, character_mail WHERE "
                "character.name=$1 AND character.id=character_mail.to_character_id";

            QueryResult count_res = this->db_pg.query(count_stmt, {target_char});
            if (!count_res.is_ok()) {
                LOG_ERROR("Failed to get mail count for character %s: %s",
                    target_char,
                    count_res.error_message());
                return false;
            }

            if (count_res.get_int32(0, 0) > GameStaticConfig::MAX_MAIL_MESSAGES) {
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_FULL_MEMO);
                return true;
            }

            const char* add_stmt = "INSERT INTO character_mail (to_character_id, "
                                   "from_character_id, message) VALUES ($1)";

            QueryResult add_res = this->db_pg.query(stmt, {target_char_id, user_id, message});
            if (!add_res.is_ok()) {
                LOG_ERROR("Failed to add character mail from %s to %s: %s",
                    pSqlPACKET->m_Name.Get(),
                    target_char_id,
                    add_res.error_message())
                return false;
            }

            g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_OK);
            return true;
        }
    }

    return true;
}

void
CWS_ThreadSQL::handle_queued_packet(QueuedPacket& p) {
    Packets::PacketType packet_type = p.packet.packet_data()->data_type();
    switch (packet_type) {
        case Packets::PacketType::CharacterCreateRequest: {
            this->handle_char_create_req(p);
            break;
        }
    }
}

bool
CWS_ThreadSQL::handle_char_create_req(QueuedPacket& p) {
    const Packets::CharacterCreateRequest* req =
        p.packet.packet_data()->data_as_CharacterCreateRequest();

    if (!req) {
        return false;
    }

    if (!req->name()) {
        return false;
    }

    std::regex name_re("^[a-zA-Z0-9_-]+$");
    bool name_valid = req->name()->size() > 3
        && req->name()->size() <= GameStaticConfig::MAX_CHARACTER_NAME
        && std::regex_match(req->name()->c_str(), name_re);

    bool face_valid = req->face_id() > 0 && req->face_id() < g_TblFACE.m_nDataCnt;
    bool hair_valid = req->hair_id() > 0 && req->hair_id() < g_TblHAIR.m_nDataCnt;

    Gender gender = gender_from(req->gender_id());
    Job job = job_from(req->job_id());

    bool job_valid = (job == Job::Visitor) || is_first_job(job);

    if (!(name_valid && face_valid && hair_valid && job_valid)) {
        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_FAILED);
        return false;
    }

    // Check if the name already exists
    std::string query = "SELECT COUNT(*) FROM character WHERE name=?";
    this->db->bind_string(1, req->name()->str());

    if (!this->db->execute(query)) {
        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_FAILED);
        return false;
    }

    if (this->db->fetch() != FetchResult::Ok) {
        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_FAILED);
        return false;
    }

    if (this->db->get_int32(1) != 0) {
        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_DUP_NAME);
        return false;
    }

    CWS_Client* client = g_pUserLIST->find_client(p.socket_id);
    if (!client) {
        return false;
    }

    std::string account_name = client->Get_ACCOUNT();

    // Check character count
    query = "SELECT COUNT(*) from character WHERE account_name=?";
    this->db->bind_string(1, account_name);

    if (!this->db->execute(query)) {
        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_FAILED);
        return false;
    }

    if (this->db->fetch() != FetchResult::Ok) {
        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_FAILED);
        return false;
    }

    int char_count = this->db->get_int32(1);
    if (char_count >= GameStaticConfig::MAX_CHARACTERS) {
        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_NO_MORE_SLOT);
        return false;
    }

    const int gender_id = static_cast<int>(gender);
    const int start_map_id = AVATAR_ZONE(gender_id);

    tagBasicETC& basic_etc = m_pDefaultBE[gender_id];

    basic_etc.m_btCharSlotNO = 0; // Premium slots?
    basic_etc.m_btCharRACE = gender_id;
    basic_etc.m_nZoneNO = start_map_id;
    basic_etc.m_PosSTART = g_ZoneLIST.Get_StartPOS(start_map_id);
    basic_etc.m_nReviveZoneNO = start_map_id;
    basic_etc.m_PosREVIVE = g_ZoneLIST.Get_StartRevivePOS(start_map_id);
    basic_etc.m_PartITEM[BODY_PART_FACE].m_nItemNo = req->face_id();
    basic_etc.m_PartITEM[BODY_PART_HAIR].m_nItemNo = req->hair_id();

    tagBasicINFO basic_info;
    basic_info.Init(0, req->face_id(), req->hair_id());
    basic_info.m_nClass = req->job_id();

    CInventory& inv = m_pDefaultINV[gender_id];
    tagBasicAbility& basic_ability = m_pDefaultBA[gender_id];

    query = "INSERT INTO character (account_name, name, basic_etc, "
            "basic_info, basic_ability, grow_ability, skill_ability, inventory, "
            "face_id, hair_id, gender_id, map_id, respawn_x, respawn_y, "
            "town_respawn_id, town_respawn_x, town_respawn_y, job_id)"
            "values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    this->db->bind_string(1, account_name);
    this->db->bind_string(2, req->name()->str());
    this->db->bind_binary(3, reinterpret_cast<uint8_t*>(&basic_etc), sizeof(tagBasicETC));
    this->db->bind_binary(4, reinterpret_cast<uint8_t*>(&basic_info), sizeof(tagBasicINFO));
    this->db->bind_binary(5, reinterpret_cast<uint8_t*>(&basic_ability), sizeof(tagBasicAbility));
    this->db->bind_binary(6, reinterpret_cast<uint8_t*>(&m_sGA), sizeof(tagGrowAbility));
    this->db->bind_binary(7, reinterpret_cast<uint8_t*>(&m_sSA), sizeof(tagSkillAbility));
    this->db->bind_binary(8, reinterpret_cast<uint8_t*>(&inv), sizeof(CInventory));
    this->db->bind_int16(9, basic_info.m_cFaceIDX);
    this->db->bind_int16(10, basic_info.m_cHairIDX);
    this->db->bind_int16(11, basic_etc.m_btCharRACE);
    this->db->bind_int16(12, basic_etc.m_nZoneNO);
    this->db->bind_float(13, basic_etc.m_PosSTART.x);
    this->db->bind_float(14, basic_etc.m_PosSTART.y);
    this->db->bind_int16(15, basic_etc.m_nReviveZoneNO);
    this->db->bind_float(16, basic_etc.m_PosREVIVE.x);
    this->db->bind_float(17, basic_etc.m_PosREVIVE.y);
    this->db->bind_int32(18, static_cast<int>(job));

    if (!this->db->execute(query)) {
        LOG_ERROR("Failed to insert character %s for account %s",
            req->name()->c_str(),
            account_name.c_str());

        for (const std::string& error: this->db->get_error_messages()) {
            LOG_ERROR(error.c_str());
        }

        g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_FAILED);
        return false;
    }

    g_pUserLIST->Send_wsv_CREATE_CHAR(p.socket_id, RESULT_CREATE_CHAR_OK, char_count + 1);

    return true;
}
