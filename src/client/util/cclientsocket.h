/*
    $Header: /7HeartsOnline/Client/Util/CClientSOCKET.h 3     04-11-05 12:14a Icarus $
*/
#ifndef __CClientSOCKET_H
#define __CClientSOCKET_H
#include "..\Util\DLLIST.h"
#include "CRawSOCKET.h"
#include "PacketHEADER.h"
//-------------------------------------------------------------------------------------------------

struct t_PACKET;
struct t_SendPACKET;

class CClientSOCKET: public CRawSOCKET {
private:
    bool _Init(void);
    void _Free(void);

    t_PACKET* m_pRecvPacket;
    short m_nRecvBytes;
    short m_nPacketSize;

    short m_nSendBytes;
    bool m_bWritable;

    CRITICAL_SECTION m_csThread;
    HANDLE m_hThreadEvent;
    HANDLE m_hThread;
    // DWORD			m_dwThreadID;
    unsigned int m_dwThreadID;

    char m_cStatus;

    classDLLIST<t_PACKET*> m_RecvPacketQ;
    classDLLIST<t_SendPACKET*> m_SendPacketQ;
    classDLLIST<t_SendPACKET*> m_WaitPacketQ;

    classDLLIST<struct tagUDPPACKET*> m_RecvUDPPacketQ;

protected:
    // friend	DWORD WINAPI ClientSOCKET_SendTHREAD (LPVOID lpParameter);
    friend unsigned __stdcall ClientSOCKET_SendTHREAD(void* lpParameter);

    void Packet_Recv(int iToRecvBytes);
    bool Packet_Send(void);

public:
    classDLLNODE<CClientSOCKET*>* m_pNode;

    CClientSOCKET() { _Init(); }
    virtual ~CClientSOCKET() { _Free(); }

    void SetSendEvent() { ::SetEvent(m_hThreadEvent); }

    virtual void OnConnect(int nErrorCode);
    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    virtual void OnSend(int nErrorCode);
    virtual void OnAccepted(int* pCode);
    virtual bool WndPROC(WPARAM wParam, LPARAM lParam);

public:
    // user interface
    bool Connect(HWND hWND, char* szServerIP, int iTCPPort, UINT uiWindowMsg);
    void Close();

    virtual void Set_NetSTATUS(BYTE btStatus);

    void Packet_Register2RecvQ(t_PACKET* pRegPacket);
    void Packet_Register2SendQ(t_PACKET* pRegPacket);
    bool Peek_Packet(t_PACKET* pPacket, bool bRemoveFromQ = true);
};

//-------------------------------------------------------------------------------------------------
#endif
