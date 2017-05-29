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
	// RoomListData�� �����ִ� ��Ŷ�� �����Ǵ� ó�� �Լ��� ����ϴ� �Լ�.
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

	// ä���� ���������� �������ٴ� ��Ŷ�� �޾Ҵ��� ��ȯ���ִ� �Լ�.
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

	// �뿡 ���������� �����ߴٴ� ��Ŷ�� �޾Ҵ��� ��ȯ���ִ� �Լ�.
	bool RoomListData::GetIsRoomSuccesslyEntered()
	{
		if (m_IsRoomSuccesslyEntered)
		{
			m_IsRoomSuccesslyEntered = false;
			return true;
		}
		return false;
	}

	// �� ���� ������Ʈ�� �ʿ������� ��ȯ���ִ� �Լ�.
	bool RoomListData::GetIsRoomDataRequestNeeded()
	{
		if (m_IsRoomRequestNeeded)
		{
			m_IsRoomRequestNeeded = false;
			return true;
		}
		return false;
	}

	// ������ �����̳ʿ� �޾Ƴ��� �� ������ ��ȯ���ִ� �Լ�.
	bool RoomListData::GetRoomInfoFromQueue(
		short * pRoomIndex,
		short * pRoomUserCount,
		std::wstring * pRoomTitle)
	{
		// Out �������� ��ȿ�� �˻�.
		if (pRoomIndex == nullptr || pRoomUserCount == nullptr || pRoomTitle == nullptr)
		{
			return false;
		}

		// �����ִ� RoomInfo�� ���� ���.
		if (m_RoomInfoQueue.empty())
		{
			return false;
		}

		// ť���� RoomInfo�� �ϳ� ���ش�.
		auto roomInfo = m_RoomInfoQueue.front();
		m_RoomInfoQueue.pop();

		*pRoomIndex = roomInfo->RoomIndex;
		*pRoomUserCount = roomInfo->RoomUserCount;
		*pRoomTitle = roomInfo->RoomTitle;

		return true;
	}

	// ������ �����̳ʿ� �޾Ƴ��� ����� �� ������ ��ȯ���ִ� �Լ�.
	// �� ������ �ִٸ� true��, ���ٸ� false�� ��ȯ�Ѵ�.
	bool RoomListData::GetChangedRoomInfoFromQueue(
		short * pRoomIndex,
		short * pRoomUserCount,
		std::wstring * pRoomTitle)
	{
		// Out �������� ��ȿ�� �˻�.
		if (pRoomIndex == nullptr || pRoomUserCount == nullptr || pRoomTitle == nullptr)
		{
			return false;
		}

		// �����ִ� RoomInfo�� ���� ���.
		if (m_ChangedRoomInfoQueue.empty())
		{
			return false;
		}

		// ť���� ����� RoomInfo�� �ϳ� ���ش�.
		auto changedRoomInfo = m_ChangedRoomInfoQueue.front();
		m_ChangedRoomInfoQueue.pop();

		*pRoomIndex = changedRoomInfo->RoomIndex;
		*pRoomUserCount = changedRoomInfo->RoomUserCount;
		*pRoomTitle = changedRoomInfo->RoomTitle;
		return true;
	}

	// ä�� �����͸� ���� ��⿭�� �о�־��ִ� �Լ�. 
	void RoomListData::PushChatData(std::wstring id, std::wstring chatMsg)
	{
		std::shared_ptr<ChatData> newMsg = std::make_shared<ChatData>();
		newMsg->DataSetting(id, chatMsg);

		m_WaitResQueue.emplace(std::move(newMsg));
	}

	// ����� ä�� ť���� �� ������ �̾��ִ� �Լ�. 
	std::wstring RoomListData::GetDataFromChatQueue()
	{
		if (m_ChatQueue.empty()) return L"";

		auto returnString = m_ChatQueue.front();
		m_ChatQueue.pop();
		return returnString->GetInLine();
	}

	//-------------------------------------------------------------------------
	// ��Ŷ ó�� ���� �Լ���. 
	//-------------------------------------------------------------------------
	void RoomListData::LobbyEnterUserNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomListData] ���� ���� ���� ����\n");

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
				OutputDebugString(L"[RoomListData] �κ� ���� ����Ʈ ���� ����\n");
				return false;
			}
			OutputDebugString(L"[RoomListData] �κ� ���� ���� ����Ʈ ���� ����\n");
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

		// ��Ŷ���� ���� �������� ī��Ʈ�� 0�� ���, ó������ UserList�� �޴� ����̹Ƿ� ���͸� �����.
		m_UserInfoList.clear();

		// ���Ϳ� ���� ������ �о�־� �ش�.
		PushUserInfoToVector(*recvData);

		// �� �޾ƾ� �� ���� �����Ͱ� �ִٸ�,
		if (!IsUserDataEnded(*recvData))
		{
			// ������ �޾Ҵ��� ����� ���´�.
			m_IsUserDataRequestNeeded = true;
			m_ReceivedLastestUserId = recvData->UserInfo->LobbyUserIndex;
		}
		// �� �޾ƾ� �� ���� �����Ͱ� ���ٸ�,
		else
		{
			// ������ �ʱ�ȭ ���ѳ��´�.
			m_IsUserDataRequestNeeded = false;
			m_ReceivedLastestUserId = 0;
		}

		// ���� ����.
		VersionUp();
	}

	void RoomListData::LobbyEnterRoomListRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktLobbyRoomListRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			auto a = recvData->ErrorCode;
			OutputDebugString(L"[RoomListData] �� ����� ����� ���� ����. \n");
			return;
		}
		OutputDebugString(L"[RoomListData] �� ��� ���������� ����. \n");

		// RoomQueue�� ����ش�.
		std::queue<std::shared_ptr<RoomSmallInfo>> emptyQueue;
		std::swap(m_RoomInfoQueue, emptyQueue);
			
		// ���������� ���������Ƿ� �� ���� ����Ʈ�� �� ������ �־��ش�.
		for (int i = 0; i < recvData->Count; ++i)
		{
			auto roomInfo = recvData->RoomInfo[i];
			auto sharedRoomInfo = std::make_shared<RoomSmallInfo>(roomInfo);
			m_RoomInfoQueue.emplace(std::move(sharedRoomInfo));
			++m_ReceivedRoomIndex;
		}

		// �߰������� �� ������ �ҷ����� ���� �ʿ����� ������ش�.
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
			OutputDebugString(L"[RoomListData] ä�� ������ ����.\n");
		}
		else
		{
			OutputDebugString(L"[RoomListData] ä�� ������ ����\n");
			m_ChatQueue.emplace(m_WaitResQueue.front());
			VersionUp();
		}

		m_WaitResQueue.pop();
	}

	void RoomListData::LobbyChatNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktLobbyChatNtf*)packet->pData;
		OutputDebugString(L"[RoomListData] �ٸ� ��� ä�� ���� ����\n");

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
			OutputDebugString(L"[RoomListData] �κ� ������ ����. \n");
		}
		else
		{
			OutputDebugString(L"[RoomListData] �κ� ������ ����. \n");
			InitializeData();
			VersionUp();
		}
	}

	void RoomListData::LobbyLeaveUserNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomListData] ���� ������ ���� ���� ����. \n");

		auto recvData = (PktLobbyLeaveUserInfoNtf*)packet->pData;
		auto leaveUserId = Util::CharToWstring(recvData->UserID);

		// ���̵� ������ �Ǻ��� ���� �Լ���.
		auto IsIdEqual = [&leaveUserId](const std::wstring i)->bool 
		{
			return i == leaveUserId;
		};

		// ���� ������ ���̵� ���� ��츦 ������� �����Ƿ�, ���̵� ���� ������ ã�� ���ش�.
		m_UserInfoList.remove_if(IsIdEqual);

		VersionUp();
	}

	void RoomListData::RoomEnterRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomEnterRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomListData] �� ���� ���� \n");
			return;
		}
		else
		{
			OutputDebugString(L"[RoomListData] �� ���� ����. \n");
			m_IsRoomSuccesslyEntered = true;
			VersionUp();
		}
	}

	void RoomListData::RoomChangedInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktChangedRoomInfoNtf*)packet->pData;
	
		// ��Ŷ�� �޾Ҵٸ�, �״�� ť�� �о�־��ش�.
		auto changedRoomInfo = std::make_shared<RoomSmallInfo>(recvData->RoomInfo);
		m_ChangedRoomInfoQueue.emplace(std::move(changedRoomInfo));
	}

	// RoomListData�� ������ �ʱ�ȭ���ִ� �Լ�.
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