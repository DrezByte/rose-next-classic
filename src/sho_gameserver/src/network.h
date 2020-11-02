#pragma once

// Forward declarations
namespace Rose::Network {
class Packet;
}

class classUSER;

namespace Rose::Network {

bool send_packet(classUSER& user, Packet& packet);
bool send_packet_party(CParty& party, Packet& packet);
bool send_packet_nearby(CGameOBJ& object, Packet& packet);

template<typename T>
Packet
build_packet_from_offset(flatbuffers::FlatBufferBuilder& builder,
    flatbuffers::Offset<T> offset,
    Packets::PacketType type) {

    Packets::PacketDataBuilder pd(builder);
    pd.add_data_type(type);
    pd.add_data(offset.Union());
    builder.Finish(pd.Finish());

    Packet p(builder);
    return p;
}

Packet build_char_move_packet(CObjCHAR& character);
//Packet build_update_stats_all_packet(classUSER& user);
Packet build_update_hpmp_packet(classUSER& user, uint32_t hp, uint32_t mp);
//Packet build_update_move_speed_packet(classUSER& user, uint16_t move_speed);

// Uses legacy packet building methods
//bool send_server_whisper(classUSER& user, const std::string& message);
} // namespace Rose::Network
