#include "stdAFX.h"

#include "CWS_Client.h"
#include "CWS_Server.h"
#include "IO_PAT.h"
#include "IO_STB.h"
#include "SHO_WS_LIB.h"
#include "WS_SocketLSV.h"
#include "WS_ThreadSQL.h"
#include "WS_ZoneLIST.h"

#include "CChatROOM.h"
#include "CThreadGUILD.h"
#include "CThreadMSGR.h"
#include "IO_Skill.h"

#include "rose/common/server_config.h"

using namespace Rose::Common;

STBDATA g_TblHAIR;
STBDATA g_TblFACE;
STBDATA g_TblARMOR;
STBDATA g_TblGAUNTLET;
STBDATA g_TblBOOTS;
STBDATA g_TblHELMET;
STBDATA g_TblWEAPON;
STBDATA g_TblSUBWPN;

STBDATA g_TblFACEITEM;
STBDATA g_TblUSEITEM;
STBDATA g_TblBACKITEM;
STBDATA g_TblGEMITEM;
STBDATA g_TblJEWELITEM;
STBDATA g_TblNATUAL;
STBDATA g_TblQUESTITEM;

/*
STBDATA			 g_TblEFFECT;
STBDATA			 g_TblNPC;
STBDATA			 g_TblAniTYPE;
STBDATA			 g_TblPRODUCT;
STBDATA			 g_TblDropITEM;
STBDATA          g_TblStore;
*/

STBDATA g_TblAVATAR; // �ƹ�Ÿ �ʱ� ���� ����Ÿ..
STBDATA g_TblWARP;
STBDATA g_TblZONE;
STBDATA* g_pTblSTBs[ITEM_TYPE_RIDE_PART + 1];

//#define	BASE_DATA_DIR	"..\\..\\sho\\"
#define BASE_DATA_DIR m_BaseDataDIR.Get() //	"..\\..\\sho\\"

WS_ZoneLIST g_ZoneLIST;

//-------------------------------------------------------------------------------------------------

SHO_WS* SHO_WS::m_pInstance = NULL;

CWS_ThreadSQL* g_pThreadSQL = NULL;

CWS_ListSERVER* g_pListSERVER = NULL;
CWS_ListCLIENT* g_pUserLIST = NULL;

WS_lsvSOCKET* g_pSockLSV = NULL;
// WS_logSOCKET	*g_pSockLOG = NULL;

CChatRoomLIST* g_pChatROOMs = NULL;
// CWSPartyBUFF	*g_pPartyBUFF=NULL;

CThreadMSGR* g_pThreadMSGR = NULL;
CThreadGUILD* g_pThreadGUILD = NULL;

//-------------------------------------------------------------------------------------------------

#define DEF_CLIENT_POOL_SIZE 0x7fff // 32767	���� ���� ��� ����
#define INC_CLIENT_POOL_SIZE 0xfff // 4095

#define DEF_SERVER_POOL_SIZE 64
#define INC_SERVER_POOL_SIZE 16

#define DEF_SEND_IO_POOL_SIZE (DEF_CLIENT_POOL_SIZE + INC_CLIENT_POOL_SIZE)
#define INC_SEND_IO_POOL_SIZE (INC_CLIENT_POOL_SIZE)

//-------------------------------------------------------------------------------------------------
#define WM_LSVSOCK_MSG (WM_SOCKETWND_MSG + 0)
#define WM_LOGSOCK_MSG (WM_SOCKETWND_MSG + 1)

#define MAX_CHAT_ROOMS 150

bool
SHO_WS::ConnectToLOG() {
    //	return g_pSockLOG->Connect();
    return true;
}
void
SHO_WS::DisconnectFromLOG() {
    //	g_pSockLOG->Disconnect ();
}
bool
SHO_WS::ConnectToLSV() {
    return g_pSockLSV->Connect();
}
void
SHO_WS::DisconnectFromLSV() {
    g_pSockLSV->Disconnect();
}

#define USE_MY_SQL_AGENT 0

#include "../SHO_WS.ver"
DWORD
GetServerBuildNO() {
    return BUILD_NUM;
}
DWORD g_dwStartTIME = 0;
DWORD
GetServerStartTIME() {
    return g_dwStartTIME;
}

SHO_WS::SHO_WS() {

#if (_WIN32_WINNT < 0x0500) || !defined(__SERVER)
    COMPILE_TIME_ASSERT(0);
#endif
    CSOCKET::Init();
    CStr::Init();
}
void
SHO_WS::SystemINIT(HINSTANCE hInstance, char* szBaseDataDIR, int iLangType) {
    if (iLangType < 0)
        iLangType = 0;
    m_iLangTYPE = iLangType;

    m_BaseDataDIR.Alloc(strlen(szBaseDataDIR) + 3);
    m_BaseDataDIR.Printf((char*)"%s\\", szBaseDataDIR);

    g_pSockLSV = new WS_lsvSOCKET;
    //	g_pSockLOG = new WS_logSOCKET( USE_MY_SQL_AGENT );

    // �α��� ������ ������ Ŭ���̾�Ʈ ���� ����...
    CSocketWND* pSockWND = CSocketWND::InitInstance(hInstance, 2);
    if (pSockWND) {
        // pSockWND->AddSocketMsgProc( WM_LSVSOCK_MSG, LSV_SocketMSG );
        pSockWND->AddSocket(&g_pSockLSV->m_SockLSV, WM_LSVSOCK_MSG);
    }

    //	g_pPartyBUFF  = new CWSPartyBUFF;

    g_pUserLIST = new CWS_ListCLIENT(DEF_CLIENT_POOL_SIZE, INC_CLIENT_POOL_SIZE);
    g_pListSERVER = new CWS_ListSERVER(DEF_SERVER_POOL_SIZE, INC_SERVER_POOL_SIZE);

    CPoolSENDIO::Instance(DEF_SEND_IO_POOL_SIZE, INC_SEND_IO_POOL_SIZE);

    Load_BasicDATA();

    m_pWorldTIMER = NULL;
}

SHO_WS::~SHO_WS() {
    if (g_pThreadSQL) {
        g_pThreadSQL->Free();
        SAFE_DELETE(g_pThreadSQL);
    }

    if (g_pThreadMSGR) {
        g_pThreadMSGR->Free();
        SAFE_DELETE(g_pThreadMSGR);
    }
    if (g_pThreadGUILD) {
        g_pThreadGUILD->Free();
        SAFE_DELETE(g_pThreadGUILD);
    }

    SAFE_DELETE(g_pUserLIST);
    SAFE_DELETE(g_pListSERVER);

    //	SAFE_DELETE( g_pPartyBUFF );

    CPoolSENDIO::Destroy();

    //	SAFE_DELETE( g_pSockLOG );
    SAFE_DELETE(g_pSockLSV);

    if (CSocketWND::GetInstance())
        CSocketWND::GetInstance()->Destroy();

    CStr::Free();
    CSOCKET::Free();
}

//-------------------------------------------------------------------------------------------------
// ���� �ʱ�ȭ
// 1. ��� ���� ����
// 2. �� ���� ���� ���
// 3. ����� ���� ���..
// 4. �α��� ���� ����
bool
SHO_WS::connect_database(DatabaseConfig& config) {
    if (NULL == this->GetInstance())
        return false;

    char* ip = (char*)config.ip.c_str();
    char* name = (char*)config.name.c_str();
    char* username = (char*)config.username.c_str();
    char* password = (char*)config.password.c_str();

    std::string log_db = config.name;
    if (log_db.back() == '\0') {
        log_db.pop_back();
    }
    log_db += "_log";

    m_DBServerIP.Set(ip);
    m_DBName.Set(name);
    m_DBUser.Set(username);
    m_DBPassword.Set(password);

    m_LogDBUser.Set(name);
    m_LogDBPassword.Set(password);

    g_pThreadSQL = new CWS_ThreadSQL; // suspend ���� ���۵�.
    if (!g_pThreadSQL->Connect(USE_MY_SQL_AGENT ? USE_MY_SQL : USE_ODBC,
            ip,
            username,
            password,
            name,
            32,
            1024 * 8)) {
        return false;
    }
    if (!g_pThreadSQL->db_pg.connect(config.connection_string)) {
        std::string error_message = g_pThreadSQL->db_pg.last_error_message();
        LOG_ERROR("Failed to connect to the database: {}", error_message.c_str());
        return false;
    }
    g_pThreadSQL->Resume();

    g_pThreadMSGR = new CThreadMSGR(16384, 1024);
    if (!g_pThreadMSGR->Connect(USE_MY_SQL_AGENT ? USE_MY_SQL : USE_ODBC,
            ip,
            username,
            password,
            name,
            32,
            1024 * 8)) {
        return false;
    }
    if(!g_pThreadMSGR->db_pg.connect(config.connection_string)) {
        std::string error_message = g_pThreadSQL->db_pg.last_error_message();
        LOG_ERROR("Failed to connect to the database: {}", error_message.c_str());
        return false;
    }
    g_pThreadMSGR->Resume();

    g_pThreadGUILD = new CThreadGUILD(8192, 512);
    if (!g_pThreadGUILD->Connect(USE_MY_SQL_AGENT ? USE_MY_SQL : USE_ODBC,
            ip,
            username,
            password,
            name,
            32,
            1024 * 8)) {
        return false;
    }
    g_pThreadGUILD->Resume();

    g_pThreadSQL->Load_WORLDVAR(g_ZoneLIST.m_nWorldVAR, MAX_WORLD_VAR_CNT);

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
SHO_WS::Start(HWND hMainWND,
    char* szLoginServerIP,
    int iLoginServerPort,
    char* szLogServerIP,
    int iLogServerPortNO,
    char* szWorldName,
    int iServerListenPortNO,
    int iClientListenPortNO,
    bool bBlockCreateCHAR) {
    g_dwStartTIME = classTIME::GetCurrentAbsSecond();
    m_hMainWND = hMainWND;

    m_bBlockCreateCHAR = bBlockCreateCHAR;
    m_iClientListenPortNO = iClientListenPortNO;
    m_iServerListenPortNO = iServerListenPortNO;

    m_ServerNAME.Set(szWorldName);
    m_ServerIP.Set(szLoginServerIP);

    g_pChatROOMs = new CChatRoomLIST(MAX_CHAT_ROOMS);

    g_pUserLIST->Active(m_iClientListenPortNO, MAX_WORLD_USER_UBFF, 5 * 60); // 5�� ���
    g_pListSERVER->Active(m_iServerListenPortNO, 512, 3 * 60); // 3�� ���

    m_dwRandomSEED = ::timeGetTime();

    this->ConnectToLOG();

    g_pSockLSV->Init(CSocketWND::GetInstance()->GetWindowHandle(),
        szLoginServerIP,
        iLoginServerPort,
        WM_LSVSOCK_MSG,
        true);

    this->ConnectToLSV();

    m_pWorldTIMER =
        new CTimer(m_hMainWND, WS_TIMER_WORLD_TIME, WORLD_TIME_TICK, (TIMERPROC)WS_TimerProc);
    m_pWorldTIMER->Start();

    return true;
}

//-------------------------------------------------------------------------------------------------
void
SHO_WS::Active(bool bActive) {
    g_pSockLSV->Send_srv_ACTIVE_MODE(bActive);
}

//-------------------------------------------------------------------------------------------------
void
SHO_WS::Shutdown() {
    SAFE_DELETE(m_pWorldTIMER);

    DisconnectFromLSV();

    g_pUserLIST->Shutdown();
    g_pListSERVER->Shutdown();

    SAFE_DELETE(g_pChatROOMs); // ���� ��������...
}

//-------------------------------------------------------------------------------------------------
void
SHO_WS::StartCLI_SOCKET() {
    g_pUserLIST->Active(m_iClientListenPortNO, MAX_WORLD_USER_UBFF, 5 * 60); // 5�� ���
}

void
SHO_WS::ShutdownCLI_SOCKET() {
    g_pUserLIST->Shutdown();
}

bool
SHO_WS::Load_BasicDATA() {
    g_TblHAIR.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Hair.STB"),
        true,
        false);
    g_TblFACE.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Face.STB"),
        true,
        false);
    g_TblARMOR.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Body.STB"),
        true,
        true);

    g_TblGAUNTLET.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Arms.STB"),
        true,
        true);
    g_TblBOOTS.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Foot.STB"),
        true,
        true);
    g_TblHELMET.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Cap.STB"),
        true,
        true);

    g_TblWEAPON.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_Weapon.STB"),
        true,
        true);
    g_TblSUBWPN.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SUBWPN.STB"),
        true,
        true);

    //	g_TblPRODUCT.Load	( CStr::Printf("%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PRODUCT.STB"),
    // true, false );
    g_TblNATUAL.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_NATURAL.STB"),
        true,
        true);

    g_TblFACEITEM.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_FACEITEM.STB"),
        true,
        true);
    g_TblUSEITEM.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_USEITEM.STB"),
        true,
        true);
    g_TblBACKITEM.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_BACK.STB"),
        true,
        true);
    g_TblGEMITEM.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEMITEM.STB"),
        true,
        true);
    g_TblJEWELITEM.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_JEWEL.STB"),
        true,
        true);
    g_TblQUESTITEM.Load(
        CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_QUESTITEM.STB"),
        true,
        true);

    if (!g_SkillList.LoadSkillTable(
            CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_SKILL.STB")))
        return false;

    if (!g_PatITEM.LoadPatITEM(
            CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_PAT.STB")))
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

    g_TblAVATAR.Load(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\INIT_AVATAR.STB"),
        false,
        false);
    g_TblWARP.Load2(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\WARP.STB"),
        true,
        false);

    g_TblZONE.Load2(CStr::Printf((char*)"%s%s", BASE_DATA_DIR, "3DDATA\\STB\\LIST_ZONE.STB"),
        true,
        false);
    g_ZoneLIST.Init(BASE_DATA_DIR);

    return true;
}
void
SHO_WS::Free_BasicDATA() {
    // STBDATA�� �ڵ� Ǯ��..
    g_SkillList.Free();
    g_PatITEM.Free();
}
void
SHO_WS::Send_ANNOUNCE(char* szMsg) {
    g_pListSERVER->Send_gsv_ANNOUNCE_CHAT(szMsg, NULL);
}

//-------------------------------------------------------------------------------------------------
void
SHO_WS::ToggleChannelActive(int iChannelNo) {
    g_pListSERVER->Set_ChannelACTIVE(iChannelNo, false, true);
}

void
SHO_WS::DeleteChannelServer(int iChannelNo) {
    g_pListSERVER->Del_ChannelSERVER(iChannelNo);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
VOID CALLBACK
WS_TimerProc(HWND hwnd /* handle to window */,
    UINT uMsg /* WM_TIMER message */,
    UINT_PTR idEvent /* timer identifier */,
    DWORD dwTime /* current system time */) {
    switch (idEvent) {
        case WS_TIMER_LSV: {
            SHO_WS* pGS = SHO_WS::GetInstance();
            if (pGS) {
                pGS->ConnectToLSV();
            }
            break;
        }

        case WS_TIMER_LOG: {
            SHO_WS* pGS = SHO_WS::GetInstance();
            if (pGS) {
                pGS->ConnectToLOG();
            }
            break;
        }

        case WS_TIMER_WORLD_TIME: {
            SHO_WS* pWS = SHO_WS::GetInstance();
            if (NULL == pWS)
                return;
            g_ZoneLIST.Inc_WorldTIME(true);

            switch (g_ZoneLIST.m_dwAccTIME % 6) {
                case 0:
                case 3: // 30�ʿ� �ѹ��� üũ...
                    g_pUserLIST->Check_SocketALIVE();
                    break;
                case 1: // case 4 :
                    g_pUserLIST->CloseIdleSCOKET(150 * 1000); // 2�� 30��
                    break;
            }
            break;
        }
    };
}

//-------------------------------------------------------------------------------------------------
