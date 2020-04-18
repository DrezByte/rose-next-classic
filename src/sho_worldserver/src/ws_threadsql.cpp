#include "stdAFX.h"

#include "CThreadGUILD.h"
#include "CThreadLOG.h"
#include "CWS_Client.h"
#include "IO_STB.h"
#include "WS_ThreadSQL.h"
#include "WS_ZoneLIST.h"

#include "rose/common/game_config.h"
#include "rose/common/game_types.h"

using namespace Rose;
using namespace Rose::Common;
using namespace Rose::Network;

#ifdef __EUROPE // Oct. 6 2005 추가 (권형근)
    #define MAX_CHAR_PER_USER 3
#else
    #define MAX_CHAR_PER_USER 5
#endif

//#define	MAX_CREATE_CHAR_PER_USER	3
#define MAX_CREATE_CHAR_PER_USER MAX_CHAR_PER_USER
#define MAX_AVATAR_NAME 20

#define DELETE_CHAR_WAIT_TIME (60 * 60) //	60분

#define DATA_VER_2 2

enum AVTTBL_COL_IDX {
    AVTTBL_CHARID = 0,
    AVTTBL_ACCOUNT,
    AVTTBL_NAME = 2,
    AVTTBL_LEVEL,
    AVTTBL_MONEY,
    AVTTBL_RIGHT,
    AVTTBL_BASIC_ETC = 6,
    AVTTBL_BASIC_INFO,
    AVTTBL_BASIC_ABILITY,
    AVTTBL_GROW_ABILITY,
    AVTTBL_SKILL_ABILITY,
    AVTTBL_QUEST = 11,
    AVTTBL_INVENTORY,
    AVTTBL_HOTICON,
    AVTTBL_DELETE_TIME,
    AVTTBL_WISHLIST,
    AVTTBL_OPTION = 16,
    AVTTBL_JOB_NO,
    AVTTBL_REG_TIME,
    AVTTBL_PARTY_IDX,
    AVTTBL_ITEM_SN,
    AVTTBL_DATA_VER
};

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
bool __fastcall CWS_ThreadSQL::ConvertBasicETC() {
    /*
        short nI;
        tagBasicETC3 *pOldBE;
        tagBasicETC   sNewBE;
        int iCharID, iRecCNT=0;

        while( true )
        {
            this->db->MakeQuery( "SELECT TOP 1 intCharID, binBasicE FROM tblGS_AVATAR WHERE
       intDataVER=0 ", //and txtACCOUNT=\'icarus3\'", MQ_PARAM_END); if (
       !this->db->QuerySQLBuffer() ) { g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n",
       this->db->GetERROR() ); return false;
            }

            if ( !this->db->GetNextRECORD() ) {
                return false;
            }

            iCharID = this->db->GetInteger( 0 );
            pOldBE = (tagBasicETC3*)this->db->GetDataPTR( 1 );

            assert( iCharID && pOldBE );

            sNewBE.Init ();
            sNewBE.m_btCharRACE		= pOldBE->m_btCharRACE;
            sNewBE.m_nReviveZoneNO	= pOldBE->m_nReviveZoneNO;

            sNewBE.m_PosSTART		= pOldBE->m_PosSTART	;
            sNewBE.m_btCharRACE		= pOldBE->m_btCharRACE	;
            sNewBE.m_nZoneNO		= pOldBE->m_nZoneNO	;
            sNewBE.m_PosREVIVE		= pOldBE->m_PosREVIVE	;
            sNewBE.m_nReviveZoneNO	= pOldBE->m_nReviveZoneNO;

            for (nI=0; nI<MAX_BODY_PART; nI++) {
                sNewBE.m_PartITEM[ nI ].m_nItemNo = pOldBE->m_nPartItemIDX[ nI ];
            }
            for (nI=0; nI<MAX_RIDING_PART; nI++) {
                sNewBE.m_RideITEM[ nI ].m_nItemNo = pOldBE->m_nRideItemIDX[ nI ];
            }

            this->db->BindPARAM( 1, (BYTE*)&sNewBE,		sizeof( tagBasicETC )	);
            this->db->MakeQuery( "UPDATE tblGS_AVATAR SET binBasicE=",
                                                            MQ_PARAM_BINDIDX,	1,
                        MQ_PARAM_ADDSTR, ",intDataVER=",	MQ_PARAM_INT,		2,
                        MQ_PARAM_ADDSTR, "WHERE intCharID=",MQ_PARAM_INT,	iCharID,
                                                            MQ_PARAM_END );
            if ( this->db->ExecSQLBuffer() < 0 ) {
                // 고치기 실패 !!!
                // log ...
                g_LOG.CS_ODS(LOG_NORMAL, "SQL Exec ERROR:: UPDATE binBasicE: %d, \n", iCharID,
       this->db->GetERROR() ); } else iRecCNT ++;
        }

        g_LOG.CS_ODS(LOG_NORMAL, "Complete ConvertBasicETC, %d records\n", iRecCNT);
    */
    return true;
}

//-------------------------------------------------------------------------------------------------
void
CWS_ThreadSQL::Execute() {
    this->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL); // Priority 1 point above the priority class

    CDLList<tagQueryDATA>::tagNODE* pSqlNODE;

    g_LOG.CS_ODS(0xffff,
        ">  > >> CWS_ThreadSQL::Execute() ThreadID: %d(0x%x)\n",
        this->ThreadID,
        this->ThreadID);

    this->ConvertBasicETC();

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

    g_LOG.CS_ODS(0xffff,
        "<<<< CWS_ThreadSQL::Execute() ThreadID: %d(0x%x)\n",
        this->ThreadID,
        this->ThreadID);
}
//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct tagDelCHAR {
    DWORD m_dwDBID;
    CStrVAR m_Name;
};
bool
CWS_ThreadSQL::Proc_cli_CHAR_LIST(tagQueryDATA* pSqlPACKET) {
    /*
        //주의 !!! 프로지져를 쓸려면 프로시져에 intCharID를 추가해야 한다.
        // if ( !this->db->QuerySQL( "{call ws_GetCharLIST(\'%s\')}", pSqlPACKET->m_Name.Get() )
       ) { this->db->MakeQuery( "SELECT txtNAME, binBasicE, binBasicI, binGrowA, dwDelTIME,
       intCharID FROM tblGS_AVATAR WHERE txtACCOUNT=", MQ_PARAM_STR, pSqlPACKET->m_Name.Get(),
                                MQ_PARAM_END);
    */
    this->db->MakeQuery(
        (char*)"SELECT txtNAME, binBasicE, binBasicI, binGrowA, dwDelTIME, intDataVER "
               "FROM tblGS_AVATAR WHERE txtACCOUNT=",
        MQ_PARAM_STR,
        pSqlPACKET->m_Name.Get(),
        MQ_PARAM_END);

    if (!this->db->QuerySQLBuffer()) {
        // ???
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }
    if (!this->db->QuerySQLBuffer()) {
        // ???
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }

    classPACKET pCPacket = classPACKET();

    pCPacket.m_HEADER.m_wType = WSV_CHAR_LIST;
    pCPacket.m_HEADER.m_nSize = sizeof(wsv_CHAR_LIST);
    pCPacket.m_wsv_CHAR_LIST.m_btCharCNT = 0;

    if (this->db->GetNextRECORD()) {
        tagBasicINFO* pBI;
        tagBasicETC* pBE;
        tagGrowAbility* pGA;
        tagCHARINFO sCHAR;
        char* szCharName;
        short nC = 0, nPlatinumCharCNT = 0;
        CSLList<tagDelCHAR> DelList;
        CSLList<tagDelCHAR>::tagNODE* pNode;
        DWORD dwDelSEC, dwCurAbsSEC = classTIME::GetCurrentAbsSecond();
        do {
            if (this->db->GetStrPTR(0)) {
                szCharName = this->db->GetStrPTR(0);
                dwDelSEC = this->db->GetInteger(4);

                if (dwDelSEC && dwCurAbsSEC >= dwDelSEC) {
                    // 삭제...
                    pNode = new CSLList<tagDelCHAR>::tagNODE;
                    pNode->m_VALUE.m_Name.Set(szCharName);
                    // pNode->m_VALUE.m_dwDBID = this->db->GetInteger( 5 );
                    DelList.AppendNode(pNode);
                } else {
                    if (dwDelSEC) {
                        // 삭제 대기...
                        dwDelSEC -= dwCurAbsSEC;
                    }

                    pCPacket.AppendString(szCharName);

                    pBE = (tagBasicETC*)this->db->GetDataPTR(1);
                    pBI = (tagBasicINFO*)this->db->GetDataPTR(2);
                    pGA = (tagGrowAbility*)this->db->GetDataPTR(3);
#ifdef __KCHS_BATTLECART__
                    short nDataVER = this->db->GetInteger16(5);
#endif
                    sCHAR.m_btCharRACE = pBE->m_btCharRACE;
                    sCHAR.m_nJOB = pBI->m_nClass;
                    sCHAR.m_nLEVEL = pGA->m_nLevel;
                    sCHAR.m_dwRemainSEC = dwDelSEC;
#ifdef __INC_PLATINUM
    #ifdef __KCHS_BATTLECART__
                    if (nDataVER < DATA_VER_2)
                        sCHAR.m_btIsPlatinumCHAR = *((BYTE*)(&pBE->m_RideITEM[RIDE_PART_ARMS]));
                    else
                        sCHAR.m_btIsPlatinumCHAR = pBE->m_btCharSlotNO;
    #else
                    sCHAR.m_btIsPlatinumCHAR = pBE->m_btCharSlotNO;
    #endif
#endif
                    if (pBE->m_btCharSlotNO)
                        nPlatinumCharCNT++;
                    pCPacket.AppendData(&sCHAR, sizeof(tagCHARINFO));
                    pCPacket.AppendData(pBE->m_PartITEM, sizeof(tagPartITEM) * MAX_BODY_PART);

                    pCPacket.m_wsv_CHAR_LIST.m_btCharCNT++;
                }
            }
        } while (this->db->GetNextRECORD() && ++nC < MAX_CHAR_PER_USER);

        if (g_pUserLIST->SendPacketToSocketIDX(pSqlPACKET->m_iTAG, pCPacket)) {
            CWS_Client* pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
            if (pFindUSER) {
                pFindUSER->m_nPlatinumCharCNT = nPlatinumCharCNT;
            }
        }

        pNode = DelList.GetHeadNode();
        while (pNode) {
            // if ( this->db->ExecSQL("DELETE FROM tblGS_AVATAR WHERE txtNAME=\'%s\'",
            // pNode->m_VALUE.m_Name.Get() ) < 1 ) {
            //	// 오류 또는 삭제된것이 없다.
            //	g_LOG.CS_ODS(LOG_NORMAL, "Exec ERROR(DELETE_CHAR:%s):: %s \n",
            // pNode->m_VALUE.m_Name.Get(), this->db->GetERROR() );
            //}

            long iResultSP = -99;
            SDWORD cbSize1 = SQL_NTS;
            this->db->SetParam_long(1, iResultSP, cbSize1);

#define SP_DeleteCHAR "{?=call ws_CharDELETE(\'%s\')}"
            if (this->db->QuerySQL((char*)SP_DeleteCHAR, pNode->m_VALUE.m_Name.Get())) {
                while (this->db->GetMoreRESULT()) {
                    if (this->db->BindRESULT()) {
                        if (this->db->GetNextRECORD()) {
                            ;
                        }
                    }
                }
                if (0 != iResultSP) {
                    // 디비프로시져 삭제 실패...
                    g_LOG.CS_ODS(LOG_NORMAL,
                        "SP Return ERROR Code:%d (ws_DeleteCHAR:%s):: %s \n",
                        iResultSP,
                        pNode->m_VALUE.m_Name.Get(),
                        this->db->GetERROR());
                }
            } else {
                // 삭제 실패.;
                g_LOG.CS_ODS(LOG_NORMAL,
                    "Exec ERROR(ws_DeleteCHAR:%s):: %s \n",
                    pNode->m_VALUE.m_Name.Get(),
                    this->db->GetERROR());
            }

            DelList.DeleteHeadNFree();
            pNode = DelList.GetHeadNode();
        };
    } else {
        // 이 서버에는 등록된 케릭터가 없다.
        CWS_Client* pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
        if (pFindUSER) {
            pFindUSER->m_nPlatinumCharCNT = 0;
        }
        g_pUserLIST->SendPacketToSocketIDX(pSqlPACKET->m_iTAG, pCPacket);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
CWS_ThreadSQL::Proc_cli_SELECT_CHAR(tagQueryDATA* pSqlPACKET) {
    /*
        존 번호에 해당되는 존서버로 이동하라고 ...
    */
    t_PACKET* pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

    short nOffset = sizeof(cli_SELECT_CHAR), nOutStrLen;
    char* pCharName = Packet_GetStringPtr(pPacket, nOffset, nOutStrLen);
    if (!pCharName) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: CharName == NULL \n");
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
                            "Proc_cli_SELECT_CHAR: 715 SQL Injection Recv'd and filtered\n");
                        m_Injected = true;
                        pCharName[i] = '\0';
                    }
                }
            } else {
                pCharName[i] = 0x00;
            }
        }
    }

    // 케릭터의 소유자인지 판단...
    this->db->MakeQuery((char*)"SELECT txtACCOUNT, binBasicE, intCharID, intDataVER FROM "
                               "tblGS_AVATAR WHERE txtNAME=",
        MQ_PARAM_STR,
        pCharName,
        MQ_PARAM_END);
    if (!this->db->QuerySQLBuffer()) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }
    if (!this->db->GetNextRECORD()) {
        // 케릭터 없다.
        g_LOG.CS_ODS(LOG_NORMAL, "Char[ %s ] not found ...\n", pCharName);
        return false;
    }

    char* szCharOWNER = this->db->GetStrPTR(0);
    tagBasicETC* pBE = (tagBasicETC*)this->db->GetDataPTR(1);
    short nDataVER = this->db->GetInteger16(3);

    CWS_Client* pUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(pSqlPACKET->m_iTAG);
    if (pUSER) {
#ifdef __KCHS_BATTLECART__
        BYTE btCharSlotNO =
            nDataVER < DATA_VER_2 ? *((BYTE*)(&pBE->m_RideITEM[4])) : pBE->m_btCharSlotNO;
#else
        BYTE btCharSlotNO = pBE->m_btCharSlotNO;
#endif
        if (btCharSlotNO) {
            // 플레티넘 케릭이면 ???
            if (0 == (pUSER->m_dwPayFLAG & PLAY_FLAG_EXTRA_CHAR)) {
                // 뭐냐 ???
                pUSER->CloseSocket();
                return false;
            }
        }
        pUSER->m_dwDBID = this->db->GetInteger(2);
        pUSER->ClanINIT();

        short nZoneNO = pBE->m_nZoneNO;
        if (!_strcmpi(pUSER->Get_ACCOUNT(), szCharOWNER)) {
            // 존이 0 이면 ...디폴트 존으로..
            g_pUserLIST->Add_CHAR(pUSER, pCharName);
            if (pUSER->Send_wsv_MOVE_SERVER(
                    (0 == nZoneNO) ? AVATAR_ZONE(pBE->m_btCharRACE) : nZoneNO)) {
                // 도착한 쪽지 체크...
                this->db->MakeQuery((char*)"SELECT Count(*) FROM tblWS_MEMO WHERE txtNAME=",
                    MQ_PARAM_STR,
                    pCharName,
                    MQ_PARAM_END);
                if (!this->db->QuerySQLBuffer()) {
                    g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
                    return true;
                }

                if (this->db->GetNextRECORD() && this->db->GetInteger(0) > 0) {
                    // 쪽지 갯수...
                    g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG,
                        MEMO_REQ_RECEIVED_CNT,
                        this->db->GetInteger(0));
                }
            }
        }
    } // else 접속 끊겼다.

    return false;
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

    if (this->db->ExecSQL((char*)"UPDATE tblGS_AVATAR SET dwDelTIME=%u WHERE txtACCOUNT=\'%s\' "
                                 "AND txtNAME=\'%s\'",
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

//-------------------------------------------------------------------------------------------------
#define WSVAR_TBL_BLOB 2
bool
CWS_ThreadSQL::Load_WORLDVAR(BYTE* pVarBUFF, short nBuffLEN) {
    this->db->MakeQuery((char*)"SELECT * FROM tblWS_VAR WHERE txtNAME=",
        MQ_PARAM_STR,
        WORLD_VAR,
        MQ_PARAM_END);
    if (!this->db->QuerySQLBuffer()) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }

    if (!this->db->GetNextRECORD()) {
        // insert !!!
        this->db->BindPARAM(1, pVarBUFF, nBuffLEN);

        this->db->MakeQuery((char*)"INSERT tblWS_VAR (txtNAME, dateUPDATE, binDATA) VALUES(",
            MQ_PARAM_STR,
            WORLD_VAR,
            MQ_PARAM_ADDSTR,
            ",",
            MQ_PARAM_STR,
            g_pThreadLOG->GetCurDateTimeSTR(),
            MQ_PARAM_ADDSTR,
            ",",
            MQ_PARAM_BINDIDX,
            1,
            MQ_PARAM_ADDSTR,
            ");",
            MQ_PARAM_END);
        if (this->db->ExecSQLBuffer() < 1) {
            g_LOG.CS_ODS(LOG_NORMAL,
                (char*)"SQL Exec ERROR:: INSERT %s : %s \n",
                WORLD_VAR,
                this->db->GetERROR());
            return true;
        }
    } else {
        BYTE* pDATA = this->db->GetDataPTR(WSVAR_TBL_BLOB);

        ::CopyMemory(pVarBUFF, pDATA, nBuffLEN);
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
    this->db->BindPARAM(1, (BYTE*)pSqlZONE->m_btZoneDATA, pSqlZONE->m_nDataSIZE);

    this->db->MakeQuery((char*)"UPDATE tblWS_VAR SET dateUPDATE=",
        MQ_PARAM_STR,
        g_pThreadLOG->GetCurDateTimeSTR(),
        MQ_PARAM_ADDSTR,
        ",binDATA=",
        MQ_PARAM_BINDIDX,
        1,
        MQ_PARAM_ADDSTR,
        "WHERE txtNAME=",
        MQ_PARAM_STR,
        WORLD_VAR,
        MQ_PARAM_END);
    if (this->db->ExecSQLBuffer() < 0) {
        // 고치기 실패 !!!
        g_LOG.CS_ODS(LOG_NORMAL,
            "SQL Exec ERROR:: UPDATE %s %s \n",
            WORLD_VAR,
            this->db->GetERROR());
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
CWS_ThreadSQL::Proc_cli_MEMO(tagQueryDATA* pSqlPACKET) {
    t_PACKET* pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

    switch (pPacket->m_cli_MEMO.m_btTYPE) {
        case MEMO_REQ_RECEIVED_CNT: {
            if (!this->db->QuerySQL((char*)"SELECT Count(*) FROM tblWS_MEMO WHERE txtNAME=\'%s\';",
                    pSqlPACKET->m_Name.Get())) {
                g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
                return false;
            }
            g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG,
                MEMO_REPLY_RECEIVED_CNT,
                this->db->GetInteger(0));
            return true;
        }

        case MEMO_REQ_CONTENTS: {
            // 한번에 5개의 쪽지 읽음
            if (!this->db->QuerySQL((char*)"SELECT TOP 5 dwDATE, txtFROM, txtMEMO FROM tblWS_MEMO "
                                           "WHERE txtNAME=\'%s\' ORDER BY dwDATE ASC",
                    pSqlPACKET->m_Name.Get())) {
                g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
                return false;
            }
            // EX: delete top 2 from `tblgs_error` where txtACCOUNT='gmsho004' order by dateREG ASC
            if (this->db->GetNextRECORD()) {
                classPACKET pCPacket = classPACKET();

                pCPacket.m_HEADER.m_wType = WSV_MEMO;
                pCPacket.m_HEADER.m_nSize = sizeof(wsv_MEMO);
                pCPacket.m_wsv_MEMO.m_btTYPE = MEMO_REPLY_CONTENTS;

                DWORD dwDate, *pDW;
                char *szFrom, *szMemo;
                int iMemoCNT = 0;
                do {
                    iMemoCNT++;
                    dwDate = (DWORD)this->db->GetInteger(0);
                    szFrom = this->db->GetStrPTR(1);
                    szMemo = this->db->GetStrPTR(2, false);

                    pDW = (DWORD*)(&pCPacket.m_pDATA[pCPacket.m_HEADER.m_nSize]);
                    pCPacket.m_HEADER.m_nSize += 4;
                    *pDW = dwDate;
                    pCPacket.AppendString(szFrom);
                    pCPacket.AppendString(szMemo);

                    if (pCPacket.m_HEADER.m_nSize > MAX_PACKET_SIZE - 270) {
                        // 꽉찼다... 전송
                        g_pUserLIST->SendPacketToSocketIDX(pSqlPACKET->m_iTAG, pCPacket);

                        pCPacket = classPACKET();
                        pCPacket.m_HEADER.m_wType = WSV_MEMO;
                        pCPacket.m_HEADER.m_nSize = sizeof(wsv_MEMO);
                        pCPacket.m_wsv_MEMO.m_btTYPE = MEMO_REPLY_CONTENTS;
                    }
                } while (this->db->GetNextRECORD());

                g_pUserLIST->SendPacketToSocketIDX(pSqlPACKET->m_iTAG, pCPacket);

                if (this->db->ExecSQL(
                        (char*)"DELETE FROM tblWS_MEMO WHERE (intSN IN (SELECT TOP %d intSN FROM "
                               "tblWS_MEMO WHERE txtNAME=\'%s\' ORDER BY dwDATE ASC))",
                        iMemoCNT,
                        pSqlPACKET->m_Name.Get())
                    < 1) {
                    // 오류 또는 삭제된것이 없다.
                    g_LOG.CS_ODS(LOG_NORMAL,
                        "Exec ERROR(DELETE_MEMO):: %s \n",
                        this->db->GetERROR());
                    return true;
                }
            } // else 쪽지 없다.
            break;
        }
        case MEMO_REQ_SEND: {
            short nOffset = sizeof(cli_MEMO);
            char* szTargetCHAR;

            szTargetCHAR = Packet_GetStringPtr(pPacket, nOffset);
            if (!szTargetCHAR || strlen(szTargetCHAR) < 1) {
                // 잘못된 케릭 이름
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_INVALID_TARGET);
                return true;
            }
            char* szMemo = Packet_GetStringPtr(pPacket, nOffset);
            if (!szMemo || strlen(szTargetCHAR) < 2) {
                // 쪽지 내용 오류.
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_INVALID_CONTENT);
                return true;
            }

#define OPTION_REFUSE_JJOKJI 0x00000001
            // 대상 케릭의 쪽지 수신 거부 여부...
            if (!this->db->QuerySQL(
                    (char*)"SELECT dwOPTION FROM tblGS_AVATAR WHERE txtNAME=\'%s\';",
                    szTargetCHAR)) {
                g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
                return false;
            }

            if (!this->db->GetNextRECORD()) {
                // 보낼 대상 케릭 없다.
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_NOT_EXIST);
                return true;
            }
            if (this->db->GetInteger(0) & OPTION_REFUSE_JJOKJI) {
                // 거부 상태
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_REFUSED);
                return true;
            }

#define MAX_RECV_MEMO_CNT 50
            // 대상 케릭이 몇개의 보관된 쪽지가 있냐?
            if (!this->db->QuerySQL((char*)"SELECT Count(*) FROM tblWS_MEMO WHERE txtNAME=\'%s\';",
                    szTargetCHAR)) {
                g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
                return false;
            }
            if (this->db->GetNextRECORD() && this->db->GetInteger(0) > MAX_RECV_MEMO_CNT) {
                // MAX_RECV_MEMO_CNT 개 이상의 쪽지를 보유 하고 있다면...
                g_pUserLIST->Send_wsv_MEMO(pSqlPACKET->m_iTAG, MEMO_REPLY_SEND_FULL_MEMO);
                return true;
            }

            // 쪽지 저장..
            DWORD dwCurAbsSEC = classTIME::GetCurrentAbsSecond();
            this->db->MakeQuery(
                (char*)"INSERT tblWS_MEMO (dwDATE, txtNAME, txtFROM, txtMEMO) VALUES(",
                MQ_PARAM_INT,
                dwCurAbsSEC,
                MQ_PARAM_ADDSTR,
                ",",
                MQ_PARAM_STR,
                szTargetCHAR,
                MQ_PARAM_ADDSTR,
                ",",
                MQ_PARAM_STR,
                pSqlPACKET->m_Name.Get(),
                MQ_PARAM_ADDSTR,
                ",",
                MQ_PARAM_STR,
                szMemo,
                MQ_PARAM_ADDSTR,
                ");",
                MQ_PARAM_END);
            if (this->db->ExecSQLBuffer() < 1) {
                // 만들기 실패 !!!
                g_LOG.CS_ODS(LOG_NORMAL,
                    "SQL Exec ERROR:: INSERT MEMO:%s : %s \n",
                    pSqlPACKET->m_Name.Get(),
                    this->db->GetERROR());
                return true;
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
        case Packets::PacketType::PacketType_CharacterCreateRequest: {
            this->handle_char_create_req(p);
            break;
        }
    }
}

bool
CWS_ThreadSQL::handle_char_create_req(QueuedPacket& p) {
    const Packets::CharacterCreateRequest* req =
        p.packet.packet_data()->data_as_CharacterCreateRequest();

    bool name_valid =
        req->name()->size() > 3 && req->name()->size() <= GameConfig::MAX_CHARACTER_NAME;
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
    std::string query = "SELECT COUNT(*) FROM tblGS_AVATAR WHERE txtNAME=?";
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

    query = "SELECT COUNT(*) from tblGS_AVATAR WHERE txtACCOUNT=?";
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
    if (char_count >= GameConfig::MAX_CHARACTERS) {
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

    query =
        "INSERT INTO tblGS_AVATAR (txtACCOUNT, txtNAME, intDataVER, binBasicE, "
        "binBasicI, binBasicA, binGrowA, binSkillA, blobINV, intJOB) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

    this->db->bind_string(1, account_name);
    this->db->bind_string(2, req->name()->str());
    this->db->bind_int32(3, DATA_VER_2);
    this->db->bind_binary(4, reinterpret_cast<uint8_t*>(&basic_etc), sizeof(tagBasicETC));
    this->db->bind_binary(5, reinterpret_cast<uint8_t*>(&basic_info), sizeof(tagBasicINFO));
    this->db->bind_binary(6, reinterpret_cast<uint8_t*>(&basic_ability), sizeof(tagBasicAbility));
    this->db->bind_binary(7, reinterpret_cast<uint8_t*>(&m_sGA), sizeof(tagGrowAbility));
    this->db->bind_binary(8, reinterpret_cast<uint8_t*>(&m_sSA), sizeof(tagSkillAbility));
    this->db->bind_binary(9, reinterpret_cast<uint8_t*>(&inv), sizeof(CInventory));
    this->db->bind_int32(10, static_cast<int>(job));

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
