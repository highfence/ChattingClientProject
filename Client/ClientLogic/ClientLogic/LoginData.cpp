#include "Observer.h"
#include "LoginData.h"

namespace ClientLogic
{
	void LoginData::OnPacketReceive()
	{
		// ���� ť�� ��������� ������ ����.
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		// ť�� �ִٸ� ����, LOGIN_IN_RES ��Ŷ�� �Դ��� Ȯ��.
		auto packet = m_RecvQueue.front();

		auto packetProcess = m_PacketFuncMap.find(packet->PacketId);

		if (packetProcess == m_PacketFuncMap.end())
		{
			OutputDebugString(L"[LoginData] �������� ���� ��Ŷ ���� ����. \n");
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
		// �����ڵ� Ȯ��.
		auto i = (PktLogInRes*)packet->pData;
		if (i->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LoginData] �α��� ����!\n");
		}
		else
		{
			OutputDebugString(L"[LoginData] �α��� ����!\n");
			// ����
			m_IsLoginSuccessed = true;
			VersionUp();
		}
	}
}