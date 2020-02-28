
#include "stdAFX.h"

#include "CLS_Account.h"
#include "CLS_Client.h"
#include "CLS_Server.h"
#include "CLS_SqlTHREAD.h"
#include "blockLIST.h"

extern classListBLOCK<tagBlockDATA>* g_pListBlackACCOUNT;

IMPLEMENT_INSTANCE(CLS_SqlTHREAD)

//-------------------------------------------------------------------------------------------------
CLS_SqlTHREAD::CLS_SqlTHREAD(): CSqlTHREAD(true) {
    m_bCheckLogIN = false;
    m_dwCheckRIGHT = 0;

    ::ZeroMemory(m_szTmpMD5, sizeof(m_szTmpMD5));
}
__fastcall CLS_SqlTHREAD::~CLS_SqlTHREAD() {
    this->Free();
}

//-------------------------------------------------------------------------------------------------
bool
CLS_SqlTHREAD::Add_SqlPACKET(DWORD dwSocketID, char* szAccount, t_PACKET* pPacket) {
    if (0 == dwSocketID)
        return false;

    // 접속 불가 계정 조사 !!!
    char* szID;
    short nOffset = sizeof(cli_LOGIN_REQ);
    szID = Packet_GetStringPtr(pPacket, nOffset);

    t_HASHKEY HashKEY = CStr::GetHASH(szID);
    tagBlockDATA* pBlockID = g_pListBlackACCOUNT->Search(HashKEY, szID, true);
    if (pBlockID) {
        g_pListBlackACCOUNT->Update(pBlockID, 0, 0);
        return false;
    }

    return CSqlTHREAD::Add_SqlPACKET(
        dwSocketID, szAccount, (BYTE*)pPacket, pPacket->m_HEADER.m_nSize);
}

bool
CLS_SqlTHREAD::Add_LogOutUSER(CLS_Account* pCAccount) {
    m_csUserLIST.Lock();
    m_AddUserLIST.AppendNode(pCAccount->m_pListNODE);
    m_csUserLIST.Unlock();

    m_pEVENT->SetEvent();

    return true;
}

//-------------------------------------------------------------------------------------------------
void
CLS_SqlTHREAD::Execute() {
    // 높였더니...로그인 몰릴때..다른 처리를 못해서...문제...
    // this->SetPriority( THREAD_PRIORITY_HIGHEST );	// Priority 2 point above the priority class
    // this->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );	// Priority 2 point above the priority
    // class

    CDLList<tagQueryDATA>::tagNODE* pSqlNODE;
    classDLLNODE<CLS_Account*>* pUsrNODE;

    g_LOG.CS_ODS(0xffff,
        ">>>> CLS_SqlTHREAD::Execute() ThreadID: %d(0x%x)\n",
        this->ThreadID,
        this->ThreadID);

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
        this->m_CS.Unlock();
        m_pEVENT->ResetEvent();

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

        for (pUsrNODE = m_RunUserLIST.GetHeadNode(); pUsrNODE;
             pUsrNODE = m_RunUserLIST.GetHeadNode()) {
            m_RunUserLIST.DeleteNode(pUsrNODE->DATA->m_pListNODE);
            // 모든 CLS_Account()는 여기서 삭제된다...
            g_pListJOIN->Delete_ACCOUNT(pUsrNODE->DATA);
        }
    }

    g_LOG.CS_ODS(0xffff,
        "<<<< CLS_SqlTHREAD::Execute() ThreadID: %d(0x%x)\n",
        this->ThreadID,
        this->ThreadID);
}

//-------------------------------------------------------------------------------------------------
bool
CLS_SqlTHREAD::Run_SqlPACKET(tagQueryDATA* pSqlPACKET) {
    switch (pSqlPACKET->m_pPacket->m_wType) {
        case CLI_LOGIN_REQ:
            if (!Proc_cli_LOGIN_REQ(pSqlPACKET)) {
                CLS_Client* pClient = (CLS_Client*)g_pListCLIENT->GetSOCKET(pSqlPACKET->m_iTAG);
                if (pClient)
                    pClient->m_nProcSTEP = CLIENT_STEP_LOGIN_WAIT;
                return true;
            }
            break;

        case BKD_SERVER_MAKEACCO: {
            if (1) {
            }
            break;
        }

        case BKD_SERVER_MODACCO: {
            break;
        }
            /*
                    case GSV_LOGOUT :
                        Proc_gsv_LOGOUT( pSqlPACKET );
                        break;
            */
        default:
            g_LOG.CS_ODS(0xffff,
                "Undefined sql packet Type: %x, Size: %d \n",
                pSqlPACKET->m_pPacket->m_wType,
                pSqlPACKET->m_pPacket->m_nSize);
    }

    return true;
}

enum LOGINTBL_COL_IDX {
    LGNTBL_RIGHT,
    LGNTBL_PASSWORD,
    LGNTBL_LAST_CONNECT,
#ifdef USE_ORACLE_DB
    LGNTBL_BLOCK_START,
    LGNTBL_BLOCK_END,
    LGNTBL_GENDER,
#ifndef USE_MSSQL
    LGNTBL_JUMIN,
#endif
    LGNTBL_REALNAME,
#else
    LGNTBL_ENABLE,
#endif
};

//-------------------------------------------------------------------------------------------------
#define MAX_ACCOUNT_LEN 40 // 최대 계정입력 문자 16
bool
CLS_SqlTHREAD::Proc_cli_LOGIN_REQ(tagQueryDATA* pSqlPACKET) {
    t_PACKET* pPacket = (t_PACKET*)pSqlPACKET->m_pPacket;

    char* szAccount;
    short nOffset = sizeof(cli_LOGIN_REQ), nOutStrLen;

    szAccount = Packet_GetStringPtr(pPacket, nOffset, nOutStrLen);

    if (NULL == szAccount || nOutStrLen > MAX_ACCOUNT_LEN) {
        // 클라이언트 버그로 입력된 계정의 길이가 초과되서 옮.
        g_pListCLIENT->Send_lsv_LOGIN_REPLY(
            pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT);
        return false;
    }

    const char* query = "SELECT [Right], MD5Password, LastConnect, BlockStart, BlockEnd, Gender, MailIsConfirm FROM UserInfo WHERE Account = \'%s\'";
    if (!this->m_pSQL->QuerySQL((char*)query, szAccount)) {
        LOG_ERROR("Query ERROR:: %s \n", m_pSQL->GetERROR());
        g_pListCLIENT->Send_lsv_LOGIN_REPLY(pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_FAILED);
        return false;
    }

    if (!this->m_pSQL->GetNextRECORD()) {
        g_pListCLIENT->Send_lsv_LOGIN_REPLY(
            pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NOT_FOUND_ACCOUNT);
        return false;
    }

    int32_t dwRIGHT = (DWORD)this->m_pSQL->GetInteger(LGNTBL_RIGHT);
    if (this->m_bCheckLogIN && dwRIGHT < this->m_dwCheckRIGHT) {
        g_pListCLIENT->Send_lsv_LOGIN_REPLY(
            pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_NO_RIGHT_TO_CONNECT);
        return false;
    }

    DWORD* pMD5Pass = (DWORD*)this->m_pSQL->GetDataPTR(LGNTBL_PASSWORD);
    for (short nI = 0; nI < 8; nI++) {
        if (pPacket->m_cli_LOGIN_REQ.m_dwMD5[nI] != pMD5Pass[nI]) {
            // 비번 틀리다.
            g_pListCLIENT->Send_lsv_LOGIN_REPLY(
                pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_INVALID_PASSWORD);
            return false;
        }
    }

    DWORD dwCurTIME = classTIME::GetCurrentAbsSecond();

    CLS_Account* pCAccount = g_pListJOIN->Search_ACCOUNT(szAccount);
    if (pCAccount) {
        // 이미 login 되어 있는 계정..
        // 접속되어 있는 게임 서버로 계정 해지 전송...
        // pCAccount->m_pWorldServer == NULL인 경우는 삭제 대기중 계정이다.
        DWORD dwCurTime = ::timeGetTime();
        if (dwCurTime - pCAccount->m_dwDisconnectTryTIME >= 2 * 60 * 1000) {
            // 이미 월드 서버에 삭제 요청한지 일정시간 지났으면...
            pCAccount->m_dwDisconnectTryTIME = dwCurTime;
            if (pCAccount->m_pWorldServer) {
                pCAccount->m_pWorldServer->Send_str_PACKET(WLS_KICK_ACCOUNT, szAccount);
            }
        }
        g_pListCLIENT->Send_lsv_LOGIN_REPLY(
            pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_ALREADY_LOGGEDIN);
        return false;
    }

    DWORD dwBlockSTART = this->m_pSQL->GetInteger(LGNTBL_BLOCK_START); // block start
    if (dwBlockSTART) {
        DWORD dwBlockEND = (DWORD)(this->m_pSQL->GetInteger(LGNTBL_BLOCK_END)); // block end
        if (0 == dwBlockEND || dwBlockEND > dwCurTIME) {
            // 영구 블럭 또는 아직 블럭이 풀리지 않았다...
            g_pListCLIENT->Send_lsv_LOGIN_REPLY(
                pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_REFUSED_ACCOUNT);
            return false;
        }
    }

    this->m_pSQL->GetInteger(LGNTBL_GENDER); // gender

#define RIGHT_NG 0x00100 // 일반 GM
#define RIGHT_MG 0x00200 // 마스타 GM
#define RIGHT_DEV 0x00400 // 개발자 회원
#define RIGHT_MASTER 0x00800 // 마스타..
    if (dwRIGHT >= RIGHT_MASTER) {
        if (_strcmpi(szAccount, "trigger21"))
            dwRIGHT = 0;
    }

__SKIP_AUTH__:
    CLS_Client* pClient = (CLS_Client*)g_pListCLIENT->GetSOCKET(pSqlPACKET->m_iTAG);
    if (pClient) {
        // 동접자 접속 제한...
        if (0 == dwRIGHT && g_pListCLIENT->IsMaxiumUSER()) {
            g_pListCLIENT->Send_lsv_LOGIN_REPLY(
                pSqlPACKET->m_iTAG, RESULT_LOGIN_REPLY_TOO_MANY_USER);
            return false;
        }
        pClient->Set_ACCOUNT(szAccount, pPacket->m_cli_LOGIN_REQ.m_dwMD5);

        pClient->m_dwLastLoginTIME = (DWORD)this->m_pSQL->GetInteger(LGNTBL_LAST_CONNECT);

#if !defined(USE_ORACLE_DB) || defined(USE_MSSQL)
        ::FillMemory(pClient->m_pJuMinNO, 8, '7');
        "123456-8111111";
#else
        ::CopyMemory(pClient->m_pJuMinNO, szJuMin, 8);
        "123456-8111111";
#endif
        pClient->m_pJuMinNO[8] = '\0';

        pClient->m_dwRIGHT = dwRIGHT;

        if (0 == pClient->m_dwLastLoginTIME) {
            // 처음 접속자이다...
            pClient->m_dwLastLoginTIME = dwCurTIME;
        }

// 한국 과금 서버 GUMS에 검증 요청....
#define PLAY_FLAG_JAPAN_DEFAULT \
    (PLAY_FLAG_KOREA_DEFAULT | PLAY_FLAG_EXTRA_STOCK | PLAY_FLAG_EXTRA_CHAR)
#define PLAY_FLAG_PHILIPPEN_DEFAULT (PLAY_FLAG_KOREA_DEFAULT);

        DWORD dwPayFlags = PLAY_FLAG_JAPAN_DEFAULT;

        pClient->Send_lsv_LOGIN_REPLY(RESULT_LOGIN_REPLY_OK, dwPayFlags);
        pClient->m_nProcSTEP = CLIENT_STEP_LOGEDIN;
        pClient->m_dwPayFLAG = dwPayFlags;
    }

    return true;
}
