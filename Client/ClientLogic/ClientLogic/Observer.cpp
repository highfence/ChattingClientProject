#include "Observer.h"

namespace ClientLogic
{
	void Observer::Subscribe(short packetId, std::shared_ptr<PacketDistributer> processor)
	{
		processor->Subscribe(packetId, &m_RecvQueue);
	}

	void LoginData::Update()
	{
		// ���� ť�� ��������� ������ ����.
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		// ť�� �ִٸ� ����, LOGIN_IN_RES ��Ŷ�� �Դ��� Ȯ��.
		auto packet = m_RecvQueue.front();

		if (packet->PacketId != (short)PACKET_ID::LOGIN_IN_RES)
		{
			// ���� �߸��� ��Ŷ�� ��.
			OutputDebugString(L"Invaild Packet Receive! (In LoginData Update)\n");
			return;
		}

		// �ùٸ� ��Ŷ�� �Դٸ�, �����ڵ� Ȯ��.
		auto i = (PktLogInRes*)packet->pData;
		if (i->ErrorCode != (short)ERROR_CODE::NONE)
		{
			// �α��� ����
			OutputDebugString(L"[LoginData] �α��� ����!\n");
		}
		else
		{
			// ����
			OutputDebugString(L"[LoginData] �α��� ����!\n");
			m_IsLoginSuccessed = true;
		}
		m_RecvQueue.pop_front();
	}

	void LoginData::SetSubscribe(std::shared_ptr<PacketDistributer> publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOGIN_IN_RES, &m_RecvQueue);
	}

	void LobbyListData::Update()
	{
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		auto packet = m_RecvQueue.front();

		if (packet->PacketId != (short)PACKET_ID::LOBBY_LIST_RES)
		{
			OutputDebugString(L"Invalid Packet Receive! (In LobbyListData Update)\n");
			return;
		}

		auto pLobbyListData = (PktLobbyListRes*)packet->pData;
		if (pLobbyListData->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LobbyListData] �κ񸮽�Ʈ ���� ����!\n");
		}
		else
		{
			OutputDebugString(L"[LobbyListData] �κ񸮽�Ʈ ���������� ����\n");
			LoadData(pLobbyListData);
		}
		m_RecvQueue.pop_front();
	}
	

	void LobbyListData::SetSubscribe(std::shared_ptr<PacketDistributer> publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_LIST_RES, &m_RecvQueue);
	}

	const LobbyListInfo * LobbyListData::GetLobbyListInfo() const
	{
		const LobbyListInfo* const listLobbyConstPointer = m_LobbyList;
		return listLobbyConstPointer;
	}

	void LobbyListData::LoadData(PktLobbyListRes* pLobbyListData)
	{
		m_LobbyCount = pLobbyListData->LobbyCount;
		
		for (int i = 0; i < m_LobbyCount; ++i)
		{
			m_LobbyList[i] = pLobbyListData->LobbyList[i];
		}
		m_IsListLoaded = true;
	}
}