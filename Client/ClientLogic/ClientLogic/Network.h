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

/*
	리시브한 내용을 버퍼에 저장해야 함. (vector)
	시작하자마자 커넥트
	- 커넥트가 성공하면 쓰레드를 하나 만듬.
	- 쓰레드 생성후에는 리시브를 하고 있어야 함.

	Connect() 서버에 접속. std::thread 생성.
	threadFunc : 네트워크 연결되어있는지 확인 후, 연결 되었다면 while문 안에서 리시브.
				 데이터가 있다면 저장소에 넣음.

	Send() 소켓 Send
*/

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
		PACKET::RecvPacketInfo* GetPacketFromDeque()
		{
			auto returnPacket = m_PacketDeque.front();
			m_PacketDeque.pop_front();
			return returnPacket;
		};

	private :

		void Init();

		void Release();

		bool Connect();

		void Update();

		void PacketHeaderDivide(char* buf, const int size);

		void PushPacketToDeque(const PACKET::PacketHeader*, char*);

		void Send(char* buf, const int size);
		
		void Disconnect();

		void ErrDisplay(char * msg);
		void ErrQuit(char * msg);

		CONNECT_STATE  m_ConnectState = CONNECT_STATE::DISCONNECT;
		SOCKET m_ClientSock = INVALID_SOCKET;
		SOCKADDR_IN m_ServerAddr;
		char m_RecvBuffer[COMMON_INFO::recvSize] = { 0, };
		std::thread m_Thread;
		std::deque<PACKET::RecvPacketInfo*> m_PacketDeque;
		std::mutex m_Mutex;

	};
}