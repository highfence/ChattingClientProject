#pragma once
#include <winsock2.h>
#include <thread>
#include <deque>
#include <mutex>
#include "Packet.h"

using namespace NCommon;

namespace DataContainer
{
	enum class CONNECT_STATE : short
	{
		DISCONNECT, CONNECTING, CONNECTED
	};

	class PacketMessenger
	{
	public :

		PacketMessenger() = default;
		~PacketMessenger() = default;

		const short GetConnectState() const { return (short)m_ConnectState; };
		RecvPacketInfo* GetPacketFromDeque();

		void Init();
		void Release();
		bool Connect();
		bool Disconnect();

	private :

		void Update();

		void PacketHeaderDivide(char* buf, const int size);

		void PushPacketToDeque(const PktHeader*, char*);

		CONNECT_STATE m_ConnectState = CONNECT_STATE::DISCONNECT;
		SOCKET m_ClientSock = INVALID_SOCKET;
		SOCKADDR_IN m_ServerAddr;
		std::thread m_RecvThread;
		std::deque<RecvPacketInfo*> m_PacketDeque;
		std::mutex m_Mutex;
	};
}