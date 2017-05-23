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

		auto packetProcess = m_PacketFuncMap.find(packet->PacketId);

		if (packetProcess == m_PacketFuncMap.end())
		{
			OutputDebugString(L"[LoginData] 구독하지 않은 패킷 전송 받음. \n");
		}
		else
		{
			packetProcess->second(packet);
		}
	
		m_RecvQueue.pop_front();
	}

	void LoginData::RegisterPacketProcess()
	{
		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOGIN_IN_RES,
				[this](std::shared_ptr<RecvPacketInfo> packet) { this->LoginInRes(packet); }));
	}

	void LoginData::SetSubscribe(PacketDistributer* publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOGIN_IN_RES, &m_RecvQueue);
		RegisterPacketProcess();
	}

	void LoginData::LoginInRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		// 에러코드 확인.
		auto i = (PktLogInRes*)packet->pData;
		if (i->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LoginData] 로그인 실패!\n");
		}
		else
		{
			OutputDebugString(L"[LoginData] 로그인 성공!\n");
			// 성공
			m_IsLoginSuccessed = true;
			VersionUp();
		}
	}
}