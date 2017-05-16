#include <string>
#include "Observer.h"
#include "RoomListData.h"
#include "Definition.h"
#include "Util.h"

namespace ClientLogic
{
	void RoomListData::OnPacketReceive()
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
			std::string userId = packetData->UserID;
			makeUserData(userNumber, userId.c_str());

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
			auto recvData = (PktLobbyChatNtf*)packet->pData;
			OutputDebugString(L"[RoomListData] 다른 사람 채팅 수령 성공\n");

			//std::wstring userIdStr;
			//Util::CharToWstring(recvData->UserID, sizeof(recvData->UserID), userIdStr);

			//std::wstring userMsgStr(recvData->Msg);
			//std::wstring wholeMsg = userIdStr + L" : " + userMsgStr;
			//m_ChatQueue.emplace_back(std::move(wholeMsg));

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

				inputData.second = Util::CharToWstring(recvData->UserInfo->UserID);
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
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_NTF, &m_RecvQueue);
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

	std::wstring RoomListData::GetChatFromQueue()
	{
		if (m_ChatQueue.empty())
		{
			return L"";
		}

		auto returnString = m_ChatQueue.front();
		m_ChatQueue.pop_front();
		return returnString;
	}

	void RoomListData::makeUserData(const int userNumber, const char * userId)
	{
		std::pair<int, std::wstring> inputData;
		inputData.first = userNumber;
		inputData.second = Util::CharToWstring(userId);

		m_UserInfoVector.emplace_back(std::move(inputData));
	}

	void RoomListData::RequestUserList()
	{
		
	}
}