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

#include "Definition.h"
#include "ErrorCode.h"
#include "Packet.h"
#include "PacketID.h"

// TODO :: 스마트 포인터를 사용하자.
using namespace NCommon;

namespace ClientLogic
{
	enum class CONNECT_STATE
	{
		DISCONNECT, CONNECTING, CONNECTED
	};

	class ClientNetwork 
	{
	public :

		ClientNetwork() = default;
		~ClientNetwork() = default;

		CONNECT_STATE GetConnectState() { return m_ConnectState; };
		RecvPacketInfo* GetPacketFromDeque()
		{
			std::lock_guard<std::mutex> lockDeque(m_Mutex);
			if (m_PacketDeque.empty())
			{
				return nullptr;
			}
			auto returnPacket = m_PacketDeque.front();
			m_PacketDeque.pop_front();
			return returnPacket;
		};

		void Init();
		void Release();
		bool Connect();

		bool SendLogin(std::wstring, std::wstring);

	private :


		void Update();

		void PacketHeaderDivide(char* buf, const int size);

		void PushPacketToDeque(const PktHeader*, char*);

		void Send(char* buf, const int size);
		void Send(const short, const short, char*);
		
		void Disconnect();

		void ErrDisplay(char * msg);
		void ErrQuit(char * msg);

		CONNECT_STATE  m_ConnectState = CONNECT_STATE::DISCONNECT;
		SOCKET m_ClientSock = INVALID_SOCKET;
		SOCKADDR_IN m_ServerAddr;
		char m_RecvBuffer[COMMON_INFO::recvSize] = { 0, };
		std::thread m_Thread;
		std::deque<RecvPacketInfo*> m_PacketDeque;
		std::mutex m_Mutex;

	};

	class PacketProcess
	{
	public :
		PacketProcess() = default;
		~PacketProcess() = default;

		void Update();
		void RegisterClientNetwork(ClientNetwork*);
		bool GetIsLoginSuccessed() { return m_IsLoginSuccessed; }; 
		void SetIsLoginSuccedded(const bool isSuccessed) { m_IsLoginSuccessed = isSuccessed; };

	private :

		ClientNetwork* m_ClientNetwork;
		bool m_IsClientRegisterd = false;
		bool m_IsLoginSuccessed = false;

	};
}