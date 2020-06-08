#include "stdAFX.h"
#if defined(__SHO_WS)

    #ifdef __SHO_WS
        #include "CWS_Client.h"
    #endif

    #include "CThreadMSGR.h"
    //#include "classLOG.h"
    #include "IO_STB.h"

//-------------------------------------------------------------------------------------------------
CThreadMSGR::CThreadMSGR(UINT uiInitDataCNT, UINT uiIncDataCNT):
    CSqlTHREAD(true), /* m_csListCMD( 4000 ) */
    m_Pools((char*)"CMessengerPOOL", uiInitDataCNT, uiIncDataCNT), m_HashMSGR(1024 * 3) {
    m_pListBUFF = new BYTE[2048];
}
CThreadMSGR::~CThreadMSGR() {
    SAFE_DELETE_ARRAY(m_pListBUFF);
}

void
CThreadMSGR::Check_FRIENDS() {
    /*
        int iCharID=0, iFriendCNT, iTotCNT=0;
        BYTE *pLIST;
        while( true )
        {
            if ( !this->db->QuerySQL( "SELECT TOP 1 * FROM tblWS_FRIEND WHERE intCharID > %d
       ORDER BY intCharID ASC", iCharID ) ) { g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n",
       this->db->GetERROR() ); return;
            }

            if ( !this->db->GetNextRECORD() )
                break;

            {
                iCharID = this->db->GetInteger( 0 );
                iFriendCNT = this->db->GetInteger( 1 );
                pLIST = this->db->GetDataPTR( 2 );

                char *pName;
                tagFriend_H *pFrH;
                tagMyFriend  FR;
                for (int iC=0; iC<iFriendCNT; iC++) {
                    pFrH = (tagFriend_H*)pLIST;
                    pName = (char*)( pLIST+sizeof(tagFriend_H) );

                    _ASSERT( iCharID != pFrH->m_dwDBID );

                    FR.m_dwDBID = pFrH->m_dwDBID;
                    FR.m_btSTATUS = pFrH->m_btSTATUS;
                    FR.m_Name.Set( pName );












                    // check name & dbid...
                    if ( !this->db->QuerySQL( "SELECT txtNAME FROM tblGS_AVATAR WHERE
       intCharID=%d", FR.m_dwDBID ) ) { g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n",
       this->db->GetERROR() ); } else if ( this->db->GetNextRECORD() ) { pName =
       this->db->GetStrPTR( 0 ); if ( 0 == pFrH->m_dwDBID || pFrH->m_dwDBID > 31793 ) {
                            LogString (0xffff, "Invalid DBID: %d, %s \n", FR.m_dwDBID,
       FR.m_Name.Get() ); } else if ( strcmpi(pName, FR.m_Name.Get() ) ) { LogString (0xffff, "Name
       mismatch %d, %s/%s \n", pFrH->m_dwDBID, pName, FR.m_Name.Get() );
                        }
                    }

                    pLIST += ( sizeof(tagFriend_H)+strlen(pName)+1 );
                }

                iTotCNT ++;
            }
        }
        LogString (0xffff, ">>>>>>>>>>>>> Totcal friend record %d \n", iTotCNT );
    */
}

//-------------------------------------------------------------------------------------------------
struct tagRU {
    CStrVAR m_Char;
    int m_iCnt;
};

void
CThreadMSGR::Check_ItemCNT(int iItemType, int iItemNo) {}

//-------------------------------------------------------------------------------------------------
void
CThreadMSGR::Execute() {
    CDLList<tagMSGR_CMD>::tagNODE* pCmdNODE;

    //	this->Check_FRIENDS ();
    //	this->Check_ItemCNT (ITEM_TYPE_NATURAL, 401);

    while (TRUE) {
        if (!this->Terminated) {
            m_pEVENT->WaitFor(INFINITE);
        } else {
            int iReaminCNT;
            this->m_CS.Lock();
            iReaminCNT = m_WaitCMD.GetNodeCount();
            this->m_CS.Unlock();

            if (iReaminCNT <= 0)
                break;
        }

        this->m_CS.Lock();
        m_ProcCMD.AppendNodeList(&m_WaitCMD);
        m_WaitCMD.Init();
        m_pEVENT->ResetEvent();
        this->m_CS.Unlock();

        for (pCmdNODE = m_ProcCMD.GetHeadNode(); pCmdNODE;) {
            this->Run_MessengerPACKET(&pCmdNODE->m_VALUE);

            SAFE_DELETE_ARRAY(pCmdNODE->m_VALUE.m_pPacket);
            m_ProcCMD.DeleteNFree(pCmdNODE);
            pCmdNODE = m_ProcCMD.GetHeadNode();
        }
    }

    int iCnt = m_AddPACKET.GetNodeCount();
    _ASSERT(iCnt == 0);
}

//-------------------------------------------------------------------------------------------------
void
CThreadMSGR::Add_MessengerCMD(char* szCharName,
    BYTE btCMD,
    t_PACKET* pPacket,
    int iSocketIDX,
    DWORD dwDBID) {
    CDLList<tagMSGR_CMD>::tagNODE* pNewNODE;

    pNewNODE = new CDLList<tagMSGR_CMD>::tagNODE;

    pNewNODE->m_VALUE.m_iSocketIDX = iSocketIDX;
    pNewNODE->m_VALUE.m_dwDBID = dwDBID;
    pNewNODE->m_VALUE.m_btCMD = btCMD;
    pNewNODE->m_VALUE.m_Name.Set(szCharName);
    if (pPacket) {
        pNewNODE->m_VALUE.m_pPacket = (t_PACKET*)new BYTE[pPacket->m_HEADER.m_nSize];
        ::CopyMemory(pNewNODE->m_VALUE.m_pPacket, pPacket, pPacket->m_HEADER.m_nSize);
    } else
        pNewNODE->m_VALUE.m_pPacket = NULL;

    this->m_CS.Lock();
    m_WaitCMD.AppendNode(pNewNODE);
    this->m_CS.Unlock();

    this->Set_EVENT();
}

//-------------------------------------------------------------------------------------------------
CMessenger*
CThreadMSGR::SearchMSGR(char* szCharName) {
    t_HASHKEY HashKEY = CStr::GetHASH(szCharName);

    tagHASH<CMessenger*>* pHashNode = m_HashMSGR.Search(HashKEY);
    while (pHashNode) {
        if (!_strcmpi(szCharName, pHashNode->m_DATA->m_Name.Get())) {
            return pHashNode->m_DATA;
        }
        pHashNode = m_HashMSGR.SearchContinue(pHashNode, HashKEY);
    }
    return NULL;
}

bool
CThreadMSGR::Run_MessengerPACKET(tagMSGR_CMD* pMsgCMD) {
    switch (pMsgCMD->m_btCMD) {
        case MSGR_CMD_LOGIN:
            this->LogIN(pMsgCMD);
            return true;
        case MSGR_CMD_LOGOUT: {
            CMessenger* pMSGR = this->SearchMSGR(pMsgCMD->m_Name.Get());
            if (pMSGR)
                this->LogOUT(pMSGR);
            return true;
        }

        case MSGR_CMD_APPEND_ACCEPT: {
            CWS_Client* pRequestUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(
                pMsgCMD->m_pPacket->m_cli_MCMD_APPEND_REPLY.m_wUserIDX);
            short nOffset = sizeof(cli_MCMD_APPEND_REPLY);
            char* szName = Packet_GetStringPtr(pMsgCMD->m_pPacket, nOffset);
            if (pRequestUSER && szName && !_strcmpi(szName, pRequestUSER->Get_NAME())) {
                CMessenger* pMSGR1 = this->SearchMSGR(pMsgCMD->m_Name.Get());
                if (pMSGR1) {
                    if (pMSGR1->Get_FriendCNT() < MAX_FRIEND_COUNT) {
                        CMessenger* pMSGR2 = this->SearchMSGR(pRequestUSER->Get_NAME());
                        if (pMSGR2 && pMSGR1->m_dwDBID != pMSGR2->m_dwDBID) {
                            if (pMSGR2->Get_FriendCNT() < MAX_FRIEND_COUNT) {
                                pMSGR1->MSGR_Add(pMSGR2);
                                pMSGR2->MSGR_Add(pMSGR1);
                                return true;
                            }
                            // pMSGR2���� ģ�� �߰� ���� ����
                        }
                    }
                    // pMSGR1���� ģ�� �߰� ���� ����
                }
            }
            return true;
        }

        case MSGR_CMD_DELETE: {
            CMessenger* pMSGR = this->SearchMSGR(pMsgCMD->m_Name.Get());
            if (pMSGR)
                pMSGR->MSGR_Del(pMsgCMD->m_pPacket->m_cli_MCMD_TAG.m_dwUserTAG);
            return true;
        }

        case MSGR_CMD_REFUSE: {
            CMessenger* pMSGR = this->SearchMSGR(pMsgCMD->m_Name.Get());
            if (pMSGR)
                pMSGR->MSGR_Ban(pMsgCMD->m_pPacket->m_cli_MCMD_TAG.m_dwUserTAG);
            return true;
        }

        case MSGR_CMD_CHANGE_STATUS: {
            // ������ ����... ��ϵ� ��� ģ������...
            CMessenger* pMSGR = this->SearchMSGR(pMsgCMD->m_Name.Get());
            if (pMSGR)
                pMSGR->MSGR_Status2ALL(pMsgCMD->m_pPacket->m_cli_MCMD_STATUS_REQ.m_btStatus);
            break;
        }

        case 0x0ff: {
            CMessenger* pMSGR = this->SearchMSGR(pMsgCMD->m_Name.Get());
            if (pMSGR && FRIEND_STATUS_REFUSE_MSG != pMSGR->m_btMsgrSTATUS)
                pMSGR->MSGR_Msg(pMsgCMD->m_pPacket);
            return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
bool
CThreadMSGR::LogIN(tagMSGR_CMD* pCMD) {
    if (!this->db->QuerySQL((char*)"{call ws_GetFRIEND(%d)}", pCMD->m_dwDBID)) {
        //	if ( !this->db->QuerySQL( "SELECT intFriendCNT, blobFRIENDS FROM tblWS_FRIEND WHERE
        // intCharID=%d", pCMD->m_dwDBID ) ) {
        g_LOG.CS_ODS(LOG_NORMAL, "Query ERROR:: %s \n", this->db->GetERROR());
        return false;
    }

    CMessenger* pMSGR;
    pMSGR = this->SearchMSGR(pCMD->m_Name.Get());
    if (pMSGR)
        return true;

    pMSGR = this->AllocMEM();
    if (NULL == pMSGR)
        return false;

    pMSGR->Init(pCMD->m_Name.Get(), pCMD->m_dwDBID, pCMD->m_iSocketIDX);
    if (!this->db->GetNextRECORD()) {
        // insert !!!
        if (this->db->ExecSQL((char*)"INSERT tblWS_FRIEND ( intCharID ) VALUES(%d);",
                pCMD->m_dwDBID)
            < 1) {
            g_LOG.CS_ODS(LOG_NORMAL,
                "SQL Exec ERROR:: INSERT %s friend : %s \n",
                pCMD->m_Name.Get(),
                this->db->GetERROR());
            this->FreeMEM(pMSGR);
            return true;
        }
    } else {
        int iFriendCNT = this->db->GetInteger(0);
        if (iFriendCNT > 0) {
            BYTE* pDATA = this->db->GetDataPTR(1);
            pMSGR->MSGR_LogIN(iFriendCNT, pDATA);
        }
    }
    pMSGR->m_btMsgrSTATUS = FRIEND_STATUS_ONLINE;

    t_HASHKEY HashKEY = CStr::GetHASH(pCMD->m_Name.Get());
    this->m_HashMSGR.Insert(HashKEY, pMSGR);

    return true;
}

//-------------------------------------------------------------------------------------------------
void
CThreadMSGR::LogOUT(CMessenger* pMSGR) {
    // update db...
    int iFriendCNT = pMSGR->Get_FriendCNT();
    int iBuffLEN = pMSGR->MSGR_LogOUT(this->m_pListBUFF);

    if (pMSGR->MSGR_IsUPDATE()) {
        if (iFriendCNT > 0) {
            this->db->BindPARAM(1, this->m_pListBUFF, iBuffLEN);
            this->db->MakeQuery((char*)"UPDATE tblWS_FRIEND SET blobFRIENDS=",
                MQ_PARAM_BINDIDX,
                1,
                MQ_PARAM_ADDSTR,
                ",intFriendCNT=",
                MQ_PARAM_INT,
                iFriendCNT,
                MQ_PARAM_ADDSTR,
                "WHERE intCharID=",
                MQ_PARAM_INT,
                pMSGR->Get_DBID(),
                MQ_PARAM_END);
        } else {
            this->db->MakeQuery((char*)"UPDATE tblWS_FRIEND SET blobFRIENDS=NULL",
                MQ_PARAM_ADDSTR,
                ",intFriendCNT=",
                MQ_PARAM_INT,
                iFriendCNT,
                MQ_PARAM_ADDSTR,
                "WHERE intCharID=",
                MQ_PARAM_INT,
                pMSGR->Get_DBID(),
                MQ_PARAM_END);
        }
        if (this->db->ExecSQLBuffer() < 0) {
            // ��ġ�� ���� !!!
            g_LOG.CS_ODS(LOG_NORMAL,
                "SQL Exec ERROR:: UPDATE messenger:%d %s \n",
                pMSGR->Get_DBID(),
                this->db->GetERROR());
        }
    }

    #ifdef _DEBUG
    if (pMSGR->Get_FriendCNT()) {
        g_LOG.CS_ODS(0xffff,
            "ERROR:: pMSGR->m_ListFRIEND.GetNodeCount() must 0 / %d ",
            pMSGR->Get_FriendCNT(),
            pMSGR->m_dwDBID);
    }
    #endif

    t_HASHKEY HashKEY = CStr::GetHASH(pMSGR->m_Name.Get());
    this->m_HashMSGR.Delete(HashKEY, pMSGR);

    this->FreeMEM(pMSGR);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool
CMessenger::SendPacket(int iClientSocketIDX, DWORD dwDBID, classPACKET* packet) {
    // Temporary until all packets are refactored
    classPACKET pCPacket = *packet;

    CWS_Client* pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(iClientSocketIDX);
    if (pFindUSER && pFindUSER->m_dwDBID == dwDBID) {
        if (!pFindUSER->SendPacket(pCPacket)) {
            // ������ ����...
        }
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
void
CMessenger::MSGR_LogIN(int iCount, BYTE* pLIST) {
    classPACKET* pToFR = Packet_AllocNLock();
    classPACKET* pToME = Packet_AllocNLock();

    pToFR->m_HEADER.m_wType = WSV_MESSENGER;
    pToFR->m_HEADER.m_nSize = sizeof(wsv_MCMD_STATUS_REPLY);

    pToFR->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
    pToFR->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = this->Get_DBID();
    pToFR->m_wsv_MCMD_STATUS_REPLY.m_btStatus = FRIEND_STATUS_ONLINE;

    pToME->m_HEADER.m_wType = WSV_MESSENGER;
    pToME->m_HEADER.m_nSize = sizeof(wsv_MCMD_LIST);

    pToME->m_wsv_MCMD_LIST.m_btCMD = MSGR_CMD_LIST;
    pToME->m_wsv_MCMD_LIST.m_btFriendCNT = iCount;

    tagFriend_H sFrStatus;

    char* pName;
    tagFriend_H* pFrH;

    CMessenger* pFindMSGR;
    CDLList<tagMyFriend>::tagNODE* pNODE;
    for (int iC = 0; iC < iCount; iC++) {
        pFrH = (tagFriend_H*)pLIST;
        pName = (char*)(pLIST + sizeof(tagFriend_H));

        if (this->m_dwDBID == pFrH->m_dwDBID) {
            // �ڱ� �ڽ��� ��ϵ� ������ �ִ°� ?
            CStrVAR tmpName;
            tmpName.Set(pName);
            pLIST += (sizeof(tagFriend_H) + tmpName.BuffLength() + 1);
            this->m_bFriendUPDATE = true;
            continue;
        }

        pNODE = m_ListFRIEND.AllocNAppend();

        pNODE->m_VALUE.m_dwDBID = pFrH->m_dwDBID;
        pNODE->m_VALUE.m_btSTATUS = pFrH->m_btSTATUS;
        pNODE->m_VALUE.m_Name.Set(pName);
        pNODE->m_VALUE.m_pMSGR = NULL;

        pLIST += (sizeof(tagFriend_H) + pNODE->m_VALUE.m_Name.BuffLength() + 1);

        if (!(FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS)) {
            pFindMSGR = g_pThreadMSGR->SearchMSGR(pName);
            if (pFindMSGR) {
                pNODE->m_VALUE.m_btSTATUS =
                    pFindMSGR->MSGR_OnOffLine(pToFR, this, this->Get_DBID(), FRIEND_STATUS_ONLINE);
                if (FRIEND_STATUS_DELETED != pNODE->m_VALUE.m_btSTATUS) {
                    // pFindMSGR��Ͽ� ���� �ִ�...
                    pNODE->m_VALUE.m_pMSGR = pFindMSGR;
                }
            } else {
                pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_OFFLINE;
            }
        } /* else {
            // ���� �����ϰų� ���� �����ѳ��̱� ���� �� ���� �뺸�� �ʿ����.
            // ���� �̳��� �α׾ƿ� ���·� ���̰���...
        } */

        sFrStatus.m_dwDBID = pNODE->m_VALUE.m_dwDBID;
        sFrStatus.m_btSTATUS = pNODE->m_VALUE.m_btSTATUS;

        pToME->AppendData(&sFrStatus, sizeof(tagFriend_H));
        pToME->AppendString(pName);
        if (pToME->m_HEADER.m_nSize > MAX_PACKET_SIZE - 30) {
            this->m_bFriendUPDATE = true;
            break;
        }
    }

    this->SendPacket(this->m_iOwnerSocetIDX, this->m_dwDBID, pToME);

    Packet_ReleaseNUnlock(pToME);
    Packet_ReleaseNUnlock(pToFR);
}

//-------------------------------------------------------------------------------------------------
/*
2. �α׾ƿ���
    . ���� ����� ģ������Ʈ ��ȸ..
        NODE->m_VALUE.m_pUSER != NULL �̸� �α׾ƿ� ����.
*/
int
CMessenger::MSGR_LogOUT(BYTE* pOutBUFF) {
    classPACKET* pCPacket = Packet_AllocNLock();

    pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
    pCPacket->m_HEADER.m_nSize = sizeof(wsv_MCMD_STATUS_REPLY);

    pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
    pCPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = this->Get_DBID();
    pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus = FRIEND_STATUS_OFFLINE;

    int iBuffLEN = 0;
    tagFriend_H* pFR;

    CMessenger* pFindMSGR;
    CDLList<tagMyFriend>::tagNODE* pNODE;
    pNODE = m_ListFRIEND.GetHeadNode();
    while (pNODE) {
        pFR = (tagFriend_H*)&pOutBUFF[iBuffLEN];

        pFR->m_btSTATUS = pNODE->m_VALUE.m_btSTATUS;
        pFR->m_dwDBID = pNODE->m_VALUE.m_dwDBID;

        iBuffLEN += sizeof(tagFriend_H);
        ::CopyMemory(&pOutBUFF[iBuffLEN],
            pNODE->m_VALUE.m_Name.Get(),
            pNODE->m_VALUE.m_Name.BuffLength());
        iBuffLEN += pNODE->m_VALUE.m_Name.BuffLength();
        pOutBUFF[iBuffLEN++] = 0;

        pFindMSGR = g_pThreadMSGR->SearchMSGR(pNODE->m_VALUE.m_Name.Get());
        if (pFindMSGR) {
            if (pFR->m_btSTATUS == FRIEND_STATUS_OFFLINE) {
                // ������ �ִµ�, ������ ���¸� ��Ͼ��ߴٴ°��� ���� �����߰ų�, ����
                // �����ߴٴ°�...
                pFR->m_btSTATUS = FRIEND_STATUS_DELETED;
            }
            pFindMSGR->MSGR_OnOffLine(pCPacket, NULL, this->Get_DBID(), FRIEND_STATUS_OFFLINE);
        } else {
            if (pNODE->m_VALUE.m_pMSGR) {
                // ���� ???
                g_LOG.CS_ODS(0xffff,
                    "**ERROR in MSGR_LogOUT owner[ 0x%x:%d:%s ], frined[ 0x%x, %d:%s ]\n",
                    this->m_btMsgrSTATUS,
                    this->m_dwDBID,
                    this->m_Name.Get(),
                    pNODE->m_VALUE.m_btSTATUS,
                    pNODE->m_VALUE.m_dwDBID,
                    pNODE->m_VALUE.m_Name.Get());
            }
        }
        /*
                if ( pNODE->m_VALUE.m_pMSGR ) {
                    if ( pFindMSGR == pNODE->m_VALUE.m_pMSGR ) {
                        pNODE->m_VALUE.m_pMSGR->MSGR_OnOffLine( pCPacket, NULL, this->Get_DBID(),
           FRIEND_STATUS_OFFLINE ); } else {
                        // ����~~~ :: � ���� ???
                        g_LOG.CS_ODS( 0xffff, "**ERROR in MSGR_LogOUT owner[ 0x%x:%d:%s ], frined[
           0x%x, %d:%s ]\n", this->m_btMsgrSTATUS, this->m_dwDBID, this->m_Name.Get(),
           pNODE->m_VALUE.m_btSTATUS, pNODE->m_VALUE.m_dwDBID, pNODE->m_VALUE.m_Name.Get() );
                    }
                } else
                if ( pFindMSGR && pFR->m_btSTATUS == FRIEND_STATUS_OFFLINE ) {
                    // ������ �ִµ�, ������ ���¸� ��Ͼ��ߴٴ°��� ���� �����߰ų�, ����
           �����ߴٴ°�... pFR->m_btSTATUS = FRIEND_STATUS_DELETED;
                }
        */
        m_ListFRIEND.DeleteNFree(pNODE);
        pNODE = m_ListFRIEND.GetHeadNode();
    }

    Packet_ReleaseNUnlock(pCPacket);

    return iBuffLEN;
}

bool
CMessenger::MSGR_Add(CMessenger* pFriend) {
    CDLList<tagMyFriend>::tagNODE* pNODE;

    pNODE = m_ListFRIEND.GetHeadNode();
    while (pNODE) {
        if (pNODE->m_VALUE.m_dwDBID == pFriend->Get_DBID()) {
            // �̹� ��ϵ� ����ڴ�... �����Ǿ������� �ٽ� �߰��� ���..
            pNODE->m_VALUE.m_btSTATUS = pFriend->m_btMsgrSTATUS;
            pNODE->m_VALUE.m_pMSGR = pFriend;
            this->MSGR_Status2ONE(pFriend->Get_DBID(),
                pFriend->m_btMsgrSTATUS,
                pFriend); // CMessenger::MSGR_Add
            return true;
        }
        pNODE = pNODE->GetNext();
    }

    CWS_Client* pFindUSER = (CWS_Client*)g_pUserLIST->GetSOCKET(pFriend->m_iOwnerSocetIDX);
    if (pFindUSER && pFindUSER->m_dwDBID == pFriend->m_dwDBID) {
        this->m_bFriendUPDATE = true;
        pNODE = m_ListFRIEND.AllocNAppend();

        pNODE->m_VALUE.m_btSTATUS = pFriend->m_btMsgrSTATUS;
        pNODE->m_VALUE.m_dwDBID = pFriend->Get_DBID();
        pNODE->m_VALUE.m_pMSGR = pFriend;
        pNODE->m_VALUE.m_Name.Set(pFriend->m_Name.Get());

        classPACKET* pCPacket = Packet_AllocNLock();

        pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
        pCPacket->m_HEADER.m_nSize = sizeof(wsv_MCMD_APPEND_ACCEPT);

        pCPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btCMD = MSGR_CMD_APPEND_ACCEPT;
        pCPacket->m_wsv_MCMD_APPEND_ACCEPT.m_dwUserTAG = pNODE->m_VALUE.m_dwDBID;
        pCPacket->m_wsv_MCMD_APPEND_ACCEPT.m_btStatus = pNODE->m_VALUE.m_btSTATUS;
        pCPacket->AppendString(pNODE->m_VALUE.m_Name.Get());

        this->SendPacket(this->m_iOwnerSocetIDX, this->m_dwDBID, pCPacket);

        Packet_ReleaseNUnlock(pCPacket);
        return true;
    }
    return false;
}

void
CMessenger::MSGR_Del(DWORD dwDBID) {
    CDLList<tagMyFriend>::tagNODE* pNODE;

    pNODE = m_ListFRIEND.GetHeadNode();
    while (pNODE) {
        if (dwDBID == pNODE->m_VALUE.m_dwDBID) {
            // ����Ŭ���̾�Ʈ �˾Ƽ� �����϶��...
            if (pNODE->m_VALUE.m_pMSGR) {
                // "����� �����Ȼ��·� �����ϰ� �ݵ�� m_pFRIEND = NULL�� ������...... " )
                pNODE->m_VALUE.m_pMSGR->MSGR_Status2ONE(this->Get_DBID(),
                    FRIEND_STATUS_DELETED,
                    NULL); // CMessenger::MSGR_Del
            }

            m_ListFRIEND.DeleteNFree(pNODE);
            this->m_bFriendUPDATE = true;
            break;
        }
        pNODE = pNODE->GetNext();
    }
}

//-------------------------------------------------------------------------------------------------
BYTE
CMessenger::MSGR_OnOffLine(classPACKET* pCPacket,
    CMessenger* pFriend,
    DWORD dwDBID,
    BYTE btNewStatus) {
    CDLList<tagMyFriend>::tagNODE* pNODE;

    pNODE = m_ListFRIEND.GetHeadNode();
    while (pNODE) {
        if (dwDBID == pNODE->m_VALUE.m_dwDBID) {
            pNODE->m_VALUE.m_pMSGR = pFriend;

            if (!(FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS)) {
                // ���� �ź��߰ų�, ���� ������ ���̾ƴϸ�...
                pNODE->m_VALUE.m_btSTATUS = btNewStatus;
                this->SendPacket(this->m_iOwnerSocetIDX, this->m_dwDBID, pCPacket);
                return this->m_btMsgrSTATUS;
            }

            return FRIEND_STATUS_OFFLINE;
        }
        pNODE = pNODE->GetNext();
    }

    return FRIEND_STATUS_DELETED;
}

//-------------------------------------------------------------------------------------------------
void
CMessenger::MSGR_Status2ONE(DWORD dwDBID, BYTE btNewStatus, CMessenger* pMessenger) {
    // ����Ͽ��� dwDBID�� ã�� btNewStatus���·� �ٲ۴�.
    // dwDBID�� ���� �����Ѱ��̶��... �� ����� m_pFRIEND�� NULL !!!�� �����ؾ���.
    CDLList<tagMyFriend>::tagNODE* pNODE;

    pNODE = m_ListFRIEND.GetHeadNode();
    while (pNODE) {
        if (dwDBID == pNODE->m_VALUE.m_dwDBID) { // 04. 10. 11 : 0x0013bfe����...
            pNODE->m_VALUE.m_pMSGR = pMessenger; // ���� �����Ҷ��� pMessenger = NULL�� ��

            if (!(FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS)) {
                // ���� �ź��� ���°ų�, ���� ���� ������ ���°� �ƴϸ�??? (���� ������ ���� ������
                // ����-������ ���)
                this->m_bFriendUPDATE =
                    (FRIEND_STATUS_REFUSED & (pNODE->m_VALUE.m_btSTATUS | btNewStatus)) ? true
                                                                                        : false;
                pNODE->m_VALUE.m_btSTATUS = btNewStatus;

                classPACKET* pCPacket = Packet_AllocNLock();

                pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
                pCPacket->m_HEADER.m_nSize = sizeof(wsv_MCMD_STATUS_REPLY);

                pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
                pCPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = dwDBID;
                pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus = btNewStatus;

                this->SendPacket(this->m_iOwnerSocetIDX, this->m_dwDBID, pCPacket);

                Packet_ReleaseNUnlock(pCPacket);
            }
            break;
        }
        pNODE = pNODE->GetNext();
    }
}

void
CMessenger::MSGR_Msg(t_PACKET* pPacket) {
    CDLList<tagMyFriend>::tagNODE* pNODE;

    pNODE = m_ListFRIEND.GetHeadNode();
    while (pNODE) {
        if (pNODE->m_VALUE.m_dwDBID == pPacket->m_cli_MESSENGER_CHAT.m_dwUserTAG) {
            if (pNODE->m_VALUE.m_pMSGR) {
                classPACKET* pCPacket = Packet_AllocNLock();
                if (pCPacket) {
                    pCPacket->m_HEADER.m_wType = WSV_MESSENGER_CHAT;
                    pCPacket->m_HEADER.m_nSize = sizeof(wsv_MESSENGER_CHAT);
                    pCPacket->m_wsv_MESSENGER_CHAT.m_dwUserTAG = this->m_dwDBID;
                    pCPacket->AppendString(pPacket->m_cli_MESSENGER_CHAT.m_szMSG);

                    this->SendPacket(pNODE->m_VALUE.m_pMSGR->m_iOwnerSocetIDX,
                        pNODE->m_VALUE.m_pMSGR->m_dwDBID,
                        pCPacket);

                    Packet_ReleaseNUnlock(pCPacket);
                }
            }
            break;
        }

        pNODE = pNODE->GetNext();
    }
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void
CMessenger::MSGR_Ban(DWORD dwDBID) {
    /*	CDLList< tagMyFriend >::tagNODE *pNODE;

        pNODE = m_ListFRIEND.GetHeadNode ();
        while( pNODE ) {
            if ( dwDBID == pNODE->m_VALUE.m_dwDBID ) {
                // �̳��� �޼��� ����..
                if ( FRIEND_STATUS_DELETED != pNODE->m_VALUE.m_btSTATUS ) {
                    if ( FRIEND_STATUS_REFUSED & pNODE->m_VALUE.m_btSTATUS ) {
                        if ( pNODE->m_VALUE.m_pMSGR )
                            pNODE->m_VALUE.m_btSTATUS = pNODE->m_VALUE.m_pMSGR->m_btMsgrSTATUS;
                        else
                            pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_OFFLINE;
                    } else
                        pNODE->m_VALUE.m_btSTATUS = FRIEND_STATUS_REFUSED;
                }

                #pragma COMPILE_TIME_MSG ( " �ٲ� ���� ����??? " )

                break;
            }
            pNODE = pNODE->GetNext ();
        } */
}

void
CMessenger::MSGR_Status2ALL(BYTE btNewStatus) {
    this->m_btMsgrSTATUS = btNewStatus;

    classPACKET* pCPacket = Packet_AllocNLock();

    pCPacket->m_HEADER.m_wType = WSV_MESSENGER;
    pCPacket->m_HEADER.m_nSize = sizeof(wsv_MCMD_STATUS_REPLY);

    pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btCMD = MSGR_CMD_CHANGE_STATUS;
    pCPacket->m_wsv_MCMD_STATUS_REPLY.m_dwUserTAG = this->Get_DBID();
    pCPacket->m_wsv_MCMD_STATUS_REPLY.m_btStatus = btNewStatus;

    CDLList<tagMyFriend>::tagNODE* pNODE;

    pNODE = m_ListFRIEND.GetHeadNode();
    while (pNODE) {
        if (pNODE->m_VALUE.m_pMSGR) {
            this->SendPacket(pNODE->m_VALUE.m_pMSGR->m_iOwnerSocetIDX,
                pNODE->m_VALUE.m_pMSGR->m_dwDBID,
                pCPacket);
        }
        pNODE = pNODE->GetNext();
    }

    Packet_ReleaseNUnlock(pCPacket);
}

//-------------------------------------------------------------------------------------------------
#endif
