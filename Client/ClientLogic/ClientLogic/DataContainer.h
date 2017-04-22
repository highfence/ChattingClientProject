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


namespace DataContainer
{
class PacketMessenger;
class PacketProcessor;

	class DataContainer
	{
	public :

		DataContainer() = default;
		~DataContainer() = default;

		// �ʱ�ȭ & ���� �Լ�.
		void Init();
		void Release();

		// ��Ʈ��ũ ���� �Լ�.
		bool ConnectRequest();
		bool DisconnectRequest();

		// ���� ���� �Լ�.
		

	private :

		// Queue ��� �Լ�.
		void RegisterQueueToProcessor();

		std::unique_ptr<PacketMessenger> m_PacketMessenger;
		std::unique_ptr<PacketProcessor> m_PacketProcessor;
		std::shared_ptr<LoginData*> m_pLoginData;

		std::deque<RecvPacketInfo*> m_PacketDeque;

	};
}