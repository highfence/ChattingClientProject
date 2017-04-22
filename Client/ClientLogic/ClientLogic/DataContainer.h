#pragma once

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <deque>
#include <string>
#include <mutex>
#include <memory>
#include "Definition.h"
#include "ErrorCode.h"
#include "Packet.h"
#include "PacketID.h"
#include "DataStruct.h"

using namespace NCommon;

class PacketMessenger;
class PacketProcessor;

namespace DataContainer
{
	class DataContainer
	{
	public :

		DataContainer() = default;
		~DataContainer() = default;

		// 초기화 & 해제 함수.
		void Init();
		void Release();

		// 네트워크 관련 함수.
		bool Connect();
		bool Disconnect();

		// 정보 관련 함수.
		

	private :

		// Queue 등록 함수.
		void RegisterQueueToProcessor();
		void RegisterQueueToMessenger();

		std::shared_ptr<PacketMessenger*> m_pPacketMessenger;
		std::shared_ptr<PacketProcessor*> m_pPacketProcessor;
		std::shared_ptr<LoginData*> m_pLoginData;

		std::deque<RecvPacketInfo*> m_PacketDeque;

	};

}