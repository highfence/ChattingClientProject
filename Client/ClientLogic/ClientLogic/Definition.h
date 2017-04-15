#pragma once
#include <string>

namespace SERVER_INFO
{
	const int serverPort = 23452;
	const std::string loopBackIp = "127.0.0.1";
};

namespace COMMON_INFO
{
	const int bufSize = 512;
	const int recvSize = 4000;
};

namespace PACKET
{
	struct PacketHeader
	{
		short Id;
		short BodySize;
	};

	const int packetHeaderSize = sizeof(PacketHeader);

	struct RecvPacketInfo
	{
		RecvPacketInfo() {};

		short PacketId = 0;
		short PacketBodySize = 0;
		char* pData = nullptr;
	};
}