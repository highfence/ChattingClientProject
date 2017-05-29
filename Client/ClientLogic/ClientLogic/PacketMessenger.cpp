#pragma comment(lib, "Ws2_32.lib")
#include "PacketMessenger.h"
#include "Definition.h"
#include <string>

namespace ClientLogic
{
	PacketMessenger::~PacketMessenger()
	{
		Release();
	}

	void PacketMessenger::Init()
	{
		if (!SetSocket())
		{
			OutputDebugString(L"Socket Setting Failed");
			return;
		}
	}

	void PacketMessenger::Release()
	{
		/* WSA Release */
		m_ConnectState = CONNECT_STATE::DISCONNECT;
		WSACleanup();
		closesocket(m_ClientSock);
		m_RecvThread.join();
	}

	std::shared_ptr<RecvPacketInfo> PacketMessenger::GetPacketFromDeque()
	{
		/* Make a lockguard */
		std::lock_guard<std::mutex> lockDeque(m_Mutex);

		/* If there is no message in queue, return nullptr */
		if (m_PacketDeque.empty()) return nullptr;

		/* Return front packet of the queue */
		auto frontPacket = m_PacketDeque.front();
		m_PacketDeque.pop_front();

		std::wstring debugLabel = L"[PacketMessenger] ��Ŷ POP : " + std::to_wstring(frontPacket->PacketId) + L"�� ��Ŷ (size : " + std::to_wstring(m_PacketDeque.size()) + L") \n";
		OutputDebugString(debugLabel.c_str());
		return frontPacket;
	}

	bool PacketMessenger::SetSocket()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			/* WSADATA make failed */
			return false;
		}

		m_ClientSock = socket(AF_INET, SOCK_STREAM, 0);
		if (m_ClientSock == INVALID_SOCKET)
		{
			/* Client Socket make failed */
			return false;
		}

		ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
		return true;
	}

	bool PacketMessenger::Connect()
	{
		m_ConnectState = CONNECT_STATE::CONNECTING;
		int retval = S_OK;

		m_ServerAddr.sin_family = AF_INET;
		m_ServerAddr.sin_port = htons(SERVER_INFO::serverPort);
		m_ServerAddr.sin_addr.s_addr = inet_addr(SERVER_INFO::loopBackIp.c_str());

		retval = connect(m_ClientSock, (SOCKADDR*)&m_ServerAddr, sizeof(m_ServerAddr));
		if (retval == SOCKET_ERROR)
		{
			/* Connect Failed */
			int err = WSAGetLastError();
			m_ConnectState = CONNECT_STATE::DISCONNECT;
			return false;
		}

		m_ConnectState = CONNECT_STATE::CONNECTED;

		/* Make Thread */
		m_RecvThread = std::thread([this]() { Update(); });
		return true;
	}

	void PacketMessenger::Update()
	{
		int len = 0;
		int retval = 0;
		char buf[COMMON_INFO::recvSize + 1];

		/* ��Ʈ��ũ ���°� CONNECTED�� ���� ���� ��ȯ */
		while (m_ConnectState == CONNECT_STATE::CONNECTED)
		{
			retval = recv(m_ClientSock, buf, COMMON_INFO::recvSize, 0);
			if (retval == SOCKET_ERROR)
			{
				/* Recv Error */
				int err = WSAGetLastError();
				break;
			}
			else if (retval == 0)
			{
				Disconnect();
			}
			LoadUpPacketToDeque(buf, retval);
		}
	}

	void PacketMessenger::LoadUpPacketToDeque(char* buf, const int bufSize)
	{
		int readPos = 0;

		/* ��� ������� �۰� ���Ҵٸ� �ǹ� ���� �����Ͷ�� �����ϰ� ���� ����. */
		while ((bufSize - readPos) > PACKET_HEADER_SIZE)
		{
			/* ��� �и� �۾� */
			PktHeader* pPacketHeader = (PktHeader*)&buf[readPos];

			readPos += PACKET_HEADER_SIZE;

			auto pPacketInfo = new RecvPacketInfo;
			pPacketInfo->PacketId = pPacketHeader->Id;
			pPacketInfo->PacketBodySize = pPacketHeader->BodySize;
			pPacketInfo->pData = new char[pPacketHeader->BodySize];
			memcpy(pPacketInfo->pData, &buf[readPos], pPacketHeader->BodySize);

			readPos += pPacketHeader->BodySize;

			std::wstring debugLabel = L"[PacketMessenger] ���� ��Ŷ ID : " + std::to_wstring(pPacketHeader->Id) + L"\n";
			OutputDebugString(debugLabel.c_str());

			// ������ ��.
			std::lock_guard<std::mutex> lockDeque(m_Mutex);
			m_PacketDeque.emplace_back(std::move(pPacketInfo));
		}
	}

	bool PacketMessenger::Disconnect()
	{
		if (m_ConnectState != CONNECT_STATE::DISCONNECT)
		{
			m_RecvThread.join();
			m_PacketDeque.clear();
			m_ConnectState = CONNECT_STATE::DISCONNECT;
		}
		return true;
	}

	/* ��Ŷ �޽����� Send�� ��û�� �� pData�� �˸��� ���·� �־��־�� �Ѵ�. */
	bool PacketMessenger::Send(const short packetId, const short packetSize, char* pData)
	{
		char data[COMMON_INFO::MAX_PACKET_SIZE] = { 0, };

		PktHeader pktHeader{ packetId, packetSize };
		memcpy(&data[0], (char*)&pktHeader, PACKET_HEADER_SIZE);

		if (packetSize > 0)
		{
			memcpy(&data[PACKET_HEADER_SIZE], pData, packetSize);
		}

		send(m_ClientSock, data, packetSize + PACKET_HEADER_SIZE, 0);

		return true;
	}
}