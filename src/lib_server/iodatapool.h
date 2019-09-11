/*
    $Header: /7HeartsOnline/LIB_Server/ioDataPOOL.h 1     04-03-25 11:20a Icarus $
*/
#ifndef __IODATA_POOL_H
#define __IODATA_POOL_H

#include "DLLIST.h"
#include "LIB_Util.h"
#include "classPACKET.h"

#include "CDataPOOL.h"

#define __USE_RECV_IODATA_POOL
#define __USE_SEND_IODATA_POOL

enum class IOMode {
    Read,
    Write,
};

struct tagIO_DATA {
    OVERLAPPED overlapped;
    IOMode mode;
    uint32_t bytes;
    classPACKET packet;
    classDLLNODE<tagIO_DATA>* node;

    // Required since the union has a member with non-trivial
    // consructor/destructor (classPacket). DON'T DELETE.
    /*
    tagIO_DATA(){};
    ~tagIO_DATA(){};
    */
};

typedef classDLLNODE<tagIO_DATA> IODATANODE;
typedef classDLLNODE<tagIO_DATA>* LPIODATANODE;

class CPoolRECVIO
#ifdef __USE_RECV_IODATA_POOL
    :
    public CDataPOOL<IODATANODE>
#endif
{
    DECLARE_INSTANCE2(CPoolRECVIO)
public:
    static CPoolRECVIO* Instance(UINT uiInitDataCNT, UINT uiIncDataCNT) {
        if (!m_pCPoolRECVIO) {
            m_pCPoolRECVIO = new CPoolRECVIO(uiInitDataCNT, uiIncDataCNT);
        }
        return m_pCPoolRECVIO;
    }

private:
    CPoolRECVIO(UINT uiInitDataCNT, UINT uiIncDataCNT);

public:
    inline void InitData(LPIODATANODE pData) {
        ::ZeroMemory(&pData->DATA.overlapped, sizeof(OVERLAPPED));
        // 2003. 11. 12 반드시 0으로 초기화 !!!, 빼먹어서 Recv_Start에서 기존의 쓰레기 패킷 뒤에
        // 추가로 받아졌다.
        pData->DATA.bytes = 0;

        pData->DATA.node = pData;
        pData->DATA.mode = IOMode::Read;
    }
};

//-------------------------------------------------------------------------------------------------
class CPoolSENDIO
#ifdef __USE_SEND_IODATA_POOL
    :
    public CDataPOOL<IODATANODE>
#endif
{
    DECLARE_INSTANCE2(CPoolSENDIO)
public:
    static CPoolSENDIO* Instance(UINT uiInitDataCNT, UINT uiIncDataCNT) {
        if (!m_pCPoolSENDIO) {
            m_pCPoolSENDIO = new CPoolSENDIO(uiInitDataCNT, uiIncDataCNT);
        }
        return m_pCPoolSENDIO;
    }

private:
    CPoolSENDIO(UINT uiInitDataCNT, UINT uiIncDataCNT);

public:
    inline void InitData(LPIODATANODE pData) {
        ::ZeroMemory(&pData->DATA.overlapped, sizeof(OVERLAPPED));
        pData->DATA.bytes = 0;

        pData->DATA.node = pData;
        pData->DATA.mode = IOMode::Write;
    }
};

inline classPACKET*
Packet_AllocOnly() {
    return new classPACKET();
}

inline classPACKET*
Packet_AllocNLock() {
    return new classPACKET();
}

inline void
Packet_ReleaseNUnlock(classPACKET* pCPacket) {
    delete pCPacket;
}

inline void
Packet_DecRefCount(classPACKET* pCPacket) {
    if (pCPacket <= 0) {
        delete pCPacket;
    }
}

#endif
