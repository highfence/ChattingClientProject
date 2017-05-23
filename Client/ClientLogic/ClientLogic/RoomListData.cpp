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
		// ���� ť�� ��������� ������ ����.
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		// ť�� ��Ŷ�� �ִٸ� ����, �����Ǵ� Function�� �ֳ� Ȯ��.
		auto packet = m_RecvQueue.front();

		auto packetProcessFunc = m_PacketFuncMap.find(packet->PacketId);

		if (packetProcessFunc == m_PacketFuncMap.end())
		{
			OutputDebugString(L"[RoomListData] �ʿ� ��ϵ��� ���� ��Ŷ ����. \n");
		}
		else
		{
			packetProcessFunc->second(packet);
		}

		m_RecvQueue.pop_front();
	}

	void RoomListData::RegisterPacketProcess()
	{
		/* �̰� ��� �ѹ��� ó���ϰ� �� ���� ������? */

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

		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_LEAVE_RES,
				[this](std::shared_ptr<RecvPacketInfo> packetInfo) { this->LobbyLeaveRes(packetInfo); }));

		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_LEAVE_USER_NTF,
				[this](std::shared_ptr<RecvPacketInfo> packetInfo) { this->LobbyLeaveUserNtf(packetInfo); }));
	}

	void RoomListData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_LIST_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_CHAT_NTF, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_LEAVE_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_LEAVE_USER_NTF, &m_RecvQueue);

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

	/* ä�� �����͸� ���� ��⿭�� �о�־��ִ� �Լ�. */
	void RoomListData::PushChatData(std::wstring id, std::wstring chatMsg)
	{
		std::shared_ptr<ChatData> newMsg = std::make_shared<ChatData>();
		newMsg->DataSetting(id, chatMsg);

		m_WaitResQueue.emplace(std::move(newMsg));
	}

	/* ����� ä�� ť���� �� ������ �̾��ִ� �Լ�. */
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
		OutputDebugString(L"[RoomListData] ���� ���� ���� ����\n");

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
		if (recvData->Count == 0) m_UserInfoList.clear();

		// ���Ϳ� ���� ������ �о�־� �ش�.
		PushUserInfoToVector(*recvData);

		// �� �޾ƾ� �� ���� �����Ͱ� �ִٸ�,
		if (!IsUserDataEnded(*recvData))
		{
			// ������ �޾Ҵ��� ����� ���´�.
			m_IsRequestNeeded = true;
			m_ReceivedLastestUserId = recvData->UserInfo->LobbyUserIndex;
		}
		// �� �޾ƾ� �� ���� �����Ͱ� ���ٸ�,
		else
		{
			// ������ �ʱ�ȭ ���ѳ��´�.
			m_IsRequestNeeded = false;
			m_ReceivedLastestUserId = 0;
		}

		// ���� ����.
		VersionUp();
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
}