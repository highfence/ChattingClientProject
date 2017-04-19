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
	const int MAX_PACKET_SIZE = 1024;
};