/*
    $Header: /Client/Network/CNetwork.cpp 168   05-10-18 3:10p Young $
*/
#include "stdAFX.h"

#include "CNetwork.h"
#include "Game.h"
#include "../Interface/ExternalUI/ExternalUILobby.h"
#include "../System/CGame.h"
#include "CSocketWND.h"
#include "../gameproc/LiveCheck.h"

#include "system/cgamestate.h"
#include "util/classmd5.h"

#include "rose/common/game_types.h"
#include "rose/network/packet.h"
#include "rose/network/packets/packet_data_generated.h"

#include "rose/network/packets/char_create_req_generated.h"
#include "rose/network/packets/login_req_generated.h"

CNetwork* g_pNet;

CNetwork* CNetwork::m_pInstance = NULL;

#define WM_WORLD_SOCKET_NOTIFY (WM_SOCKETWND_MSG + 0)
#define WM_ZONE_SOCKET_NOTIFY (WM_SOCKETWND_MSG + 1)

using namespace Rose::Common;
using namespace Rose::Network;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
CNetwork::CNetwork(HINSTANCE hInstance) {
    m_btZoneSocketSTATUS = 0;
    m_nProcLEVEL = NS_NULL;

    CSocketWND* pSockWND = CSocketWND::InitInstance(hInstance, 2);
    if (pSockWND) {
        pSockWND->AddSocket(&this->m_WorldSOCKET, WM_WORLD_SOCKET_NOTIFY);
        pSockWND->AddSocket(&this->m_ZoneSOCKET, WM_ZONE_SOCKET_NOTIFY);
    }
}
CNetwork::~CNetwork() {
    CSOCKET::Free();

    if (CSocketWND::GetInstance())
        CSocketWND::GetInstance()->Destroy();
}
CNetwork*
CNetwork::Instance(HINSTANCE hInstance) {
    if (m_pInstance == NULL) {
        if (CSOCKET::Init()) {
            m_pInstance = new CNetwork(hInstance);
        }
    }

    return m_pInstance;
}
void
CNetwork::Destroy() {
    m_WorldSOCKET.Close();
    m_ZoneSOCKET.Close();
    SAFE_DELETE(m_pInstance);
}

//-------------------------------------------------------------------------------------------------
bool
CNetwork::ConnectToServer(std::string& ip, WORD wTcpPORT, short nProcLEVEL) {
    // World ����...
    if (m_nProcLEVEL == nProcLEVEL)
        return true;

    m_nProcLEVEL = nProcLEVEL;
    return m_WorldSOCKET.Connect(CSocketWND::GetInstance()->GetWindowHandle(),
        (char*)ip.c_str(),
        wTcpPORT,
        WM_WORLD_SOCKET_NOTIFY);
}

//-------------------------------------------------------------------------------------------------

void
CNetwork::DisconnectFromServer(short nProcLEVEL) {
    m_nProcLEVEL = nProcLEVEL;
    m_WorldSOCKET.Close();
    m_WorldSOCKET.Set_NetSTATUS(NETWORK_STATUS_DISCONNECT);
}

//-------------------------------------------------------------------------------------------------
void
CNetwork::MoveZoneServer() {
    if (NETWORK_STATUS_CONNECT == m_btZoneSocketSTATUS) {
        // �� ���� ������ ���� ���� �����Ѵ�...
        m_bMoveingZONE = true;
        m_ZoneSOCKET.Close();
    } else {
        // �ٷ� ����...
        m_bMoveingZONE = false;
        m_ZoneSOCKET.Connect(CSocketWND::GetInstance()->GetWindowHandle(),
            m_GSV_IP.Get(),
            m_wGSV_PORT,
            WM_ZONE_SOCKET_NOTIFY);
    }
}

void
CNetwork::Proc_ZonePacket() {
    switch (m_pRecvPacket->m_HEADER.m_wType) {
        case GSV_CART_RIDE:
            Recv_gsv_CART_RIDE();
            break;

        case GSV_PATSTATE_CHANGE:
            Recv_gsv_PATSTATE_CHANGE();
            break;

        case SRV_CHECK_AUTH:
            break;
        case GSV_SET_NPC_SHOW:
            Recv_gsv_SET_NPC_SHOW();
            break;

        case GSV_MOVE_ZULY:
            Recv_gsv_MOVE_ZULY();
            break;
        case WSV_CLANMARK_REG_TIME:
            Recv_wsv_CLANMARK_REG_TIME();
            break;
        case GSV_BILLING_MESSAGE:
            Recv_gsv_BILLING_MESSAGE();
            break;
        case GSV_BILLING_MESSAGE_EXT:
            Recv_gsv_BILLING_MESSAGE_EXT();
            break;
        case GSV_ITEM_RESULT_REPORT:
            Recv_gsv_ITEM_RESULT_REPORT();
            break;
        case GSV_LOGOUT_REPLY:
            Recv_gsv_LOGOUT_REPLY();
            break;

        case WSV_CLAN_COMMAND:
            Recv_wsv_CLAN_COMMAND();
            break;
        case WSV_CLAN_CHAT:
            Recv_wsv_CLAN_CHAT();
            break;
        case WSV_CLANMARK_REPLY:
            Recv_wsv_CLANMARK_REPLY();
            break;

        case SRV_ERROR:
            Recv_srv_ERROR();
            break;

        case GSV_RELAY_REQ:
            this->Send_PACKET(m_pRecvPacket);
            break;

        case GSV_SET_GLOBAL_VAR:
            Recv_gsv_SET_GLOBAL_VAR();
            break;

        case GSV_SET_GLOBAL_FLAG:
            Recv_gsv_SET_GLOVAL_FLAG();
            break;

        case SRV_ANNOUNCE_TEXT:
            Recv_srv_ANNOUNCE_TEXT();
            break;
        case GSV_ANNOUNCE_CHAT:
            Recv_gsv_ANNOUNCE_CHAT();
            break;

        case GSV_GM_COMMAND:
            Recv_gsv_GM_COMMAND();
            break;

        // ĳ���� ���� ��� �뺸����
        case GSV_SELECT_CHAR:
            Recv_gsv_SELECT_CHAR();

            // Send_cli_JOIN_ZONE ();		// GSV_SELECT_CHAR
            break;

        case GSV_JOIN_ZONE:
            Recv_gsv_JOIN_ZONE();
            break;

        case GSV_REVIVE_REPLY:
            Recv_gsv_REVIVE_REPLY();
            break;

        case GSV_SET_VAR_REPLY:
            Recv_gsv_SET_VAR_REPLY();
            break;

        case GSV_TELEPORT_REPLY:
            this->m_bWarping = false;
            Recv_gsv_TELEPORT_REPLY();
            //				Send_cli_JOIN_ZONE ();		// GSV_TELEPORT_REPLY
            break;

        case GSV_INVENTORY_DATA:
            Recv_gsv_INVENTORY_DATA();
            //				g_pCApp->SetStatus( AS_MAIN_GAME );
            break;
        case GSV_QUEST_DATA:
            Recv_gsv_QUEST_DATA();
            break;
        case GSV_INIT_DATA:
            Recv_gsv_INIT_DATA();
            break;

        case GSV_CHEAT_CODE:
            Recv_gsv_CHEAT_CODE();
            break;

        case GSV_SET_MOTION:
            Recv_gsv_SET_MOTION();
            break;

        case GSV_TOGGLE:
            Recv_gsv_TOGGLE();
            break;
        case GSV_PARTY_CHAT:
            Recv_gsv_PARTY_CHAT();
            break;
        case GSV_CHAT:
            Recv_gsv_CHAT();
            break;
        case GSV_WHISPER:
            Recv_gsv_WHISPER();
            break;
        case GSV_SHOUT:
            Recv_gsv_SHOUT();
            break;

        case GSV_NPC_CHAR:
            Recv_gsv_NPC_CHAR();
            break;
        case GSV_MOB_CHAR:
            Recv_gsv_MOB_CHAR();
            break;
        case GSV_AVT_CHAR:
            Recv_gsv_AVT_CHAR();
            break;

        case GSV_SUB_OBJECT:
            Recv_gsv_SUB_OBJECT();
            break;
        case GSV_STOP:
            Recv_gsv_STOP();
            break;

        case GSV_MOVE:
            Recv_gsv_MOVE();
            break;
        case GSV_ATTACK:
            Recv_gsv_ATTACK();
            break;
            // case GSV_ATTACK_START :
            //	Recv_gsv_ATTACK_START ();
            //	break;

        case GSV_CHANGE_NPC:
            Recv_gsv_CHANGE_NPC();
            break;

        case GSV_DAMAGE:
            Recv_gsv_DAMAGE();
            break;

        case GSV_SETEXP:
            Recv_gsv_SETEXP();
            break;
        case GSV_LEVELUP:
            Recv_gsv_LEVELUP();
            break;

        case GSV_HP_REPLY:
            Recv_gsv_HP_REPLY();
            break;

        case GSV_CHANGE_SKIN:
            Recv_gsv_CHANGE_SKIN();
            break;

        case GSV_EQUIP_ITEM:
            Recv_gsv_EQUIP_ITEM();
            break;

        case GSV_ADD_FIELDITEM:
            Recv_gsv_ADD_FIELDITEM();
            break;

        case GSV_GET_FIELDITEM_REPLY:
            Recv_gsv_GET_FIELDITEM_REPLY();
            break;
        case GSV_MOUSECMD:
            Recv_gsv_MOUSECMD();
            break;

        case GSV_SET_WEIGHT_RATE:
            Recv_gsv_SET_WEIGHT_RATE();
            break;

        case GSV_ADJUST_POS:
            Recv_gsv_ADJUST_POS();
            break;

        case GSV_SKILL_LEARN_REPLY:
            Recv_gsv_SKILL_LEARN_REPLY();
            break;

        case GSV_SKILL_LEVELUP_REPLY:
            Recv_gsv_SKILL_LEVELUP_REPLY();
            break;

        case GSV_SELF_SKILL:
            Recv_gsv_SELF_SKILL();
            break;
        case GSV_TARGET_SKILL:
            Recv_gsv_TARGET_SKILL();
            break;
        case GSV_POSITION_SKILL:
            Recv_gsv_POSITION_SKILL();
            break;

        case GSV_EFFECT_OF_SKILL:
            Recv_gsv_EFFECT_OF_SKILL();
            break;
        case GSV_DAMAGE_OF_SKILL:
            Recv_gsv_DAMAGE_OF_SKILL();
            break;
        case GSV_RESULT_OF_SKILL:
            Recv_gsv_RESULT_OF_SKILL();
            break;

        case GSV_SKILL_START:
            Recv_gsv_SKILL_START();
            break;

        case GSV_SKILL_CANCEL:
            Recv_gsv_SKILL_CANCEL();
            break;

        case GSV_CLEAR_STATUS:
            Recv_gsv_CLEAR_STATUS();
            break;

        case GSV_SPEED_CHANGED:
            Recv_gsv_SPEED_CHANGED();
            break;

        /// �������� ����ߴ�.
        case GSV_USE_ITEM:
            Recv_gsv_USE_ITEM();
            break;

        case GSV_P_STORE_MONEYnINV:
            Recv_gsv_P_STORE_MONEYnINV();
            break;
        case GSV_SET_MONEYnINV:
            Recv_gsv_SET_MONEYnINV();
            break;

        case GSV_REWARD_ITEM: /// ����Ʈ ���� ���� �������� ����
        {
            Recv_gsv_REWARD_ITEM();
        }

        case GSV_SET_INV_ONLY:
            Recv_gsv_SET_INV_ONLY();
            break;

        case GSV_SET_HOTICON:
            Recv_gsv_SET_HOTICON();
            break;

        case GSV_USE_BPOINT_REPLY:
            Recv_gsv_USE_BPOINT_REPLY();
            break;

        case GSV_QUEST_REPLY:
            Recv_gsv_QUEST_REPLY();
            break;
        /*
        case GSV_QUEST_DATA_REPLY :
            Recv_gsv_QUEST_DATA_REPLY ();
            break;
        */
        case GSV_TRADE_P2P:
            Recv_gsv_TRADE_P2P();
            break;
        case GSV_TRADE_P2P_ITEM:
            Recv_gsv_TRADE_P2P_ITEM();
            break;

        case GSV_PARTY_REQ:
            Recv_gsv_PARTY_REQ();
            break;
        case GSV_PARTY_REPLY:
            Recv_gsv_PARTY_REPLY();
            break;
        case GSV_PARTY_MEMBER:
            Recv_gsv_PARTY_MEMBER();
            break;
        case GSV_PARTY_LEVnEXP:
            Recv_gsv_PARTY_LEVnEXP();
            break;
        case GSV_PARTY_ITEM:
            Recv_gsv_PARTY_ITEM();
            break;
        case GSV_PARTY_RULE:
            Recv_gsv_PARTY_RULE();
            break;
        case GSV_CHANGE_OBJIDX:
            Recv_gsv_CHANGE_OBJIDX();
            break;
        case GSV_STORE_TRADE_REPLY:
            Recv_gsv_STORE_TRADE_REPLY();
            break;
        case GSV_CREATE_ITEM_REPLY:
            Recv_gsv_CREATE_ITEM_REPLY();
            break;
        case GSV_BANK_LIST_REPLY:
            Recv_gsv_BANK_LIST_REPLY();
            break;
        case GSV_MOVE_ITEM:
            Recv_gsv_MOVE_ITEM();
            break;
        case GSV_SET_BULLET:
            Recv_gsv_SET_BULLET();
            break;
        case GSV_SERVER_DATA:
            Recv_gsv_SERVER_DATA();
            break;
        case GSV_ASSEMBLE_RIDE_ITEM:
            Recv_gsv_ASSEMBLE_RIDE_ITEM();
            break;

        case GSV_SET_EVENT_STATUS:
            Recv_GSV_SET_EVENT_STATUS();
            break;

        case GSV_SET_ITEM_LIFE:
            Recv_GSV_SET_ITEM_LIFE();
            break;

        case GSV_P_STORE_OPENED:
            Recv_gsv_P_STORE_OPENED();
            break;
        case GSV_P_STORE_CLOSED:
            Recv_gsv_P_STORE_CLOSED();
            break;
        case GSV_P_STORE_LIST_REPLY:
            Recv_gsv_P_STORE_LIST_REPLY();
            break;
        case GSV_P_STORE_RESULT:
            Recv_gsv_P_STORE_RESULT();
            break;

        case GSV_USED_ITEM_TO_REPAIR:
            Recv_gsv_USED_ITEM_TO_REPAIR();
            break;
        case GSV_REPAIRED_FROM_NPC:
            Recv_gsv_REPAIRED_FROM_NPC();
            break;

        case GSV_SET_MONEY_ONLY:
            Recv_gsv_SET_MONEY_ONLY();
            break;
        case GSV_REWARD_MONEY:
            Recv_gsv_REWARD_MONEY();
            break;

        case GSV_REWARD_ADD_ABILITY: {
            Recv_gsv_REWARD_ADD_ABILITY();
            break;
        }
        case GSV_REWARD_SET_ABILITY: {
            Recv_gsv_REWARD_SET_ABILITY();
            break;
        }

        case GSV_GODDNESS_MODE: {
            Recv_gsv_GODDNESS_MODE();
            break;
        }

        //----------------------------------------------------------------------------------------------------
        /// @brief ������ ��ְ���
        //----------------------------------------------------------------------------------------------------
        case GSV_CRAFT_ITEM_REPLY:
            Recv_gsv_CRAFT_ITEM_REPLY();
            break;

        //----------------------------------------------------------------------------------------------------
        /// @brief �̺�Ʈ ������Ʈ ����
        //----------------------------------------------------------------------------------------------------
        case GSV_ADD_EVENTOBJ:
            Recv_gsv_ADD_EVENTOBJ();
            break;

        case GSV_APPRAISAL_REPLY:
            Recv_gsv_APPRAISAL_REPLY();
            break;

        case GSV_SET_HPnMP:
            Recv_gsv_SET_HPnMP();
            break;

        case GSV_CHECK_NPC_EVENT:
            Recv_gsv_CHECK_NPC_EVENT();
            break;

        case GSV_ALLIED_CHAT:
            Recv_gsv_ALLIED_CHAT();
            break;
        case GSV_ALLIED_SHOUT:
            Recv_gsv_ALLIED_SHOUT();
            break;

        case GSV_CHARSTATE_CHANGE:
            Recv_gsv_CHARSTATE_CHANGE();
            break;

        case GSV_SCREEN_SHOT_TIME:
            Recv_gsv_SCREEN_SHOT_TIME();
            break;

        case SRV_UPDATE_NAME:
            Recv_gsv_UPDATE_NAME();
            break;

        default:
            //_ASSERT(0);
            LogString(LOG_NORMAL,
                "received Invalid packet type ... type: 0x%x , size: %d \n",
                m_pRecvPacket->m_HEADER.m_wType,
                m_pRecvPacket->m_HEADER.m_nSize);
    }
}

//-------------------------------------------------------------------------------------------------
void
CNetwork::Proc() {
    while (m_WorldSOCKET.Peek_Packet(m_pRecvPacket, true)) {
        this->recv_packet(m_pRecvPacket);

        switch (m_pRecvPacket->m_HEADER.m_wType) {
            case SOCKET_NETWORK_STATUS: {
                switch (m_pRecvPacket->m_NetSTATUS.m_btStatus) {
                    case NETWORK_STATUS_ACCEPTED: {
                        CGame::GetInstance().AcceptedConnectLoginSvr();
                        CGame::GetInstance().active_state->on_loginserver_connected();
                        continue;
                    }
                    case NETWORK_STATUS_CONNECT: {
                        // ������ ����ƴ�...
                        switch (m_nProcLEVEL) {
                            case NS_CON_TO_WSV: // ���� ������ �����ߴ�.
                                Send_cli_JOIN_SERVER_REQ(m_dwWSV_ID, true);
                                m_bWarping = false;
                                bAllInONE = true;
                                break;
                            case NS_CON_TO_LSV: // �α� ������ �����ߴ�.
                                Send_cli_HEADER(CLI_ACCEPT_REQ, true);

                                break;
                        }
                        continue;
                    }
                    case NETWORK_STATUS_DISCONNECT: {
                        if (NS_DIS_FORM_LSV == m_nProcLEVEL) {
                            // ���� ������ ������ �Ѵ�...
                            std::string world_server_ip(m_WSV_IP.Get());
                            this->ConnectToServer(world_server_ip, m_wWSV_PORT, NS_CON_TO_WSV);
                            continue;
                        }
                        CGame::GetInstance().ProcWndMsg(WM_USER_WORLDSERVER_DISCONNECTED, 0, 0);
                        break;
                    }
                    case NETWORK_STATUS_DERVERDEAD: {
                        g_pCApp->SetCaption("Server DEAD");
                        g_pCApp->ErrorBOX(STR_SERVER_EXAMINE, STR_SERVER_INFO, MB_OK);
                        g_pCApp->SetExitGame();
                        break;
                    }
                }

                LogString(LOG_NORMAL, "�������� ���ӿ� �����߽��ϴ�.\n");
                break;
            }
            case SRV_ERROR:
                Recv_srv_ERROR();
                break;

            case SRV_JOIN_SERVER_REPLY: // ���� ������ �����ߴ�.
            {
                DWORD dwRet = Recv_srv_JOIN_SERVER_REPLY();
                if (dwRet) {
                    CLiveCheck::GetSingleton().ResetTime();
                    this->Send_cli_CHAR_LIST();
                    CGame::GetInstance().active_state->on_charserver_connected();
                } else {
                    // TODO:: error
                    this->DisconnectFromServer();
                    CGame::GetInstance().active_state->on_charserver_connect_failed();
                    continue;
                }
                break;
            }

            case LSV_LOGIN_REPLY: {
                const bool login_succeeded = Recv_lsv_LOGIN_REPLY();
                if (login_succeeded) {
                    CGame::GetInstance().active_state->on_login_succeeded();
                } else {
                    this->DisconnectFromServer();
                    CGame::GetInstance().active_state->on_login_failed(
                        m_pRecvPacket->m_srv_LOGIN_REPLY.m_btResult & ~0x80);
                    continue;
                }
                break;
            }
            case LSV_SELECT_SERVER: {
                DWORD dwRet = Recv_lsv_SELECT_SERVER();
                if (dwRet == 0) {
                    CGame::GetInstance().active_state->on_charserver_connect_failed();
                }
                break;
            }
            case LSV_CHANNEL_LIST_REPLY:
                Recv_lsv_CHANNEL_LIST_REPLY();
                break;
            // ĳ���� ����Ʈ �޾���
            case WSV_CHAR_LIST:
                Recv_wsv_CHAR_LIST();
                break;

            case WSV_DELETE_CHAR:
                Recv_wsv_DELETE_CHAR();
                break;
            // ĳ���� ������û ��� �뺸����
            case WSV_CREATE_CHAR:
                Recv_wsv_CREATE_CHAR();

                break;
            case WSV_MESSENGER:
                Recv_tag_MCMD_HEADER();
                break;
            case WSV_MESSENGER_CHAT:
                Recv_wsv_MESSENGER_CHAT();
                break;
            case WSV_MEMO:
                Recv_wsv_MEMO();
                break;
            case WSV_CHATROOM:
                Recv_wsv_CHATROOM();
                break;
            case WSV_CHATROOM_MSG:
                Recv_wsv_CHATROOM_MSG();
                break;
            case WSV_CHAR_CHANGE:
                Recv_wsv_CHAR_CHANGE();
                break;
            // �� ������ �̵��ض�...
            case WSV_MOVE_SERVER: {
                bAllInONE = false;
                Recv_wsv_MOVE_SERVER();
                MoveZoneServer();
                break;
            }

            case GSV_GODDNESS_MODE: {
                Recv_gsv_GODDNESS_MODE();
                break;
            }

            default:
                Proc_ZonePacket();
        }
    }

    while (m_ZoneSOCKET.Peek_Packet(m_pRecvPacket, true)) {
        this->recv_packet(m_pRecvPacket);

        switch (m_pRecvPacket->m_HEADER.m_wType) {
            case SOCKET_NETWORK_STATUS: {
                m_btZoneSocketSTATUS = m_pRecvPacket->m_NetSTATUS.m_btStatus;
                switch (m_pRecvPacket->m_NetSTATUS.m_btStatus) {
                    case NETWORK_STATUS_ACCEPTED: {
                        continue;
                    }
                    case NETWORK_STATUS_CONNECT: {
                        // �� ������ ����ƴ�...
                        // �ɸ��� ������ ���� ���� ����.
                        m_bWarping = false;
                        this->Send_cli_JOIN_SERVER_REQ(m_dwGSV_IDs[0]);
                        continue;
                    }
                    case NETWORK_STATUS_DISCONNECT: {
                        // ������ �ű�� ���� ������ �������ΰ� ? ������ΰ� ?
                        if (m_bMoveingZONE) {
                            this->MoveZoneServer();
                            continue;
                        }
                        CGame::GetInstance().ProcWndMsg(WM_USER_SERVER_DISCONNECTED, 0, 0);
                        break;
                    }
                    case NETWORK_STATUS_DERVERDEAD: {
                        break;
                    }
                }

                LogString(LOG_NORMAL, "�� �������� ���ӿ� �����߽��ϴ�.\n");
                break;
            }

            default:
                this->Proc_ZonePacket();
        }
    }
}

void
CNetwork::Send_AuthMsg(void) {

    m_pSendPacket->m_HEADER.m_wType = CLI_CHECK_AUTH;
    m_pSendPacket->m_HEADER.m_nSize = sizeof(cli_CHECK_AUTH);
    Send_PACKET(m_pSendPacket);
}

void
CNetwork::recv_packet(t_PACKET* packet) {
    flatbuffers::Verifier verifier(&packet->m_pDATA[2], packet->size - 2);
    bool valid = Packets::VerifyPacketDataBuffer(verifier);
    if (valid) {
        Packet p(&packet->m_pDATA[0], packet->size);

        Packets::PacketType packet_type = p.packet_data()->data_type();
        switch (packet_type) {
            case Packets::PacketType::UpdateStats: {
                return this->recv_update_stats(p);
            }
            default: {
                LOG_WARN("Received unknown packet type %d", packet_type);
                break;
            }
        }
    }
}

void
CNetwork::recv_update_stats(Packet& p) {
    if (!g_pAVATAR || !p.packet_data()) {
        return;
    }

    const Packets::UpdateStats* req = p.packet_data()->data_as_UpdateStats();
    if (!req || !req->stats()) {
        return;
    }

    uint32_t target_id = req->target_id();
    const Packets::Stats* stats = req->stats();
    if (!stats) {
        return;
    }

    CObjAVT* target = g_pObjMGR->Get_ClientCharAVT(target_id, false);
    if (!target) {
        return;
    }

    if (target->IsA(OBJ_USER)) {
        target = g_pAVATAR;
    }

    if (flatbuffers::IsFieldPresent(stats, Packets::Stats::VT_MOVE_SPEED)) {
        target->stats.move_speed = stats->move_speed();
    }
    if (flatbuffers::IsFieldPresent(stats, Packets::Stats::VT_ATTACK_SPEED)) {
        target->stats.attack_speed = stats->attack_speed();
    }

    if (flatbuffers::IsFieldPresent(stats, Packets::Stats::VT_HIT_RATE)) {
        target->stats.hit_rate = stats->hit_rate();
    }

    if (target->IsA(OBJ_USER)) {
        g_pAVATAR->UpdateAbility();
    }
}

void
CNetwork::send_packet(const Packet& packet, Server target) {
    if (target == Server::World || target == Server::Login || bAllInONE) {
        m_WorldSOCKET.add_send_packet(packet);
    } else {
        m_ZoneSOCKET.add_send_packet(packet);
    }
}

void
CNetwork::send_char_create_req(const std::string& name,
    int face_id,
    int hair_id,
    Gender gender,
    Job job) {

    bool valid_job = (job == Job::Visitor || is_first_job(job));

    if (name.empty() || face_id <= 0 || hair_id <= 0 || !valid_job) {
        return;
    }

    flatbuffers::FlatBufferBuilder builder;
    const auto name_string = builder.CreateString(name);

    Packets::CharacterCreateRequestBuilder req(builder);
    req.add_name(name_string);
    req.add_face_id(face_id);
    req.add_hair_id(hair_id);
    req.add_gender_id(static_cast<int>(gender));
    req.add_job_id(static_cast<int>(job));
    const auto char_create_req = req.Finish();

    Packets::PacketDataBuilder pd(builder);
    pd.add_data_type(Packets::PacketType::CharacterCreateRequest);
    pd.add_data(char_create_req.Union());
    builder.Finish(pd.Finish());

    Packet p(builder);
    this->send_packet(p, Server::World);
}

void
CNetwork::send_login_req(const std::string& username, const std::string& password) {
    if (username.empty()) {
        return;
    }

    for (const char& c: username) {
        if (c == '\'' || c == '\"') {
            return;
        }
    }

    GetMD5(this->m_pMD5Buff,
        reinterpret_cast<unsigned char*>(const_cast<char*>(password.c_str())),
        password.size());

    flatbuffers::FlatBufferBuilder builder;
    const auto username_string = builder.CreateString(username);
    const auto password_string = builder.CreateString(reinterpret_cast<char*>(m_pMD5Buff), 32);

    Packets::LoginRequestBuilder req(builder);
    req.add_username(username_string);
    req.add_password(password_string);
    const auto login_request = req.Finish();

    Packets::PacketDataBuilder pd(builder);
    pd.add_data_type(Packets::PacketType::LoginRequest);
    pd.add_data(login_request.Union());
    builder.Finish(pd.Finish());

    Packet p(builder);
    this->send_packet(p, Server::Login);
    return;
}
