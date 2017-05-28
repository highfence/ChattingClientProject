#pragma once
#include <queue>

namespace ClientLogic
{

	class RoomListData : public Observer
	{

	public:
		RoomListData() = default;
		~RoomListData() = default;

		void SetSubscribe(PacketDistributer* publisher);

		bool GetIsChatDelivered();
		bool GetIsRequestNeeded() const { return m_IsRequestNeeded; };
		int GetReceivedLastestUserId() const { return m_ReceivedLastestUserId; };
		// TODO :: ���� ���� �޴� ��� ���ľ���.
		std::list<std::wstring>& GetUserInfoList() { return m_UserInfoList; };
		void PushChatData(std::wstring id, std::wstring chatMsg);
		std::wstring GetDataFromChatQueue();

	private:
		void RequestUserList();

		void EnterUserNotify(std::shared_ptr<RecvPacketInfo> packet);
		void EnterUserListRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyChatRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyChatNtf(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyLeaveRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyLeaveUserNtf(std::shared_ptr<RecvPacketInfo> packet);

		std::list<std::wstring> m_UserInfoList;
		std::queue<std::shared_ptr<ChatData>> m_WaitResQueue;
		std::queue<std::shared_ptr<ChatData>> m_ChatQueue;

		bool m_IsRequestNeeded = false;
		bool m_IsChatDelivered = false;
		int m_ReceivedLastestUserId = 0;
	};
}