#include "stdafx.h"

#include "LIB_gsMAIN.h"

#include "CSocketWND.h"

#include "GS_ThreadLOG.h"
#include "GS_ThreadSQL.h"
#include "GS_ThreadZONE.h"

#include "CThreadGUILD.h"
#include "GS_ListUSER.h"
#include "GS_PARTY.h"
#include "GS_SocketASV.h"
#include "GS_SocketLSV.h"
#include "GS_ThreadMALL.h"
#include "IO_AI.h"
#include "IO_PAT.h"
#include "IO_Quest.h"
#include "IO_Skill.h"
#include "ZoneLIST.h"
#include "classTIME.h"
#include "ioDataPOOL.h"

#include "rose/common/util.h"

#define TEST_ZONE_NO 100
#define DB_INI_STRING 30

// �ִ� ���� ��ü�� ( ����� ���� )
#define MAX_GAME_OBJECTS 65535
#define DEF_GAME_USER_POOL_SIZE 8192
#define INC_GAME_USER_POOL_SIZE 1024

#define DEF_GAME_PARTY_POOL_SIZE 4096
#define INC_GAME_PARTY_POOL_SIZE 1024

#define INC_SEND_IO_POOL_SIZE 8192
#define DEF_SEND_IO_POOL_SIZE 32768

//-------------------------------------------------------------------------------------------------
STBDATA g_TblHAIR;
STBDATA g_TblFACE;
STBDATA g_TblARMOR;
STBDATA g_TblGAUNTLET;
STBDATA g_TblBOOTS;
STBDATA g_TblHELMET;
STBDATA g_TblWEAPON;
STBDATA g_TblSUBWPN;
STBDATA g_TblEFFECT;
STBDATA g_TblNPC;
STBDATA g_TblAniTYPE;
STBDATA g_TblPRODUCT;
STBDATA g_TblNATUAL;

STBDATA g_TblFACEITEM;
STBDATA g_TblUSEITEM;
STBDATA g_TblBACKITEM;
STBDATA g_TblGEMITEM;
STBDATA g_TblQUESTITEM;
STBDATA g_TblJEWELITEM;

STBDATA g_TblDropITEM;

STBDATA g_TblStore;

STBDATA g_TblWARP;
STBDATA g_TblEVENT;

STBDATA g_TblZONE;

STBDATA* g_pTblSTBs[ITEM_TYPE_RIDE_PART + 1];

STBDATA g_TblAVATAR; // �ƹ�Ÿ �ʱ� ���� ����Ÿ..
STBDATA g_TblSTATE; /// ĳ������ ���¸� �����ϴ� ����.

STBDATA g_TblUnion;
STBDATA g_TblClass;
STBDATA g_TblItemGRADE;

STBDATA g_TblSkillPoint;
STBDATA g_TblATTR;

// classMYSQL       g_MySQL;

CObjMNG* g_pObjMGR = NULL;
CAI_LIST g_AI_LIST;

CUserLIST* g_pUserLIST;
CZoneLIST* g_pZoneLIST;
CCharDatLIST* g_pCharDATA;
CMotionLIST g_MotionFILE;
CPartyBUFF* g_pPartyBUFF;

GS_lsvSOCKET* g_pSockLSV = NULL;
GS_asvSOCKET* g_pSockASV = NULL;

GS_CThreadSQL* g_pThreadSQL = NULL;
GS_CThreadLOG* g_pThreadLOG = NULL;

CThreadGUILD* g_pThreadGUILD = NULL;

GS_CThreadMALL* g_pThreadMALL = NULL;

char g_szURL[MAX_PATH];

#define BASE_DATA_DIR m_BaseDataDIR.Get() //	"..\\..\\sho\\"

CLIB_GameSRV* CLIB_GameSRV::m_pInstance = NULL;

FILE* g_fpTXT = NULL;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#define WORLD_TIME_TICK 10000 // 10 sec

enum eUserCntIDX { USERCNT_HG = 0, USERCNT_EX, USERCNT_BB, USERCNT_GL, USERCNT_MAX };
int g_iUserCount[USERCNT_MAX];

VOID CALLBACK
GS_TimerProc(HWND hwnd /* handle to window */,
    UINT uMsg /* WM_TIMER message */,
    UINT_PTR idEvent /* timer identifier */,
    DWORD dwTime /* current system time */) {
    switch (idEvent) {
        case GS_TIMER_ASV: {
            CLIB_GameSRV* pGS = CLIB_GameSRV::GetInstance();
            if (pGS) {
                pGS->ConnectToASV();
            }
            break;
        }

        case GS_TIMER_LSV: {
            CLIB_GameSRV* pGS = CLIB_GameSRV::GetInstance();
            if (pGS) {
                pGS->ConnectToLSV();
            }
            break;
        }

        case GS_TIMER_LOG: {
            CLIB_GameSRV* pGS = CLIB_GameSRV::GetInstance();
            if (pGS) {
                pGS->ConnectToLOG();
            }
            break;
        }

        case GS_TIMER_WORLD_TIME: {
            g_pZoneLIST->Inc_WorldTIME();

            switch (g_pZoneLIST->m_dwAccTIME % 6) {
                case 0:
                case 3: // 30�ʿ� �ѹ��� üũ...
                    g_pUserLIST->Check_SocketALIVE();
                    break;
                case 1: // case 4 :
                    g_pUserLIST->CloseIdleSCOKET(90 * 1000);
                    break;
            }

            break;
        }
    };
}

//-------------------------------------------------------------------------------------------------
void
WriteLOG(char* szMSG) {
    LOG_INFO(szMSG);
}

#define TAG_HG 0x4840 // @H G
#define TAG_EX 0x4540 // @E X
#define TAG_BB 0x4240 // @B B
#define TAG_GL 0x4740 // @G L

void
IncUserCNT(int iUserCNT, classUSER* pUSER) {
    LOG_INFO("User connected, total count: %d", iUserCNT);
}

void
DecUserCNT(int iUserCNT, classUSER* pUSER) {
    LOG_INFO("User disconnected, total count: %d", iUserCNT);
}

#include "../SHO_GS.ver"

DWORD
GetServerBuildNO() {
    return BUILD_NUM;
}
DWORD g_dwStartTIME = 0;
DWORD
GetServerStartTIME() {
    return g_dwStartTIME;
}

//-------------------------------------------------------------------------------------------------
#define WM_LSVSOCK_MSG (WM_SOCKETWND_MSG + 0)
#define WM_ASVSOCK_MSG (WM_SOCKETWND_MSG + 1)
//#define	WM_LOGSOCK_MSG	( WM_SOCKETWND_MSG+1 )

#define USE_MY_SQL_AGENT 0

CLIB_GameSRV::CLIB_GameSRV() {
#if !defined(__SERVER)
    COMPILE_TIME_ASSERT(0);
#endif

    COMPILE_TIME_ASSERT(MAX_ZONE_USER_BUFF > 4096);
    COMPILE_TIME_ASSERT(sizeof(gsv_SELECT_CHAR) < 1024);
    COMPILE_TIME_ASSERT(sizeof(tagGrowAbility) < 384);
    COMPILE_TIME_ASSERT(
        (sizeof(__int64) + sizeof(tagITEM) * INVENTORY_TOTAL_SIZE) == sizeof(CInventory));
}

CLIB_GameSRV::~CLIB_GameSRV() {
    Shutdown();

    if (g_pThreadSQL) {
        g_pThreadSQL->Free();
        SAFE_DELETE(g_pThreadSQL);
    }
    if (g_pThreadLOG) { // �׻� g_pThreadSQL���� �ڿ�...
        g_pThreadLOG->Free();
        SAFE_DELETE(g_pThreadLOG);
    }

    if (g_pThreadGUILD) {
        g_pThreadGUILD->Free();
        SAFE_DELETE(g_pThreadGUILD);
    }
    if (g_pThreadMALL) {
        g_pThreadMALL->Free();
        SAFE_DELETE(g_pThreadMALL);
    }

    DisconnectFromLSV();
    DisconnectFromLOG();

    SAFE_DELETE(g_pUserLIST);
    SAFE_DELETE(g_pPartyBUFF);

    m_pInstance = NULL;

    SAFE_DELETE(g_pSockLSV);
    SAFE_DELETE(g_pSockASV);

    Free_BasicDATA();
    g_pCharDATA->Destroy();

    CPoolSENDIO::Destroy();

    CStr::Free();

    if (CSocketWND::GetInstance())
        CSocketWND::GetInstance()->Destroy();

    SAFE_DELETE_ARRAY(m_pCheckedLocalZONE);

    CSOCKET::Free();
}

void
CLIB_GameSRV::SystemINIT(HINSTANCE hInstance, char* szBaseDataDIR, int iLangType) {
    m_pInstance = this;

    if (iLangType < 0)
        iLangType = 0;

    m_bTestServer = true;
    m_iLangTYPE = iLangType;
    m_pCheckedLocalZONE = NULL;
    m_pWorldTIMER = NULL;

    CSOCKET::Init();
    CStr::Init();

    m_BaseDataDIR.Alloc((WORD)(strlen(szBaseDataDIR) + 3));
    m_BaseDataDIR.Printf("%s\\", szBaseDataDIR);

    CPoolSENDIO::Instance(DEF_SEND_IO_POOL_SIZE, INC_SEND_IO_POOL_SIZE);

    g_pCharDATA = CCharDatLIST::Instance();

    Load_BasicDATA();

    ::ZeroMemory(g_iUserCount, sizeof(int) * USERCNT_MAX);

    g_pSockLSV = new GS_lsvSOCKET;
    g_pSockASV = new GS_asvSOCKET;

    //	g_pSockLOG = new GS_logSOCKET( USE_MY_SQL_AGENT );

    g_pPartyBUFF = new CPartyBUFF(MAX_PARTY_BUFF);
    g_pUserLIST = new CUserLIST(DEF_GAME_USER_POOL_SIZE, INC_GAME_USER_POOL_SIZE);

    // Lsv, Log = 2
    CSocketWND* pSockWND = CSocketWND::InitInstance(hInstance, 2);
    if (pSockWND) {
        pSockWND->AddSocket(&g_pSockLSV->m_SockLSV, WM_LSVSOCK_MSG);
        pSockWND->AddSocket(&g_pSockASV->m_SockASV, WM_ASVSOCK_MSG);
    }

    this->InitLocalZone(true);
}

bool
CLIB_GameSRV::CheckSTB_UseITEM() {
    for (short nD = 0; nD < g_TblUSEITEM.m_nDataCnt; nD++) {
        if (USEITEM_COOLTIME_TYPE(nD) < 0)
            USEITEM_COOLTIME_TYPE(nD) = 0;
        else if (USEITEM_COOLTIME_TYPE(nD) >= MAX_USEITEM_COOLTIME_TYPE)
            USEITEM_COOLTIME_TYPE(nD) = 0;
        if (USEITEM_COOLTIME_DELAY(nD) < 0)
            USEITEM_COOLTIME_DELAY(nD) = 0;

        if (0 == USEITME_STATUS_STB(nD))
            continue;

        short nIngSTB = USEITME_STATUS_STB(nD);
        short nDuringTime;

        for (short nI = 0; nI < STATE_APPLY_ABILITY_CNT; nI++) {
            if (!STATE_APPLY_ABILITY_VALUE(nIngSTB, nI))
                continue;

            assert(0 != STATE_APPLY_ABILITY_VALUE(nIngSTB, nI));

            // �� ������ ��ġ.
            nDuringTime = USEITEM_ADD_DATA_VALUE(nD) / STATE_APPLY_ABILITY_VALUE(nIngSTB, nI);
        }
    }

    return true;
}
bool
CLIB_GameSRV::CheckSTB_NPC() {
    t_HASHKEY HashKey;
    CQuestTRIGGER* pQuestTrigger;

    int iDropTYPE;
    for (short nI = 0; nI < g_TblNPC.m_nDataCnt; nI++) {
        if (NPC_LEVEL(nI) < 1)
            SET_NPC_LEVEL(nI, 1);
        if (NPC_ATK_SPEED(nI) <= 0)
            SET_NPC_ATK_SPEED(nI, 100);
        if (NPC_DROP_TYPE(nI) >= g_TblDropITEM.m_nDataCnt) {
            iDropTYPE = NPC_DROP_TYPE(nI);
            SET_NPC_DROP_TYPE(nI, 0);

            assert(NPC_DROP_TYPE(nI) < g_TblDropITEM.m_nDataCnt);
        }

        if (nI && NPC_DEAD_EVENT(nI)) {
            HashKey = ::StrToHashKey(NPC_DEAD_EVENT(nI));
            pQuestTrigger = g_QuestList.GetQuest(HashKey);
            if (!pQuestTrigger) {
                SET_NPC_DEAD_EVENT(nI, NULL);
            } else {
                do {
                    pQuestTrigger->m_iOwerNpcIDX = nI; // ������ �߻��Ǵ� Ʈ���Ŵ�.
                    pQuestTrigger = pQuestTrigger->m_pNextTrigger;
                } while (pQuestTrigger);
            }
        }
    }
    return true;
}

bool
CLIB_GameSRV::CheckSTB_DropITEM() {
    //	/*
    //������ ��� ������� �ٲ�鼭 stb���� ���� Ʋ������.
    int iDropITEM;
    tagITEM sITEM;

    for (short nI = 1; nI < g_TblDropITEM.m_nDataCnt; nI++) {
        for (short nC = 1; nC < g_TblDropITEM.m_nColCnt; nC++) {
            iDropITEM = DROPITEM_ITEMNO(nI, nC);
            if (iDropITEM <= 0)
                continue;

            sITEM.m_cType = (BYTE)(iDropITEM / 1000);
            sITEM.m_nItemNo = iDropITEM % 1000;

            if ((sITEM.m_cType < ITEM_TYPE_FACE_ITEM || sITEM.m_cType > ITEM_TYPE_RIDE_PART
                    || sITEM.m_cType == ITEM_TYPE_QUEST)
                && iDropITEM > 1000) {
                DROPITEM_ITEMNO(nI, nC) = 0;
                continue;
            }

            if (iDropITEM <= 1000) {
                if (iDropITEM >= 1 && iDropITEM <= 4) {
                    // �ٽ� ���
                    int iDropTblIDX = 26 + (iDropITEM * 5) + 4 /*RANDOM(5)�� �ִ밪 4 */;
                    if (iDropTblIDX >= g_TblDropITEM.m_nColCnt) {
                        // ���̺� �÷� ���� �ʰ�...
                        g_LOG.CS_ODS(0xffff, "This drop item[ %d %d ] may be too big\n", nI, nC);
                    }
                    continue;
                }
                DROPITEM_ITEMNO(nI, nC) = 0;
                continue;
            }

            if (sITEM.m_nItemNo > g_pTblSTBs[sITEM.m_cType]->m_nDataCnt) {
                DROPITEM_ITEMNO(nI, nC) = 0;
                continue;
            }

            assert(DROPITEM_ITEMNO(nI, nC) > 1000);
        }
    }
    //	*/

    return true;
}
bool
CLIB_GameSRV::CheckSTB_ItemRateTYPE() {
    short nD, nRateTYPE;

    for (nD = 0; nD < g_TblUSEITEM.m_nDataCnt; nD++) {
        nRateTYPE = ITEM_RATE_TYPE(ITEM_TYPE_USE, nD);
        assert(nRateTYPE >= 0 && nRateTYPE < MAX_PRICE_TYPE);
    }

    for (nD = 0; nD < g_TblNATUAL.m_nDataCnt; nD++) {
        nRateTYPE = ITEM_RATE_TYPE(ITEM_TYPE_NATURAL, nD);
        assert(nRateTYPE >= 0 && nRateTYPE < MAX_PRICE_TYPE);
    }
    return true;
}
bool
CLIB_GameSRV::CheckSTB_Motion() {
    short nX, nY, nFileIDX;

    // type_motion.stb
    for (nY = 0; nY < g_TblAniTYPE.m_nDataCnt; nY++) {
        for (nX = 0; nX < g_TblAniTYPE.m_nColCnt; nX++) {
            nFileIDX = FILE_MOTION(nX, nY);
            if (!nFileIDX)
                continue;
        }
    }
    return true;
}

bool
CLIB_GameSRV::CheckSTB_GemITEM() {
    short nType, nValue;

    for (short nC = 0; nC < g_TblGEMITEM.m_nDataCnt; nC++) {
        for (short nI = 0; nI < 2; nI++) {
            nType = GEMITEM_ADD_DATA_TYPE(nC, nI);
            nValue = GEMITEM_ADD_DATA_VALUE(nC, nI);

            _ASSERT(nType >= 0 && nType <= AT_MAX);
        }
    }
    return true;
}

bool
CLIB_GameSRV::CheckSTB_ListPRODUCT() {
    tagITEM sOutITEM;
    for (short nI = 0; nI < g_TblPRODUCT.m_nDataCnt; nI++) {
        for (short nS = 0; nS < 4; nS++) {
            if (PRODUCT_NEED_ITEM_NO(nI, nS)) {
                sOutITEM.Init(PRODUCT_NEED_ITEM_NO(nI, nS), 1);
                if (!sOutITEM.IsValidITEM()) {
                    _ASSERT(0);
                }
                // ��� ������ ��ȣ
                _ASSERT(PRODUCT_NEED_ITEM_CNT(nI, nS) > 0); // �ӿ� ����
            }
        }
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
void
CLIB_GameSRV::TranslateNameWithDescKey(STBDATA* pOri, char* szStbFile, int iLangCol) {
    int iKeyCol = 0;

    STBDATA tmpTBL;
    // ������ �̸��� ���� :: �̸� �÷��� 1, 3, 5 ... ���� �÷��� 2, 4, 6 ...
    if (!tmpTBL.LoadWSTB(
            true, CStr::Printf("%s%s", BASE_DATA_DIR, szStbFile), iKeyCol, iLangCol, -1))
        return;
    STBDATA* pLang = &tmpTBL;

    char* szKey;
    int iIdx;
    for (int iL = 0; iL < pOri->m_nDataCnt; iL++) {
        szKey = pOri->m_ppDESC[iL];
        if (!szKey)
            continue;

        iIdx = pLang->GetRowIndex(szKey);
        if (iIdx && pLang->m_ppVALUE[iIdx][iLangCol].GetStrLEN()) {
            SAFE_DELETE(pOri->m_ppNAME[iL]);
            pOri->m_ppNAME[iL] = new char[pLang->m_ppVALUE[iIdx][iLangCol].GetStrLEN() + 1];
            strcpy(pOri->m_ppNAME[iL], pLang->m_ppVALUE[iIdx][iLangCol].GetSTR());
            // pOri->m_ppNAME[ iL ][ pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetStrLEN() ] = 0;
        }
    }

    tmpTBL.Free();
}

void
CLIB_GameSRV::TranslateNameWithColoumKey(STBDATA* pOri,
    char* szStbFile,
    int iLangCol,
    int iNameCol,
    int iDescCol) {
    int iKeyCol = 0;

    STBDATA tmpTBL;

    // ������ �̸��� ���� :: �̸� �÷��� 1, 3, 5 ... ���� �÷��� 2, 4, 6 ...
    if (!tmpTBL.LoadWSTB(
            true, CStr::Printf("%s%s", BASE_DATA_DIR, szStbFile), iKeyCol, iLangCol, -1))
        return;
    STBDATA* pLang = &tmpTBL;

    char* szKey;
    int iIdx;
    for (int iL = 1; iL < pOri->m_nDataCnt; iL++) {
        szKey = pOri->m_ppVALUE[iL][iDescCol].GetSTR();
        if (!szKey)
            continue;

        iIdx = pLang->GetRowIndex(szKey);
        if (iIdx && pLang->m_ppVALUE[iIdx][iLangCol].GetStrLEN()) {
            // szStr = pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR();
            // while( *szStr ) {
            //	if ( *szStr == '\'' )
            //		*szStr = '`';
            //	szStr ++;
            //}
            // LogString(0xffff," %s :: %d:%d : %s/%s\n", szKey, iL, iIdx, pOri->m_ppVALUE[ iL ][
            // iNameCol ].GetSTR(), pLang->m_ppVALUE[ iIdx ][ iLangCol ].GetSTR() );

            pOri->m_ppVALUE[iL][iNameCol].SetVALUE(pLang->m_ppVALUE[iIdx][iLangCol].GetSTR());
        } else if (iIdx) {
#ifdef __KCHS_TEST
            LogString(0xffff,
                "Language string not found:: %d:%d / %s [ %d / %s ] \n",
                iL,
                iIdx,
                pOri->m_ppVALUE[iL][iNameCol].GetSTR(),
                pLang->m_ppVALUE[iIdx][iLangCol].GetStrLEN(),
                pLang->m_ppVALUE[iIdx][iLangCol].GetSTR());
#endif
        }
    }

    tmpTBL.Free();
}

//-------------------------------------------------------------------------------------------------

#include "IP_Addr.h"
bool
CLIB_GameSRV::Load_BasicDATA() {
    size_t ttt = sizeof(tagGrowAbility);

    if (!g_AI_LIST.Load(
            BASE_DATA_DIR, "3DDATA\\STB\\FILE_AI.STB", "3DDATA\\AI\\AI_s.STB", m_iLangTYPE))
        return false;

    g_TblHAIR.Load(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Hair.STB"), true, false);
    g_TblFACE.Load(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Face.STB"), true, false);
    g_TblARMOR.Load(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Body.STB"), true, true);

    g_TblGAUNTLET.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Arms.STB"), true, true);
    g_TblBOOTS.Load(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Foot.STB"), true, true);
    g_TblHELMET.Load(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Cap.STB"), true, true);

    g_TblWEAPON.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Weapon.STB"), true, true);
    g_TblSUBWPN.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SUBWPN.STB"), true, true);
    g_TblEFFECT.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_EFFECT.STB"), false, false);
    g_TblDropITEM.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\ITEM_DROP.STB"), false, false);

    g_TblPRODUCT.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PRODUCT.STB"), true, false);
    g_TblNATUAL.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_NATURAL.STB"), true, true);
    g_TblFACEITEM.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_FACEITEM.STB"), true, true);
    g_TblUSEITEM.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_USEITEM.STB"), true, true);
    g_TblBACKITEM.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_BACK.STB"), true, true);
    g_TblGEMITEM.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEMITEM.STB"), true, true);
    g_TblJEWELITEM.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEWEL.STB"), true, true);
    g_TblQUESTITEM.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_QUESTITEM.STB"), true, true);
    g_TblStore.Load(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SELL.STB"), true, false);

    g_TblAniTYPE.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\TYPE_MOTION.STB"), false, false);

    g_TblEVENT.Load2(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_EVENT.STB"), false, true);

    g_TblWARP.Load2(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\WARP.STB"), true, false);
    g_TblZONE.Load2(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_ZONE.STB"), true, false);
    assert(g_TblZONE.m_nColCnt > 33);

    g_TblNPC.Load2(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_NPC.STB"), true, false);
    //	assert( g_TblNPC.m_nColCnt > 43 );

    g_TblAVATAR.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\INIT_AVATAR.STB"), false, false);
    g_TblSTATE.Load2(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_STATUS.STB"), true, false);

    g_TblUnion.Load2(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_UNION.STB"), false, false);
    g_TblClass.Load2(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_CLASS.STB"), false, false);

    g_TblItemGRADE.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_GRADE.STB"), false, false);
    g_TblSkillPoint.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Skill_P.STB"), false, false);
#ifdef __APPLY_2ND_JOB
    g_TblATTR.Load(
        CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_ATTRIBUTE.STB"), false, false);
#endif

    if (!g_MotionFILE.Load("3DDATA\\STB\\FILE_MOTION.stb", 0, BASE_DATA_DIR))
        return false;

    if (!g_QuestList.LoadQuestTable("3DDATA\\STB\\LIST_Quest.STB",
            "3DDATA\\STB\\LIST_QuestDATA.STB",
            BASE_DATA_DIR,
            "3DDATA\\QuestData\\Quest_s.STB",
            m_iLangTYPE))
        return false;

    if (!g_SkillList.LoadSkillTable(
            CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SKILL.STB")))
        return false;

    if (!g_pCharDATA->Load_MOBorNPC(BASE_DATA_DIR, "3DDATA\\NPC\\LIST_NPC.CHR")) {
        assert(0 && "LIST_NPC.chr loading error");
        return false;
    }

    if (!g_PatITEM.LoadPatITEM(CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PAT.STB")))
        return false;

    g_pTblSTBs[ITEM_TYPE_FACE_ITEM] = &g_TblFACEITEM;
    g_pTblSTBs[ITEM_TYPE_HELMET] = &g_TblHELMET;
    g_pTblSTBs[ITEM_TYPE_ARMOR] = &g_TblARMOR;
    g_pTblSTBs[ITEM_TYPE_GAUNTLET] = &g_TblGAUNTLET;
    g_pTblSTBs[ITEM_TYPE_BOOTS] = &g_TblBOOTS;
    g_pTblSTBs[ITEM_TYPE_KNAPSACK] = &g_TblBACKITEM;
    g_pTblSTBs[ITEM_TYPE_JEWEL] = &g_TblJEWELITEM;
    g_pTblSTBs[ITEM_TYPE_WEAPON] = &g_TblWEAPON;
    g_pTblSTBs[ITEM_TYPE_SUBWPN] = &g_TblSUBWPN;
    g_pTblSTBs[ITEM_TYPE_USE] = &g_TblUSEITEM;
    g_pTblSTBs[ITEM_TYPE_GEM] = &g_TblGEMITEM;
    g_pTblSTBs[ITEM_TYPE_NATURAL] = &g_TblNATUAL;
    g_pTblSTBs[ITEM_TYPE_QUEST] = &g_TblQUESTITEM;
    g_pTblSTBs[ITEM_TYPE_RIDE_PART] = &g_PatITEM.m_ItemDATA;

    // ������ �̸��� ���� :: �̸� �÷��� 1, 3, 5 ... ���� �÷��� 2, 4, 6 ...
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_FACE_ITEM], "Language\\LIST_FACEITEM_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_HELMET], "Language\\LIST_CAP_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_ARMOR], "Language\\LIST_BODY_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_GAUNTLET], "Language\\LIST_ARMS_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_BOOTS], "Language\\LIST_FOOT_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_KNAPSACK], "Language\\LIST_BACK_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_JEWEL], "Language\\LIST_JEWEL_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_WEAPON], "Language\\LIST_WEAPON_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_SUBWPN], "Language\\LIST_SUBWPN_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_USE], "Language\\LIST_USEITEM_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_GEM], "Language\\LIST_JEMITEM_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_NATURAL], "Language\\LIST_NATURAL_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_QUEST], "Language\\LIST_QUESTITEM_s.STB", 1 + 2 * m_iLangTYPE);
    this->TranslateNameWithDescKey(
        g_pTblSTBs[ITEM_TYPE_RIDE_PART], "Language\\LIST_PAT_s.STB", 1 + 2 * m_iLangTYPE);

    this->TranslateNameWithDescKey(
        &g_SkillList.m_SkillDATA, "Language\\LIST_SKILL_s.stb", 1 + 2 * m_iLangTYPE);

    // �̸� �÷��� 1,2,3 ...
    this->TranslateNameWithColoumKey(&g_TblNPC,
        "Language\\LIST_NPC_s.STB",
        1 + m_iLangTYPE,
        NPC_NAME_STR_COLUMN,
        NPC_STRING_ID_COLOUM);

#define ZONE_NAME_STR_COLUMN 0
#define ZONE_KEY_STR_COLOUM 26
    //	assert( g_TblZONE.m_nColCnt == 30 );
    this->TranslateNameWithColoumKey(&g_TblZONE,
        "Language\\LIST_ZONE_s.STB",
        1 + 2 * m_iLangTYPE,
        ZONE_NAME_STR_COLUMN,
        ZONE_KEY_STR_COLOUM);

    //	CheckSTB_AllITEM ();
    CheckSTB_UseITEM();
    CheckSTB_DropITEM();
    CheckSTB_NPC();

    CheckSTB_ItemRateTYPE();
    CheckSTB_Motion();
    CheckSTB_GemITEM();
    CheckSTB_ListPRODUCT();

    return true;
}
void
CLIB_GameSRV::Free_BasicDATA() {
    // STBDATA�� �ڵ� Ǯ��..
    g_PatITEM.Free();
    g_QuestList.Free();
    g_SkillList.Free();
    g_MotionFILE.Free();
    g_AI_LIST.Free();
}

//-------------------------------------------------------------------------------------------------
char*
CLIB_GameSRV::GetZoneName(short nZoneNO) {
    if (nZoneNO > 0 && nZoneNO < g_TblZONE.m_nDataCnt) {
        if (nZoneNO >= TEST_ZONE_NO)
            return NULL;

        if (ZONE_NAME(nZoneNO) && ZONE_FILE(nZoneNO)) {
            char* szZoneFILE = CStr::Printf("%s%s", BASE_DATA_DIR, ZONE_FILE(nZoneNO));
            if (CUtil::Is_FileExist(szZoneFILE)) {
                return ZONE_NAME(nZoneNO);
            }
        }
    }
    return NULL;
}

short
CLIB_GameSRV::InitLocalZone(bool bAllActive) {
    SAFE_DELETE_ARRAY(m_pCheckedLocalZONE);

    m_pCheckedLocalZONE = new bool[g_TblZONE.m_nDataCnt];

    ::FillMemory(m_pCheckedLocalZONE, sizeof(bool) * g_TblZONE.m_nDataCnt, bAllActive);

    for (short nI = TEST_ZONE_NO; nI < g_TblZONE.m_nDataCnt; nI++) {
        m_pCheckedLocalZONE[nI] = false;
    }

    return g_TblZONE.m_nDataCnt;
}
bool
CLIB_GameSRV::CheckZoneToLocal(short nZoneNO, bool bChecked) {
    if (nZoneNO > 0 && nZoneNO < g_TblZONE.m_nDataCnt) {
        if (nZoneNO >= TEST_ZONE_NO)
            return false;

        m_pCheckedLocalZONE[nZoneNO] = bChecked;
        return m_pCheckedLocalZONE[nZoneNO];
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
#define IS_SIGNED(type) (((type)(-1)) < ((type)0))
#define IS_UNSIGNED(type) (((type)(-1)) > ((type)0))

#ifndef COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT(expr) \
    { typedef int compile_time_assert_fail[1 - 2 * !(expr)]; }
#endif

//-------------------------------------------------------------------------------------------------
bool
CLIB_GameSRV::ConnectSERVER(char* szDBServerIP,
    char* szDBName,
    char* szDBUser,
    char* szDBPassword,
    char* szLogUser,
    char* szLogPW,
    char* szMallUser,
    char* szMallPW,
    char* szLoginServerIP,
    int iLoginServerPort,
    char* szAccountServerIP,
    int iAccountServerPortNO) {
    if (NULL == this->GetInstance())
        return false;

    m_AccountServerIP.Set(szAccountServerIP);
    m_iAccountServerPORT = iAccountServerPortNO;

    m_LoginServerIP.Set(szLoginServerIP);
    m_iLoginServerPORT = iLoginServerPort;

    //	char *szDBUser     = "icarus";
    //	char *szDBPassword = "1111";

    m_DBServerIP.Set(szDBServerIP);
    m_DBName.Set(szDBName);
    m_DBUser.Set(szDBUser);
    m_DBPassword.Set(szDBPassword);

    if (*szLogUser == '?') {
        m_LogUser.Set(szDBUser);
        m_LogPW.Set(szDBPassword);
    } else {
        m_LogUser.Set(szLogUser);
        m_LogPW.Set(szLogPW);
    }

    g_pThreadSQL = new GS_CThreadSQL; // suspend ���� ���۵�.
    if (!g_pThreadSQL->Connect(USE_MY_SQL_AGENT ? USE_MY_SQL : USE_ODBC,
            szDBServerIP,
            szDBUser,
            szDBPassword,
            szDBName,
            32,
            1024 * 8)) {
        return false;
    }
    g_pThreadSQL->Resume();

    CStrVAR stLogODBC;
    stLogODBC.Set("SHO_LOG");

    g_pThreadLOG = new GS_CThreadLOG;
    if (!g_pThreadLOG->Connect(USE_ODBC,
            szDBServerIP,
            m_LogUser.Get(),
            m_LogPW.Get(),
            stLogODBC.Get(),
            32,
            1024 * 8)) {
        return false;
    }
    g_pThreadLOG->Resume();

    g_pThreadGUILD = new CThreadGUILD(32, 16);
    if (!g_pThreadGUILD
        || !g_pThreadGUILD->Connect(USE_MY_SQL_AGENT ? USE_MY_SQL : USE_ODBC,
            (char*)szDBServerIP,
            szDBUser,
            szDBPassword,
            szDBName,
            32,
            1024 * 8)) {
        return false;
    }
    g_pThreadGUILD->Resume();

#define MALL_DB_IP "127.0.0.1"
#define MALL_DB_NAME "SHO_MALL"

    if (*szMallUser != '?') {
        g_pThreadMALL = new GS_CThreadMALL;
        if (!g_pThreadMALL->Connect(
                USE_ODBC, MALL_DB_IP, szMallUser, szMallPW, MALL_DB_NAME, 32, 1024 * 8)) {
            return false;
        }
        g_pThreadMALL->Resume();
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
CLIB_GameSRV::Start(HWND hMainWND,
    char* szServerName,
    char* szClientListenIP,
    int iClientListenPort,
    BYTE btChannelNO,
    BYTE btLowAge,
    BYTE btHighAge) {
    srand(timeGetTime());

    if (_strnicmp(szServerName, "TEST", 4))
        m_bTestServer = false;

    m_dwUserLIMIT = MAX_ZONE_USER_BUFF;

    g_pSockLSV->Init(CSocketWND::GetInstance()->GetWindowHandle(),
        m_LoginServerIP.Get(),
        m_iLoginServerPORT,
        WM_LSVSOCK_MSG);
    this->ConnectToLSV();

    g_pSockASV->Init(CSocketWND::GetInstance()->GetWindowHandle(),
        m_AccountServerIP.Get(),
        m_iAccountServerPORT,
        WM_ASVSOCK_MSG);
    this->ConnectToASV();
    this->ConnectToLOG();

    m_hMainWND = hMainWND;

    m_iListenPortNO = iClientListenPort;
    m_ServerNAME.Set(szServerName);
    m_btChannelNO = btChannelNO;
    m_btLowAGE = btLowAge;
    m_btHighAGE = btHighAge;
    m_ServerIP.Set(szClientListenIP);

    LogString(0xfff, "sizeof(classUSER)	== %d \n", sizeof(classUSER));
    LogString(0xfff, "sizeof(tagQuestData) == %d \n", sizeof(tagQuestData));

    COMPILE_TIME_ASSERT(sizeof(t_PACKETHEADER) == 6);
    COMPILE_TIME_ASSERT(sizeof(tagITEM) == (6 + sizeof(__int64)));

    COMPILE_TIME_ASSERT(sizeof(gsv_DAMAGE) == (sizeof(t_PACKETHEADER) + 6));
    COMPILE_TIME_ASSERT(sizeof(char) == 1);
    COMPILE_TIME_ASSERT(sizeof(short) >= 2);
    COMPILE_TIME_ASSERT(sizeof(long) >= 4);
    COMPILE_TIME_ASSERT(sizeof(int) >= sizeof(short));
    COMPILE_TIME_ASSERT(sizeof(long) >= sizeof(int));

    COMPILE_TIME_ASSERT(sizeof(gsv_INVENTORY_DATA) < MAX_PACKET_SIZE);
    //	COMPILE_TIME_ASSERT( sizeof( gsv_QUEST_ONLY ) < MAX_PACKET_SIZE );

    COMPILE_TIME_ASSERT(IS_SIGNED(long));
    COMPILE_TIME_ASSERT(IS_UNSIGNED(DWORD));

    this->Shutdown();
    g_dwStartTIME = classTIME::GetCurrentAbsSecond();

    LogString(LOG_NORMAL,
        "Size : %d / %d / %d \n",
        sizeof(t_PACKETHEADER),
        sizeof(t_PACKET),
        sizeof(classPACKET));

    g_pObjMGR = new CObjMNG(MAX_GAME_OBJECTS);
    g_pZoneLIST = CZoneLIST::Instance();

    g_pZoneLIST->InitZoneLIST(BASE_DATA_DIR);

    m_dwRandomSEED = ::timeGetTime();

    m_pWorldTIMER =
        new CTimer(m_hMainWND, GS_TIMER_WORLD_TIME, WORLD_TIME_TICK, (TIMERPROC)GS_TimerProc);
    m_pWorldTIMER->Start();

    g_pUserLIST->Active(m_iListenPortNO, MAX_ZONE_USER_BUFF, 5 * 60); // 5�� ���.

    return true;
}

//-------------------------------------------------------------------------------------------------
void
CLIB_GameSRV::Shutdown() {
    SAFE_DELETE(m_pWorldTIMER); // Ÿ�̸� ������ �ռ�����...

    g_pUserLIST->ShutdownACCEPT();

    if (g_pZoneLIST) {
        g_pZoneLIST->Destroy();
        g_pZoneLIST = NULL;
    }

    g_pUserLIST->ShutdownWORKER();
    g_pUserLIST->ShutdownSOCKET();

    // sql thread�� ��� ������ ��� �ɵ��� ���...
    if (g_pThreadSQL) {
        _ASSERT(g_pThreadSQL);
        g_pThreadSQL->Set_EVENT();
        do {
            ::Sleep(200); // wait 0.2 sec
        } while (
            !g_pThreadSQL->IsWaiting() || g_pThreadSQL->WaitUserCNT() > 0); // ó�����̸� ���..
    }

    SAFE_DELETE(g_pObjMGR);
    //	SAFE_DELETE( m_pWorldTIMER );
}

//-------------------------------------------------------------------------------------------------
bool
CLIB_GameSRV::ConnectToLSV() {
    return g_pSockLSV->Connect();
}
void
CLIB_GameSRV::DisconnectFromLSV() {
    g_pSockLSV->Disconnect();
}

bool
CLIB_GameSRV::ConnectToASV() {
    return g_pSockASV->Connect();
}
void
CLIB_GameSRV::DisconnectFromASV() {
    g_pSockASV->Disconnect();
}

//-------------------------------------------------------------------------------------------------
bool
CLIB_GameSRV::ConnectToLOG() {
    //	return g_pSockLOG->Connect();
    return true;
}
void
CLIB_GameSRV::DisconnectFromLOG() {
    //	g_pSockLOG->Disconnect ();
}

//-------------------------------------------------------------------------------------------------
void
CLIB_GameSRV::Send_ANNOUNCE(short nZoneNO, char* szMsg) {
    if (nZoneNO)
        g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT(nZoneNO, szMsg, NULL);
    else
        g_pZoneLIST->Send_gsv_ANNOUNCE_CHAT(szMsg, NULL);
}

//-------------------------------------------------------------------------------------------------
void
CLIB_GameSRV::Set_UserLIMIT(DWORD dwUserLimit) {
    m_dwUserLIMIT = dwUserLimit;

    if (g_pSockLSV) {
        g_pSockLSV->Send_srv_USER_LIMIT(dwUserLimit);
    }
}

//-------------------------------------------------------------------------------------------------
