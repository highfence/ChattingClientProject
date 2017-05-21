#pragma once
#include <string>
#include <chrono>
#include <ctime>

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

class ChatData
{
public :
	ChatData() = default;
	ChatData(std::wstring id, std::wstring chat);
	~ChatData() = default;

	void DataSetting(std::wstring id, std::wstring chat);
	std::wstring GetInLine();

private :

	std::wstring m_Id;
	std::chrono::system_clock::time_point m_ChatTime;
	std::wstring m_Chat;

};