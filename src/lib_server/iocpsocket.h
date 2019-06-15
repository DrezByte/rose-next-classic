#ifndef iocpSOCKETH
#define iocpSOCKETH

#include "CDLList.h"
#include "DLLIST.h"
#include "classHASH.h"
#include "classSTR.h"
#include "classSYNCOBJ.h"
#include "classTIME.h"
#include "ioDataPOOL.h"

enum ePacketRECV { eRESULT_PACKET_OK = 0, eRESULT_PACKET_DISCONNECT, eRESULT_PACKET_BLOCK };

class iocpSOCKET {
private:
    ePacketRECV Recv_Continue(tagIO_DATA* pRecvDATA);

    bool Send_Continue(tagIO_DATA* pSendDATA);
    CCriticalSection m_csSOCKET;

protected:
    CCriticalSection m_csSendQ;
    CCriticalSection m_csRecvQ;

    classDLLIST<tagIO_DATA> m_SendList; // Client에게 보낼 데이타 리스트.
    bool m_bWritable; // WriteFile에 보낼수 있냐 ?

    classDLLIST<tagIO_DATA> m_RecvList; // Client에서 받은 데이타 리스트.
    DWORD m_dwCheckTIME; // 마지막 보내기 시도한 시간

public:
    void CloseSocket(void);

    SOCKET m_Socket;
    int m_iSocketIDX;
    CStrVAR m_IP;
    t_HASHKEY m_HashKeyIP;

    bool m_bVerified; // 맞는 클라이언트에서 접속된거냐 ??
    CDLList<iocpSOCKET*>::tagNODE* m_pSockNODE;
    DWORD m_dwConnTIME; // 접속된 시간

    iocpSOCKET();
    virtual ~iocpSOCKET();

    void LockSOCKET() { this->m_csSOCKET.Lock(); }
    void UnlockSOCKET() { this->m_csSOCKET.Unlock(); }

    void Init_SCOKET() {
        m_iSocketIDX = 0;
        m_Socket = INVALID_SOCKET;
        m_bWritable = true;
        m_bVerified = false;
        m_pSockNODE = NULL;
    }

    SOCKET Get_SOCKET() { return m_Socket; }
    char* Get_IP() { return m_IP.Get(); }
    DWORD Get_CheckTIME() { return m_dwCheckTIME; }
    void Clear_LIST(void);

    ePacketRECV Recv_Start(void);
    ePacketRECV Recv_Complete(tagIO_DATA* pRecvDATA);

    bool Send_Start(const classPACKET& packet);
    bool Send_Complete(tagIO_DATA* pSendDATA);

    //	virtual bool OnIdelCLOSE()							{	return true;				}

    virtual bool Recv_Done(tagIO_DATA* pRecvDATA);
    virtual bool HandlePACKET(t_PACKETHEADER* pPacket) = 0;

    classDLLNODE<tagIO_DATA>* Alloc_RecvIODATA(void) {
        classDLLNODE<tagIO_DATA>* pRecvDATA = CPoolRECVIO::GetInstance()->Pool_Alloc();
        if (pRecvDATA) {
            CPoolRECVIO::GetInstance()->InitData(pRecvDATA);
            pRecvDATA->DATA.m_pCPacket = classPACKET();
        }

        return pRecvDATA;
    }

    static void Free_RecvIODATA(tagIO_DATA* pRecvDATA) {
        CPoolRECVIO::GetInstance()->Pool_Free(pRecvDATA->m_pNODE);
    }

    classDLLNODE<tagIO_DATA>* Alloc_SendIODATA(const classPACKET& pCPacket) {
        classDLLNODE<tagIO_DATA>* pSendDATA;
        pSendDATA = CPoolSENDIO::GetInstance()->Pool_Alloc();
        if (NULL != pSendDATA) {
            CPoolSENDIO::GetInstance()->InitData(pSendDATA);
            pSendDATA->DATA.m_pCPacket = pCPacket;

            _ASSERT(pSendDATA->DATA.m_IOmode == ioWRITE);
            _ASSERT(pSendDATA->DATA.m_dwIOBytes == 0);
        }
        return pSendDATA;
    }

    static void Free_SendIODATA(tagIO_DATA* pSendDATA) {
        CPoolSENDIO::GetInstance()->Pool_Free(pSendDATA->m_pNODE);
    }
};

#endif
