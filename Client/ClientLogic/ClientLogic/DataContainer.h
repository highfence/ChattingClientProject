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

		// �ʱ�ȭ & ���� �Լ�.
		void Init();
		void Release();

		// ��Ʈ��ũ ���� �Լ�.
		bool Connect();
		bool Disconnect();

		// ���� ���� �Լ�.
		

	private :

		PacketMessenger* m_pPacketMessenger;
		PacketProcessor* m_pPacketProcessor;

	};

}