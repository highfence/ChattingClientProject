#include <string>
#include <utility>
#include <queue>
#include <vector>
#include <stack>
#include <memory>
#include "Observer.h"
#include "Definition.h"
#include "Util.h"
#include "RoomListData.h"

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

		auto packetProcessFunc = m_PacketFuncMap.find(packet->PacketId);

		if (packetProcessFunc == m_PacketFuncMap.end())
		{
			OutputDebugString(L"[RoomListData] 맵에 등록되지 않은 패킷 받음. \n");
		}
		else
		{
			packetProcessFunc->second(packet);
		}

		m_RecvQueue.pop_front();
	}

	void RoomListData::RegisterPacketProcess()
	{
		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_ENTER_USER_NTF,
				[this](std::shared_ptr<RecvPacketInfo> packetInfo) { this->EnterUserNotify(packetInfo); }));

		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES,
				[this](std::shared_ptr<RecvPacketInfo> packetInfo) { this->EnterUserListRes(packetInfo); }));

		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_CHAT_RES,
				[this](std::shared_ptr<RecvPacketInfo> packetInfo) { this->LobbyChatRes(packetInfo); }));

		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_CHAT_NTF,
				[this](std::shared_ptr<RecvPacketInfo> packetInfo) { this->LobbyChatNtf(packetInfo); }));
	}

	void RoomListData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_NTF, &m_RecvQueue);
		RegisterPacketProcess();
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

	void RoomListData::PushChatData(std::wstring id, std::wstring chatMsg)
	{
		std::shared_ptr<ChatData> newMsg = std::make_shared<ChatData>();
		newMsg->DataSetting(id, chatMsg);

		m_WaitResQueue.emplace(std::move(newMsg));
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

	void RoomListData::EnterUserNotify(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomListData] 유저 정보 수령 성공\n");

		auto recvData = (PktLobbyNewUserInfoNtf*)packet->pData;

		auto userNumber = m_UserInfoVector.back().first + 1;
		std::string userId = recvData->UserID;
		makeUserData(userNumber, userId.c_str());

		VersionUp();
	}

	void RoomListData::EnterUserListRes(std::shared_ptr<RecvPacketInfo> packet)
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
			inputData.first = recvData->UserInfo[i].LobbyUserIndex;

			inputData.second = Util::CharToWstring(recvData->UserInfo[i].UserID);
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

	void RoomListData::LobbyChatRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktLobbyChatRes*)packet->pData;
		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomListData] 채팅 보내기 실패.\n");
		}
		else
		{
			OutputDebugString(L"[RoomListData] 채팅 보내기 성공\n");
			m_ChatStack.emplace(m_WaitResQueue.front());

			VersionUp();
		}

		m_WaitResQueue.pop();
	}

	void RoomListData::LobbyChatNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktLobbyChatNtf*)packet->pData;
		OutputDebugString(L"[RoomListData] 다른 사람 채팅 수령 성공\n");

		std::wstring userIdStr = Util::CharToWstring(recvData->UserID);;
		std::wstring userMsgStr(recvData->Msg);

		m_ChatStack.emplace(std::make_shared<ChatData>(userIdStr, userMsgStr));

		VersionUp();
	}
}