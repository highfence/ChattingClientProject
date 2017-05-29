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
		short GetReceivedLastestUserId() const { return m_ReceivedLastestUserId; };
		short GetReceivedLastestRoomId() const { return m_ReceivedRoomIndex; };
		bool GetRoomInfoFromQueue(
			short* pRoomIndex,
			short* pRoomUserCount,
			std::wstring* pRoomTitle);
		bool GetChangedRoomInfoFromQueue(
			short* pRoomIndex,
			short* pRoomUserCount,
			std::wstring* pRoomTitle);

		std::list<std::wstring>& GetRefUserInfoList() { return m_UserInfoList; };
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
		short m_ReceivedLastestUserId = 0;
		short m_ReceivedRoomIndex = 0;
		bool m_IsRoomRequestNeeded = false;
	};
}