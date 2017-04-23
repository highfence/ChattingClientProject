
#include "Network.h"
#include "Definition.h"
#include "Util.h"

namespace ClientLogic
{
	void ClientNetwork::Init()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			return;
		}

		m_ClientSock = socket(AF_INET, SOCK_STREAM, 0);
		if (m_ClientSock == INVALID_SOCKET)
		{
			ErrQuit("Connect()");
		}

		ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));

	}

	void ClientNetwork::Release()
	{
		WSACleanup();
	}


	bool ClientNetwork::Connect()
	{
		m_ConnectState = CONNECT_STATE::CONNECTING;
		int retval;

		m_ServerAddr.sin_family = AF_INET;
		m_ServerAddr.sin_port = htons(SERVER_INFO::serverPort);
		m_ServerAddr.sin_addr.s_addr = inet_addr(SERVER_INFO::loopBackIp.c_str());
		//m_ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

		retval = connect(m_ClientSock, (SOCKADDR*)&m_ServerAddr, sizeof(m_ServerAddr));
		if (retval == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			ErrDisplay("Receive Failed");
			m_ConnectState = CONNECT_STATE::DISCONNECT;
			return false;
		}

		m_ConnectState = CONNECT_STATE::CONNECTED;

		// 쓰레드 생성.
		m_Thread = std::thread([this]() { Update(); });
		return true;
	}

	void ClientNetwork::Update()
	{
		int len = 0;
		int retval = 0;
		char buf[COMMON_INFO::recvSize + 1];

		// 네트워크 상태가 CONNECTED일 때만 루프 순환.
		while (m_ConnectState == CONNECT_STATE::CONNECTED)
		{
			retval = recv(m_ClientSock, buf, COMMON_INFO::recvSize, 0);
			if (retval == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				ErrDisplay("Receive Failed");
				break;
			}
			else if (retval == 0)
			{
				Disconnect();
			}

			PacketHeaderDivide(buf, retval);
		}
	}

	void ClientNetwork::PacketHeaderDivide(char * buf, const int size)
	{
		int readPos = 0;
		int dataSize = size;
		PktHeader* pPacketHeader;

		while ((dataSize - readPos) > PACKET_HEADER_SIZE)
		{
			pPacketHeader = (PktHeader*)&buf[readPos];

			readPos += PACKET_HEADER_SIZE;

			// 디큐에 패킷 추가.
			PushPacketToDeque(pPacketHeader, &buf[readPos]);

			readPos += pPacketHeader->BodySize;
		}
	}

	void ClientNetwork::PushPacketToDeque(const PktHeader * pkt, char * pDataPos)
	{
		RecvPacketInfo* packetInfo = new RecvPacketInfo;
		packetInfo->PacketId = pkt->Id;
		packetInfo->PacketBodySize = pkt->BodySize;
		packetInfo->pData = new char[pkt->BodySize];
		memcpy(&packetInfo->pData, pDataPos, pkt->BodySize);

		// 메인쓰레드에서 패킷 디큐를 사용하기 때문에 락을 걸어준다.
		std::lock_guard<std::mutex> lockDeque(m_Mutex);
		m_PacketDeque.push_back(packetInfo);
	}

	bool ClientNetwork::SendLogin(std::wstring idStr, std::wstring pwStr)
	{
		PktLogInReq newLoginReq;

		/* 변환 과정. */
		char szID[MAX_USER_ID_SIZE] = { 0, };
		char szPW[MAX_USER_PASSWORD_SIZE] = { 0, };

		//Util::UnicodeToAnsi(idStr.c_str(), MAX_USER_ID_SIZE, szID);
		//Util::UnicodeToAnsi(pwStr.c_str(), MAX_USER_PASSWORD_SIZE, szPW);

		strncpy_s(newLoginReq.szID, MAX_USER_ID_SIZE + 1, szID, MAX_USER_ID_SIZE);
		strncpy_s(newLoginReq.szPW, MAX_USER_PASSWORD_SIZE + 1, szPW, MAX_USER_PASSWORD_SIZE);

		Send((short)PACKET_ID::LOGIN_IN_REQ, sizeof(newLoginReq), (char*)&newLoginReq);
		return true;
	}

	void ClientNetwork::Send(const short packetId, const short dataSize, char* pData)
	{
		char data[COMMON_INFO::MAX_PACKET_SIZE] = { 0, };

		PktHeader pktHeader{ packetId, dataSize };
		memcpy(&data[0], (char*)&pktHeader, PACKET_HEADER_SIZE);

		if (dataSize > 0)
		{
			memcpy(&data[PACKET_HEADER_SIZE], pData, dataSize);
		}

		int hr = send(m_ClientSock, data, dataSize + PACKET_HEADER_SIZE, 0);
		if (hr == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			ErrDisplay("Send Failed");
		}
	}

	void ClientNetwork::Disconnect()
	{
		if (m_ConnectState == CONNECT_STATE::CONNECTED)
		{
			m_Thread.join();

			closesocket(m_ClientSock);

			m_PacketDeque.clear();
		}
		Release();
	}

	void ClientNetwork::ErrDisplay(char * msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL
		);
		printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
		LocalFree(lpMsgBuf);
		return;
	}

	void ClientNetwork::ErrQuit(char * msg)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf, 0, NULL
		);

		MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
		LocalFree(lpMsgBuf);
		exit(-1);
	}

}