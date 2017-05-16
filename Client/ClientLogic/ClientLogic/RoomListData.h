#pragma once

namespace ClientLogic
{

	class RoomListData : public Observer
	{
	public:
		RoomListData() = default;
		~RoomListData() = default;

		void OnPacketReceive() override;
		void SetSubscribe(PacketDistributer*);

		bool GetIsRequestNeeded() const { return m_IsRequestNeeded; };
		bool GetIsChatDelivered();
		int GetReceivedLastestUserId() const { return m_ReceivedLastestUserId; };
		std::wstring GetChatFromQueue();
		std::vector<std::pair<int, std::wstring>>& GetUserInfoVector() { return m_UserInfoVector; };

	private:

		void makeUserData(const int, const char*);
		void RequestUserList();

		std::vector<std::pair<int, std::wstring>> m_UserInfoVector;
		std::deque<std::wstring> m_ChatQueue;
		bool m_IsRequestNeeded = false;
		bool m_IsChatDelivered = false;
		int m_ReceivedLastestUserId = 0;
	};
}