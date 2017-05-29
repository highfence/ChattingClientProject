#pragma once
#include <queue>
#include <string>

namespace ClientLogic
{
	class RoomListData : public Observer
	{
	public:
		RoomListData() = default;
		~RoomListData() = default;

		void SetSubscribe(PacketDistributer* publisher);

		bool GetIsChatDelivered();
		bool GetIsRoomSuccesslyEntered();
		bool GetIsRoomDataRequestNeeded();
		bool GetIsUserDataRequestNeeded() const { return m_IsUserDataRequestNeeded; };
		int GetReceivedLastestUserId() const { return m_ReceivedLastestUserId; };
		int GetReceivedLastestRoomId() const { return m_ReceivedRoomIndex; };
		bool GetRoomInfoFromQueue(
			short* pRoomIndex,
			short* pRoomUserCount,
			std::wstring* pRoomTitle);
		bool GetChangedRoomInfoFromQueue(
			short* pRoomIndex,
			short* pRoomUserCount,
			std::wstring* pRoomTitle);
		// TODO :: 유저 인포 받는 방식 고쳐야함. list ref를 넘겨주는 건 너무 자유도가 높음.
		std::list<std::wstring>& GetUserInfoList() { return m_UserInfoList; };
		void PushChatData(std::wstring id, std::wstring chatMsg);
		std::wstring GetDataFromChatQueue();

	private:
		void LobbyEnterUserNtf(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyEnterUserListRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyEnterRoomListRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyChatRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyChatNtf(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyLeaveRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyLeaveUserNtf(std::shared_ptr<RecvPacketInfo> packet);
		void RoomEnterRes(std::shared_ptr<RecvPacketInfo> packet);
		void RoomChangedInfoNtf(std::shared_ptr<RecvPacketInfo> packet);

		std::list<std::wstring> m_UserInfoList;
		std::queue<std::shared_ptr<ChatData>> m_WaitResQueue;
		std::queue<std::shared_ptr<ChatData>> m_ChatQueue;
		std::queue<std::shared_ptr<RoomSmallInfo>> m_RoomInfoQueue;
		std::queue<std::shared_ptr<RoomSmallInfo>> m_ChangedRoomInfoQueue;

		void InitializeData();
		bool m_IsUserDataRequestNeeded = false;
		bool m_IsChatDelivered = false;
		bool m_IsRoomSuccesslyEntered = false;
		int m_ReceivedLastestUserId = 0;
		int m_ReceivedRoomIndex = 0;
		bool m_IsRoomRequestNeeded = false;
	};
}