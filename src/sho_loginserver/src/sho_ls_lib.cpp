
#include "stdAFX.h"

#include "SHO_LS_LIB.h"

#include "CLS_Account.h"
#include "CLS_Client.h"
#include "CLS_Server.h"
#include "CLS_SqlTHREAD.h"
#include "blockLIST.h"
#include "csocketwnd.h"

#include "rose/common/config.h"

using namespace Rose::Common;

SHO_LS* SHO_LS::m_pInstance = NULL;

CLS_AccountLIST* g_pListJOIN = NULL;
CLS_AccountLIST* g_pListWAIT = NULL;

CLS_SqlTHREAD* g_pThreadSQL = NULL;

CLS_ListCLIENT* g_pListCLIENT;
CLS_ListSERVER* g_pListSERVER;


#define DEF_CLIENT_POOL_SIZE 8192 // µ¿½Ã Á¢¼Ó ´ë±â ¼ÒÄÏ
#define INC_CLIENT_POOL_SIZE 1024

#define DEF_SERVER_POOL_SIZE 256
#define INC_SERVER_POOL_SIZE 128

#define DEF_SEND_IO_POOL_SIZE (DEF_CLIENT_POOL_SIZE + INC_CLIENT_POOL_SIZE)
#define INC_SEND_IO_POOL_SIZE (INC_CLIENT_POOL_SIZE)

#define JOIN_HASH_TABLE_SIZE 4096
#define WAIT_HASH_TABLE_SIZE 512

#define LS_TIMER_CHECK_WAIT_LIST 1
#define LS_TICK_CHECK_WAIT_LIST 1000

static DWORD s_dwTick = 0;
VOID CALLBACK
LS_TimerProc(HWND hwnd /* handle to window */,
    UINT uMsg /* WM_TIMER message */,
    UINT_PTR idEvent /* timer identifier */,
    DWORD dwTime /* current system time */) {
    if (LS_TIMER_CHECK_WAIT_LIST == idEvent) {
        if (++s_dwTick % 2)
            g_pListWAIT->Delete_IdleACCOUNT(90); // ¼­¹ö ÀÌµ¿ÇÏ´Âµ¥ 30ÃÊ ÀÌ»ó °É¸®¸é Â©·¯~
        else
            g_pListCLIENT->Delete_IdleSOCKET();
    }
}

#include "../sho_ls.ver"
DWORD
GetServerBuildNO() {
    return BUILD_NUM;
}
DWORD g_dwStartTIME = 0;

DWORD
GetServerStartTIME() {
    return g_dwStartTIME;
}

SHO_LS::SHO_LS(): m_iClientListenPortNO(0), m_iServerListenPortNO(0) {
    m_pTIMER = NULL;
}

void
SHO_LS::SystemINIT(HINSTANCE hInstance) {
    ::Sleep(500);

    CPoolSENDIO::Instance(DEF_SEND_IO_POOL_SIZE, INC_SEND_IO_POOL_SIZE);

    g_pListCLIENT = new CLS_ListCLIENT(DEF_CLIENT_POOL_SIZE, INC_CLIENT_POOL_SIZE);
    g_pListSERVER = new CLS_ListSERVER(DEF_SERVER_POOL_SIZE, INC_SERVER_POOL_SIZE);

    g_pListJOIN = new CLS_AccountLIST((char*)"JOIN", JOIN_HASH_TABLE_SIZE);
    g_pListWAIT = new CLS_AccountLIST((char*)"WAIT", WAIT_HASH_TABLE_SIZE);

    m_iClientListenPortNO = 0;
    m_iServerListenPortNO = 0;

    CSocketWND* pSockWND = CSocketWND::InitInstance(hInstance, 1);
}

SHO_LS::~SHO_LS() {
    SAFE_DELETE(m_pTIMER);

    if (g_pThreadSQL) {
        g_pThreadSQL->Destroy();
        g_pThreadSQL = NULL;
    }

    SAFE_DELETE(g_pListJOIN);
    SAFE_DELETE(g_pListWAIT);

    SAFE_DELETE(g_pListCLIENT);
    SAFE_DELETE(g_pListSERVER);

    if (CSocketWND::GetInstance())
        CSocketWND::GetInstance()->Destroy();
    ;
    CPoolSENDIO::Destroy();
}

bool
SHO_LS::StartClientSOCKET(int iClientListenPort, int iLimitUserCNT, BYTE btMD5[32]) {
    if (m_iClientListenPortNO)
        return false;

    ::CopyMemory(m_btMD5, btMD5, 32);

    m_iClientListenPortNO = iClientListenPort;
    g_pListCLIENT->Active(m_iClientListenPortNO,
        65535,
        3 * 60);
    this->SetLimitUserCNT(iLimitUserCNT);

    return true;
}

void
SHO_LS::CloseClientSOCKET() {
    if (m_iClientListenPortNO) {
        g_pListCLIENT->Shutdown();
        m_iClientListenPortNO = 0;
    }
}

bool
SHO_LS::StartServerSOCKET(HWND hMainWND,
    char* szDBServerIP,
    int iServerListenPort,
    DWORD dwLoginRight,
    bool bShowOnlyWS) {

    if (m_iServerListenPortNO) {
        return false;
    }

    m_bShowOnlyWS = bShowOnlyWS;
    g_dwStartTIME = classTIME::GetCurrentAbsSecond();

    m_iServerListenPortNO = iServerListenPort;
    g_pListSERVER->Active(m_iServerListenPortNO,
        1024,
        60); // µ¿½Ã Á¢¼Ó ¼ÒÄÏ °¹¼ö 1024. Á¢¼ÓÀ¯Áö Ã¼Å© 1ºÐ

    m_pTIMER = new CTimer(hMainWND,
        LS_TIMER_CHECK_WAIT_LIST,
        LS_TICK_CHECK_WAIT_LIST,
        (TIMERPROC)LS_TimerProc);
    m_pTIMER->Start();

    return true;
}

void
SHO_LS::Shutdown() {
    CloseClientSOCKET();
    if (m_iServerListenPortNO) {
        SAFE_DELETE(m_pTIMER);

        g_pListSERVER->Shutdown();
        m_iServerListenPortNO = 0;
    }
}

//-------------------------------------------------------------------------------------------------
void
SHO_LS::Send_ANNOUNCE(void* pServer, char* szAnnounceMsg) {
    if (g_pListSERVER)
        g_pListSERVER->Send_lsv_ANNOUNCE_CHAT(pServer, szAnnounceMsg);
}

//-------------------------------------------------------------------------------------------------
void
SHO_LS::SetLoginRIGHT(DWORD dwLoginRight) {
    if (g_pThreadSQL)
        g_pThreadSQL->m_dwCheckRIGHT = dwLoginRight;
}

void
SHO_LS::SetLimitUserCNT(int iLimitUserCNT) {
    if (g_pListCLIENT)
        g_pListCLIENT->SetLimitUserCNT(iLimitUserCNT);
}

bool
SHO_LS::connect_database(const DatabaseConfig& db_config) {
    if (!g_pThreadSQL) {
        g_pThreadSQL = CLS_SqlTHREAD::Instance();

        char* ip = (char*)db_config.ip.c_str();
        char* username = (char*)db_config.username.c_str();
        char* password = (char*)db_config.password.c_str();
        char* name = (char*)db_config.name.c_str();

        if (!g_pThreadSQL->Connect(USE_ODBC,
                ip,
                username,
                password,
                name,
                32,
                1024 * 4)) {

            LOG_ERROR("Failed to connect to the databases: %s@%s(%s)", username, name, ip);

            g_pThreadSQL->Destroy();
            g_pThreadSQL = NULL;
            return false;
        }

        g_pThreadSQL->Resume();
        g_pThreadSQL->m_bCheckLogIN = true;
        g_pThreadSQL->m_dwCheckRIGHT = 0;
    }
    return true;
}
