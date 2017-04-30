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

	void LobbyListData::Update()
	{
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		auto packet = m_RecvQueue.front();

		/* 로비 리스트 답변 */
		if (packet->PacketId == (short)PACKET_ID::LOBBY_LIST_RES)
		{
			auto pLobbyListData = (PktLobbyListRes*)packet->pData;
			if (pLobbyListData->ErrorCode != (short)ERROR_CODE::NONE)
			{
				OutputDebugString(L"[LobbyListData] 로비리스트 수령 실패!\n");
			}
			else
			{
				OutputDebugString(L"[LobbyListData] 로비리스트 성공적으로 수령\n");
				LoadData(pLobbyListData);
				VersionUp();
			}
		}
		/* 로비 진입 데이터 답변 */
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_RES)
		{
			auto pLobbyEnterResData = (PktLobbyEnterRes*)packet->pData;
			if (pLobbyEnterResData->ErrorCode != (short)ERROR_CODE::NONE)
			{
				OutputDebugString(L"[LobbyListData] 로비 진입 데이터 수령 실패!\n");
			}
			else
			{
				OutputDebugString(L"[LobbyListData] 로비 진입 데이터 성공적으로 수령\n");
				m_IsLobbySuccesslyEntered = true;
				VersionUp();
			}
		}
		/* 다른 사람의 로비 진입 데이터 */
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_USER_NTF)
		{
			OutputDebugString(L"[LobbyListData] LOBBY_ENTER_USER_NTF 패킷 수령\n");
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
		// 받은 큐가 비어있으면 일하지 않음.
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		// 큐가 있다면 빼서, LOGIN_IN_RES 패킷이 왔는지 확인.
		auto packet = m_RecvQueue.front();

		if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_USER_NTF)
		{
			OutputDebugString(L"[RoomListData] 유저 정보 수령 성공\n");

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
			OutputDebugString(L"[RoomListData] 채팅 답변 수령 성공\n");
			m_IsChatDelivered = true;
			VersionUp();
		}
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_CHAT_NTF)
		{
			OutputDebugString(L"[RoomListData] 다른 사람 채팅 수령 성공\n");
			VersionUp();
		}
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES)
		{
			auto recvData = (PktLobbyUserListRes*)packet->pData;
			if (recvData->ErrorCode != 0)
			{
				OutputDebugString(L"[RoomListData] 로비 유저 리스트 수령 실패\n");
				return;
			}
			OutputDebugString(L"[RoomListData] 로비 안의 유저 리스트 수령 성공\n");

			if (recvData->Count == 0)
			{
				// 패킷에서 받은 데이터의 카운트가 0이면 벡터를 비워주고 처음부터 다시 받음.
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