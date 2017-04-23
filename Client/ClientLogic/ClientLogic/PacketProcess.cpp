#include "network.h"
#include "ErrorCode.h"
#include "Packet.h"
#include "PacketID.h"
using namespace NCommon;

namespace ClientLogic
{
	void PacketProcess::Update()
	{
		/* 클라이언트가 등록이 안되어있으면 Update 실시하지 않음. */
		if (m_IsClientRegisterd == false)
		{
			return;
		}

		auto packet = m_ClientNetwork->GetPacketFromDeque();

		/* 큐에 패킷이 없으면 Update 실시하지 않음. */
		if (packet == nullptr)
		{
			return;
		}

		/* 패킷 아이디 분석하여 해당 사항 실행. */
		switch (packet->PacketId)
		{
		case (short)PACKET_ID::LOGIN_IN_RES :

			auto i = (PktLogInRes*)packet->pData;

			if (i->ErrorCode != (short)ERROR_CODE::NONE)
			{
				// ERROR
				OutputDebugString(L"[PacketProcess] 로그인 실패!");
			}
			else
			{
				// 성공
				OutputDebugString(L"[PacketProcess] 로그인 성공");
				m_IsLoginSuccessed = true;
			}
			delete i;
			
			break;
		}

	}

	void PacketProcess::RegisterClientNetwork(ClientNetwork* clientNetwork)
	{
		if (m_IsClientRegisterd == false)
		{
			m_ClientNetwork = clientNetwork;
			m_IsClientRegisterd = true;
		}
		return;
	}

}