#include "Observer.h"
#include "Util.h"

namespace ClientLogic
{
	void Observer::Subscribe(short packetId, PacketDistributer* processor)
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

	void LoginData::SetSubscribe(PacketDistributer* publisher)
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

		/* �κ� ����Ʈ �亯 */
		if (packet->PacketId == (short)PACKET_ID::LOBBY_LIST_RES)
		{
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
		}
		/* �κ� ���� ������ �亯 */
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_RES)
		{
			auto pLobbyEnterResData = (PktLobbyEnterRes*)packet->pData;
			if (pLobbyEnterResData->ErrorCode != (short)ERROR_CODE::NONE)
			{
				OutputDebugString(L"[LobbyListData] �κ� ���� ������ ���� ����!\n");
			}
			else
			{
				OutputDebugString(L"[LobbyListData] �κ� ���� ������ ���������� ����\n");
				m_IsLobbySuccesslyEntered = true;
			}
		}
		else
		{
			OutputDebugString(L"Invalid Packet Receive! (In LobbyListData Update)\n");
			return;
		}

		m_RecvQueue.pop_front();
	}
	

	void LobbyListData::SetSubscribe(PacketDistributer* publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_LIST_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_RES, &m_RecvQueue);
	}

	const LobbyListInfo * LobbyListData::GetLobbyListInfo(const int idx) const
	{
		const auto listLobbyConstPointer = &m_LobbyList[idx];
		return listLobbyConstPointer;
	}

	void LobbyListData::LoadData(PktLobbyListRes* pLobbyListData)
	{
		m_LobbyCount = pLobbyListData->LobbyCount;
		
		for (int i = 0; i < m_LobbyCount; ++i)
		{
			m_LobbyList[i] = pLobbyListData->LobbyList[i];
		}
	}

	void RoomListData::Update()
	{
		// ���� ť�� ��������� ������ ����.
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		// ť�� �ִٸ� ����, LOGIN_IN_RES ��Ŷ�� �Դ��� Ȯ��.
		auto packet = m_RecvQueue.front();

		if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_USER_NTF)
		{
			auto i = (PktLobbyNewUserInfoNtf*)packet->pData;
			OutputDebugString(L"[RoomListData] ���� ���� ���� ����\n");
			m_UserIdVector.push_back(i->UserID);
		}
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_CHAT_REQ)
		{
			OutputDebugString(L"[RoomListData] ä�� �亯 ���� ����\n");
		}
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_CHAT_NTF)
		{
			OutputDebugString(L"[RoomListData] �ٸ� ��� ä�� ���� ����\n");
		}

		m_RecvQueue.pop_front();
	}

	void RoomListData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_REQ, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_NTF, &m_RecvQueue);
	}
}