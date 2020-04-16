#ifndef CNETWORK_H
#define CNETWORK_H

#include "NET_Prototype.h"
#include "../util/CClientSOCKET.h"
#include "Object.h"
#include "RecvPACKET.h"
#include "SendPACKET.h"

#include "rose/network/packet.h"

enum { NS_NULL = 0, NS_CON_TO_LSV, NS_DIS_FORM_LSV, NS_CON_TO_WSV };

class CNetwork: public CRecvPACKET, public CSendPACKET {
public:
    enum class Server {
        Login,
        World,
        Game,
    };

private:
    bool bAllInONE;
    void Send_PACKET(t_PACKET* pSendPacket, bool bSendToWorld = false) {
        if (bSendToWorld || bAllInONE)
            m_WorldSOCKET.Packet_Register2SendQ(pSendPacket);
        else
            m_ZoneSOCKET.Packet_Register2SendQ(pSendPacket);
    }

    short m_nProcLEVEL;
    bool m_bMoveingZONE;
    BYTE m_btZoneSocketSTATUS;

    static CNetwork* m_pInstance;
    CNetwork(HINSTANCE hInstance);
    ~CNetwork();

    void Proc_WorldPacket();
    void Proc_ZonePacket();

    void MoveZoneServer();

public:
    CClientSOCKET m_WorldSOCKET;
    CClientSOCKET m_ZoneSOCKET;

    bool m_bWarping;

    static CNetwork* Instance(HINSTANCE hInstance);
    void Destroy();

    bool ConnectToServer(std::string& ip, WORD wTcpPORT, short nProcLEVEL = 0);
    void DisconnectFromServer(short nProcLEVEL = 0);

    void Send_AuthMsg(void);

    void Proc();

    void send_packet(const Rose::Network::Packet& packet, Server target = Server::Game);
    void send_login_req(const std::string& username, const std::string& password);
};
extern CNetwork* g_pNet;
#endif