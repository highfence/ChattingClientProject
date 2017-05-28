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
	void RoomListData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe(
			(short)PACKET_ID::ROOM_ENTER_RES,
			std::bind(&RoomListData::EnterUserListRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::ROOM_ENTER_USER_NTF,
			std::bind(&RoomListData::EnterUserNotify, this, std::placeholders::_1));
			
		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_CHAT_RES,
			std::bind(&RoomListData::LobbyChatRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_CHAT_NTF,
			std::bind(&RoomListData::LobbyChatNtf, this, std::placeholders::_1));
		
		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_LEAVE_RES,
			std::bind(&RoomListData::LobbyLeaveRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_LEAVE_USER_NTF,
			std::bind(&RoomListData::LobbyLeaveUserNtf, this, std::placeholders::_1));
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

	/* 채팅 데이터를 응답 대기열에 밀어넣어주는 함수. */
	void RoomListData::PushChatData(std::wstring id, std::wstring chatMsg)
	{
		std::shared_ptr<ChatData> newMsg = std::make_shared<ChatData>();
		newMsg->DataSetting(id, chatMsg);

		m_WaitResQueue.emplace(std::move(newMsg));
	}

	/* 저장된 채팅 큐에서 한 라인을 뽑아주는 함수. */
	std::wstring RoomListData::GetDataFromChatQueue()
	{
		if (m_ChatQueue.empty()) return L"";

		auto returnString = m_ChatQueue.front();
		m_ChatQueue.pop();
		return returnString->GetInLine();
	}

	void RoomListData::RequestUserList()
	{
		
	}

	void RoomListData::EnterUserNotify(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomListData] 유저 정보 수령 성공\n");

		auto recvData = (PktLobbyNewUserInfoNtf*)packet->pData;

		std::string userId = recvData->UserID;
		m_UserInfoList.emplace_back(std::move(Util::CharToWstring(userId.c_str())));

		VersionUp();
	}

	void RoomListData::EnterUserListRes(std::shared_ptr<RecvPacketInfo> packet)
	{
#pragma region Functions...

		auto CheckIsRecvDataValid = [](PktLobbyUserListRes& recvData)-> bool
		{
			if (recvData.ErrorCode != 0)
			{
				OutputDebugString(L"[RoomListData] 로비 유저 리스트 수령 실패\n");
				return false;
			}
			OutputDebugString(L"[RoomListData] 로비 안의 유저 리스트 수령 성공\n");
			return true;
		};

		auto PushUserInfoToVector = [this](PktLobbyUserListRes& recvData)
		{
			for (int i = 0; i < recvData.Count; ++i)
			{
				std::wstring inputData;

				inputData = Util::CharToWstring(recvData.UserInfo[i].UserID);
				m_UserInfoList.emplace_back(std::move(inputData));
			}
		};

		auto IsUserDataEnded = [](PktLobbyUserListRes& recvData)-> bool
		{
			return recvData.IsEnd;
		};

#pragma endregion

		auto recvData = (PktLobbyUserListRes*)packet->pData;

		if (!CheckIsRecvDataValid(*recvData)) return;

		// 패킷에서 보낸 데이터의 카운트가 0일 경우, 처음부터 UserList를 받는 경우이므로 벡터를 비워줌.
		if (recvData->Count == 0) m_UserInfoList.clear();

		// 벡터에 유저 정보를 밀어넣어 준다.
		PushUserInfoToVector(*recvData);

		// 더 받아야 할 유저 데이터가 있다면,
		if (!IsUserDataEnded(*recvData))
		{
			// 어디까지 받았는지 기록해 놓는다.
			m_IsRequestNeeded = true;
			m_ReceivedLastestUserId = recvData->UserInfo->LobbyUserIndex;
		}
		// 더 받아야 할 유저 데이터가 없다면,
		else
		{
			// 변수를 초기화 시켜놓는다.
			m_IsRequestNeeded = false;
			m_ReceivedLastestUserId = 0;
		}

		// 버전 갱신.
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
			m_ChatQueue.emplace(m_WaitResQueue.front());

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

		m_ChatQueue.emplace(std::make_shared<ChatData>(userIdStr, userMsgStr));

		VersionUp();
	}

	void RoomListData::LobbyLeaveRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktLobbyLeaveRes*)packet->pData;
		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomListData] 로비 떠나기 실패. \n");
		}
		else
		{
			OutputDebugString(L"[RoomListData] 로비 떠나기 성공. \n");
		}
	}

	void RoomListData::LobbyLeaveUserNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomListData] 유저 떠나는 정보 수령 성공. \n");

		auto recvData = (PktLobbyLeaveUserInfoNtf*)packet->pData;
		auto leaveUserId = Util::CharToWstring(recvData->UserID);

		// 아이디가 같은지 판별할 람다 함수식.
		auto IsIdEqual = [&leaveUserId](const std::wstring i)->bool 
		{
			return i == leaveUserId;
		};

		// 서버 조건이 아이디가 같은 경우를 허용하지 않으므로, 아이디가 같은 유저를 찾아 없앤다.
		m_UserInfoList.remove_if(IsIdEqual);

		VersionUp();
	}
}