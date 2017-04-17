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
	���ú��� ������ ���ۿ� �����ؾ� ��. (vector)
	�������ڸ��� Ŀ��Ʈ
	- Ŀ��Ʈ�� �����ϸ� �����带 �ϳ� ����.
	- ������ �����Ŀ��� ���ú긦 �ϰ� �־�� ��.

	Connect() ������ ����. std::thread ����.
	threadFunc : ��Ʈ��ũ ����Ǿ��ִ��� Ȯ�� ��, ���� �Ǿ��ٸ� while�� �ȿ��� ���ú�.
				 �����Ͱ� �ִٸ� ����ҿ� ����.

	Send() ���� Send
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