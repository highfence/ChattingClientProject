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
			VersionUp();
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
				VersionUp();
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
				VersionUp();
			}
		}
		/* �ٸ� ����� �κ� ���� ������ */
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_USER_NTF)
		{
			OutputDebugString(L"[LobbyListData] LOBBY_ENTER_USER_NTF ��Ŷ ����\n");
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
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);
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
			OutputDebugString(L"[RoomListData] ���� ���� ���� ����\n");

			auto packetData = (PktLobbyNewUserInfoNtf*)packet->pData;
			auto userNumber = m_UserInfoVector.back().first + 1;
			auto userId = packetData->UserID;
			std::pair<int, std::wstring> inputData;
			inputData.first = userNumber;
			inputData.second = (LPCTSTR)userId;

			m_UserInfoVector.emplace_back(std::move(inputData));
			VersionUp();
		}
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_CHAT_RES)
		{
			OutputDebugString(L"[RoomListData] ä�� �亯 ���� ����\n");
			m_IsChatDelivered = true;
			VersionUp();
		}
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_CHAT_NTF)
		{
			OutputDebugString(L"[RoomListData] �ٸ� ��� ä�� ���� ����\n");
			VersionUp();
		}
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES)
		{
			auto recvData = (PktLobbyUserListRes*)packet->pData;
			if (recvData->ErrorCode != 0)
			{
				OutputDebugString(L"[RoomListData] �κ� ���� ����Ʈ ���� ����\n");
				return;
			}
			OutputDebugString(L"[RoomListData] �κ� ���� ���� ����Ʈ ���� ����\n");

			if (recvData->Count == 0)
			{
				// ��Ŷ���� ���� �������� ī��Ʈ�� 0�̸� ���͸� ����ְ� ó������ �ٽ� ����.
				m_UserInfoVector.clear();
			}

			for (int i = 0; i < recvData->Count; ++i)
			{
				std::pair<int, std::wstring> inputData;
				inputData.first = recvData->UserInfo->LobbyUserIndex;
				
				char test[300];
				memcpy(test, recvData->UserInfo->UserID, sizeof(recvData->UserInfo->UserID));

				inputData.second = (LPCTSTR)recvData->UserInfo->UserID;
				m_UserInfoVector.emplace_back(std::move(inputData));
			}

			if (recvData->IsEnd == false)
			{
				m_IsRequestNeeded = true;
				m_ReceivedLastestUserId = recvData->UserInfo->LobbyUserIndex;
			}
			else
			{
				m_IsRequestNeeded = false;
				m_ReceivedLastestUserId = 0;
			}

			VersionUp();
		}

		m_RecvQueue.pop_front();
	}

	void RoomListData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_NTF, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, &m_RecvQueue);
	}

	bool RoomListData::GetIsChatDelivered()
	{
		if (m_IsChatDelivered == false)
		{
			return false;
		}
		else
		{
			m_IsChatDelivered = false;
			return true;
		}
	}
}