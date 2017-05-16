#include "Observer.h"
#include "LoginData.h"

namespace ClientLogic
{
	void LoginData::OnPacketReceive()
	{
		// 받은 큐가 비어있으면 일하지 않음.
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		// 큐가 있다면 빼서, LOGIN_IN_RES 패킷이 왔는지 확인.
		auto packet = m_RecvQueue.front();

		if (packet->PacketId != (short)PACKET_ID::LOGIN_IN_RES)
		{
			// 뭔가 잘못된 패킷이 옴.
			OutputDebugString(L"Invaild Packet Receive! (In LoginData Update)\n");
			return;
		}

		// 올바른 패킷이 왔다면, 에러코드 확인.
		auto i = (PktLogInRes*)packet->pData;
		if (i->ErrorCode != (short)ERROR_CODE::NONE)
		{
			// 로그인 실패
			OutputDebugString(L"[LoginData] 로그인 실패!\n");
		}
		else
		{
			// 성공
			OutputDebugString(L"[LoginData] 로그인 성공!\n");
			m_IsLoginSuccessed = true;
			VersionUp();
		}
		m_RecvQueue.pop_front();
	}

	void LoginData::SetSubscribe(PacketDistributer* publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOGIN_IN_RES, &m_RecvQueue);
	}
}