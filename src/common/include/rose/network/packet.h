#pragma once

namespace Rose {
namespace Network {
	const uint16_t MAX_PACKET = 4096;

	class Packet {
	public:
		uint16_t size;
		uint8_t buffer[4096];
	};
}
}