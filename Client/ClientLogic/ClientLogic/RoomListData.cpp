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
	// RoomListData가 관심있는 패킷과 대응되는 처리 함수를 등록하는 함수.
	void RoomListData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES,
			std::bind(&RoomListData::LobbyEnterUserListRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_ENTER_USER_NTF,
			std::bind(&RoomListData::LobbyEnterUserNtf, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_RES,
			std::bind(&RoomListData::LobbyEnterRoomListRes, this, std::placeholders::_1));
			
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

		publisher->Subscribe(
			(short)PACKET_ID::ROOM_ENTER_RES,
			std::bind(&RoomListData::RoomEnterRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::ROOM_CHANGED_INFO_NTF,
			std::bind(&RoomListData::RoomChangedInfoNtf, this, std::placeholders::_1));
	}

	// 채팅이 성공적으로 보내졌다는 패킷을 받았는지 반환해주는 함수.
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

	// 룸에 성공적으로 입장했다는 패킷을 받았는지 반환해주는 함수.
	bool RoomListData::GetIsRoomSuccesslyEntered()
	{
		if (m_IsRoomSuccesslyEntered)
		{
			m_IsRoomSuccesslyEntered = false;
			return true;
		}
		return false;
	}

	// 룸 정보 리퀘스트가 필요한지를 반환해주는 함수.
	bool RoomListData::GetIsRoomDataRequestNeeded()
	{
		if (m_IsRoomRequestNeeded)
		{
			m_IsRoomRequestNeeded = false;
			return true;
		}
		return false;
	}

	// 데이터 컨테이너에 받아놓은 룸 정보를 반환해주는 함수.
	bool RoomListData::GetRoomInfoFromQueue(
		short * pRoomIndex,
		short * pRoomUserCount,
		std::wstring * pRoomTitle)
	{
		// Out 데이터의 유효성 검사.
		if (pRoomIndex == nullptr || pRoomUserCount == nullptr || pRoomTitle == nullptr)
		{
			return false;
		}

		// 남아있는 RoomInfo가 없을 경우.
		if (m_RoomInfoQueue.empty())
		{
			return false;
		}

		// 큐에서 RoomInfo를 하나 빼준다.
		auto roomInfo = m_RoomInfoQueue.front();
		m_RoomInfoQueue.pop();

		*pRoomIndex = roomInfo->RoomIndex;
		*pRoomUserCount = roomInfo->RoomUserCount;
		*pRoomTitle = roomInfo->RoomTitle;

		return true;
	}

	// 데이터 컨테이너에 받아놓은 변경된 룸 정보를 반환해주는 함수.
	// 룸 정보가 있다면 true를, 없다면 false를 반환한다.
	bool RoomListData::GetChangedRoomInfoFromQueue(
		short * pRoomIndex,
		short * pRoomUserCount,
		std::wstring * pRoomTitle)
	{
		// Out 데이터의 유효성 검사.
		if (pRoomIndex == nullptr || pRoomUserCount == nullptr || pRoomTitle == nullptr)
		{
			return false;
		}

		// 남아있는 RoomInfo가 없을 경우.
		if (m_ChangedRoomInfoQueue.empty())
		{
			return false;
		}

		// 큐에서 변경된 RoomInfo를 하나 빼준다.
		auto changedRoomInfo = m_ChangedRoomInfoQueue.front();
		m_ChangedRoomInfoQueue.pop();

		*pRoomIndex = changedRoomInfo->RoomIndex;
		*pRoomUserCount = changedRoomInfo->RoomUserCount;
		*pRoomTitle = changedRoomInfo->RoomTitle;
		return true;
	}

	// 채팅 데이터를 응답 대기열에 밀어넣어주는 함수. 
	void RoomListData::PushChatData(std::wstring id, std::wstring chatMsg)
	{
		std::shared_ptr<ChatData> newMsg = std::make_shared<ChatData>();
		newMsg->DataSetting(id, chatMsg);

		m_WaitResQueue.emplace(std::move(newMsg));
	}

	// 저장된 채팅 큐에서 한 라인을 뽑아주는 함수. 
	std::wstring RoomListData::GetDataFromChatQueue()
	{
		if (m_ChatQueue.empty()) return L"";

		auto returnString = m_ChatQueue.front();
		m_ChatQueue.pop();
		return returnString->GetInLine();
	}

	//-------------------------------------------------------------------------
	// 패킷 처리 관련 함수들. 
	//-------------------------------------------------------------------------
	void RoomListData::LobbyEnterUserNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomListData] 유저 정보 수령 성공\n");

		auto recvData = (PktLobbyNewUserInfoNtf*)packet->pData;

		std::string userId = recvData->UserID;
		m_UserInfoList.emplace_back(std::move(Util::CharToWstring(userId.c_str())));

		VersionUp();
	}

	void RoomListData::LobbyEnterUserListRes(std::shared_ptr<RecvPacketInfo> packet)
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

				auto a = recvData.UserInfo[i].UserID;
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
		m_UserInfoList.clear();

		// 벡터에 유저 정보를 밀어넣어 준다.
		PushUserInfoToVector(*recvData);

		// 더 받아야 할 유저 데이터가 있다면,
		if (!IsUserDataEnded(*recvData))
		{
			// 어디까지 받았는지 기록해 놓는다.
			m_IsUserDataRequestNeeded = true;
			m_ReceivedLastestUserId = recvData->UserInfo->LobbyUserIndex;
		}
		// 더 받아야 할 유저 데이터가 없다면,
		else
		{
			// 변수를 초기화 시켜놓는다.
			m_IsUserDataRequestNeeded = false;
			m_ReceivedLastestUserId = 0;
		}

		// 버전 갱신.
		VersionUp();
	}

	void RoomListData::LobbyEnterRoomListRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktLobbyRoomListRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			auto a = recvData->ErrorCode;
			OutputDebugString(L"[RoomListData] 룸 목록을 제대로 받지 못함. \n");
			return;
		}
		OutputDebugString(L"[RoomListData] 룸 목록 성공적으로 수령. \n");

		// RoomQueue를 비워준다.
		std::queue<std::shared_ptr<RoomSmallInfo>> emptyQueue;
		std::swap(m_RoomInfoQueue, emptyQueue);
			
		// 성공적으로 수령했으므로 룸 정보 리스트에 룸 정보를 넣어준다.
		for (int i = 0; i < recvData->Count; ++i)
		{
			auto roomInfo = recvData->RoomInfo[i];
			auto sharedRoomInfo = std::make_shared<RoomSmallInfo>(roomInfo);
			m_RoomInfoQueue.emplace(std::move(sharedRoomInfo));
			++m_ReceivedRoomIndex;
		}

		// 추가적으로 룸 정보를 불러오는 것이 필요한지 기록해준다.
		if (recvData->IsEnd) m_IsRoomRequestNeeded = false;
		else m_IsRoomRequestNeeded = true;

		VersionUp();
		return;
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
			InitializeData();
			VersionUp();
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

	void RoomListData::RoomEnterRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomEnterRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomListData] 룸 진입 실패 \n");
			return;
		}
		else
		{
			OutputDebugString(L"[RoomListData] 룸 진입 성공. \n");
			m_IsRoomSuccesslyEntered = true;
			VersionUp();
		}
	}

	void RoomListData::RoomChangedInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktChangedRoomInfoNtf*)packet->pData;
	
		// 패킷을 받았다면, 그대로 큐에 밀어넣어준다.
		auto changedRoomInfo = std::make_shared<RoomSmallInfo>(recvData->RoomInfo);
		m_ChangedRoomInfoQueue.emplace(std::move(changedRoomInfo));
	}

	// RoomListData의 정보를 초기화해주는 함수.
	void RoomListData::InitializeData()
	{
		m_IsUserDataRequestNeeded = false;
		m_IsChatDelivered = false;
		m_IsRoomSuccesslyEntered = false;
		m_IsRoomRequestNeeded = false;
		m_ReceivedLastestUserId = 0;
		m_ReceivedRoomIndex = 0;
		m_UserInfoList.clear();

		std::queue<std::shared_ptr<ChatData>> emptyQueue1;
		std::swap(m_WaitResQueue, emptyQueue1);

		std::queue<std::shared_ptr<ChatData>> emptyQueue2;
		std::swap(m_ChatQueue, emptyQueue2);

		std::queue<std::shared_ptr<RoomSmallInfo>> emptyQueue3;
		std::swap(m_RoomInfoQueue, emptyQueue3);
	}
}