#ifndef __CSQLTHREAD_H
#define __CSQLTHREAD_H
#include "classSYNCOBJ.h"
#include "classTHREAD.h"
#include "classODBC.h"
#include "classSTR.h"
#include "CDLList.h"
#include "PacketHEADER.h"

#include "rose/network/packet.h"

#include <queue>
#include <mutex>
#include <string>

struct tagQueryDATA {
    int m_iTAG;
    CStrVAR m_Name;
    int m_iDataLEN;
    union {
        BYTE* m_pDATA;
        t_PACKETHEADER* m_pPacket;
    };
};

#define USE_MY_SQL 0x000
#define USE_ODBC 0x001

/// A network packet that needs to be processed by the sql thread
struct QueuedPacket {
    int32_t socket_id;
    Rose::Network::Packet packet;
};

class CSqlTHREAD: public classTHREAD {
public:
    std::unique_ptr<classODBC> db;

    std::mutex queue_mutex;
    std::queue<QueuedPacket> packet_queue;

protected:
    classEVENT* m_pEVENT;
    CCriticalSection m_CS;

    CDLList<tagQueryDATA> m_RunPACKET;
    CDLList<tagQueryDATA> m_AddPACKET;

    CDLList<char*> m_RunQUERY;
    CDLList<char*> m_AddQUERY;

    CDLList<tagQueryDATA>::tagNODE* Del_SqlPACKET(CDLList<tagQueryDATA>::tagNODE* pDelNODE);
    virtual bool Run_SqlPACKET(tagQueryDATA* pQueryDATA) = 0;

    virtual bool Proc_QuerySTRING();

public:
    CSqlTHREAD(bool bCreateSuspended);
    virtual ~CSqlTHREAD();

    //	virtual void Execute() = 0;

    bool Connect(BYTE btSqlTYPE,
        char* szServerIP,
        char* szUser,
        char* szPassword,
        char* szDBName,
        short nBindParamCNT = 64,
        WORD wQueryBufferLEN = 8192);
    void Free();

    bool Add_SqlPACKET(int iTAG, char* szName, BYTE* pDATA, int iDataSize);
    bool Add_QueryString(char* szQuery);

    void tick();
    void queue_packet(int32_t socket_id,
        const Rose::Network::Packet& p);

    virtual void handle_queued_packet(QueuedPacket& p){};
};

#endif
