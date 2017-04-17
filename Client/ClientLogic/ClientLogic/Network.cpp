
#include "Network.h"
#include "Definition.h"

void ClientLogic::ClientNetwork::Init()
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

	// 커넥트
	if (!Connect())
	{
		ErrDisplay("커넥트 실패");
	}
}

void ClientLogic::ClientNetwork::Release()
{
	WSACleanup();
}


bool ClientLogic::ClientNetwork::Connect()
{
	m_ConnectState = CONNECT_STATE::CONNECTING;
	int retval;

	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_port = SERVER_INFO::serverPort;
	m_ServerAddr.sin_addr.s_addr = inet_addr(SERVER_INFO::loopBackIp.c_str());

	retval = connect(m_ClientSock, (SOCKADDR*)&m_ServerAddr, sizeof(m_ServerAddr));
	if (retval == SOCKET_ERROR)
	{
		m_ConnectState = CONNECT_STATE::DISCONNECT;
		return false;
	}

	m_ConnectState = CONNECT_STATE::CONNECTED;

	// 쓰레드 생성.
	m_Thread = std::thread([this]() { Update(); });
	return true;
}

void ClientLogic::ClientNetwork::Update()
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

void ClientLogic::ClientNetwork::PacketHeaderDivide(char * buf, const int size)
{
	using namespace PACKET;

	int readPos = 0;
	int dataSize = size;
	PacketHeader* pPacketHeader;

	while ((dataSize - readPos) > packetHeaderSize)
	{
		pPacketHeader = (PacketHeader*)&buf[readPos];

		readPos += packetHeaderSize;

		// 디큐에 패킷 추가.
		PushPacketToDeque(pPacketHeader, &buf[readPos]);

		readPos += pPacketHeader->BodySize;
	}
}

void ClientLogic::ClientNetwork::PushPacketToDeque(const PACKET::PacketHeader * pkt, char * pDataPos)
{
	using namespace PACKET;

	RecvPacketInfo* packetInfo = new RecvPacketInfo;
	packetInfo->PacketId = pkt->Id;
	packetInfo->PacketBodySize = pkt->BodySize;
	packetInfo->pData = new char[pkt->BodySize];
	memcpy(&packetInfo->pData, pDataPos, pkt->BodySize);

	// 메인쓰레드에서 패킷 디큐를 사용하기 때문에 락을 걸어준다.
	std::lock_guard<std::mutex> lockDeque(m_Mutex);
	m_PacketDeque.push_back(packetInfo);
}

void ClientLogic::ClientNetwork::Send(char* buf, const int size)
{
	int retval = 0;


}


void ClientLogic::ClientNetwork::Disconnect()
{
	if (m_ConnectState == CONNECT_STATE::CONNECTED)
	{
		m_Thread.join();

		closesocket(m_ClientSock);

		m_PacketDeque.clear();
	}
	Release();
}

void ClientLogic::ClientNetwork::ErrDisplay(char * msg)
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

void ClientLogic::ClientNetwork::ErrQuit(char * msg)
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


