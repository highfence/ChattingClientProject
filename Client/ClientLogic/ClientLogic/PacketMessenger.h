#pragma once
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

		void Init();
		void Release();
		bool Connect();
		bool Disconnect();
		bool Send(const short, const short, char*);

		const short GetConnectState() const { return (short)m_ConnectState; };
		std::shared_ptr<RecvPacketInfo> GetPacketFromDeque();


	private :

		bool SetSocket();
		void Update();

		void LoadUpPacketToDeque(char* buf, const int bufSize);

		CONNECT_STATE m_ConnectState = CONNECT_STATE::DISCONNECT;
		SOCKET m_ClientSock = INVALID_SOCKET;
		SOCKADDR_IN m_ServerAddr;
		std::thread m_RecvThread;
		std::deque<std::shared_ptr<RecvPacketInfo>> m_PacketDeque;
		std::mutex m_Mutex;
	};
}