#pragma once


namespace ClientLogic
{
	class RoomData : public Observer
	{
	public:
		RoomData() = default;
		~RoomData() = default;

		void SetSubscribe(PacketDistributer* publisher);

		//----------------------------------------------------------
		// Getter, Setter
		//----------------------------------------------------------
		std::list<std::wstring>& GetRefUserInfoList() { return m_UserInfoList; };


	private:

		void InitializeData();

		void RoomEnterUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet);
		void RoomLeaveUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet);
		void RoomLeaveRes(std::shared_ptr<RecvPacketInfo> packet);
		void RoomChatRes(std::shared_ptr<RecvPacketInfo> packet);
		void RoomChatNtf(std::shared_ptr<RecvPacketInfo> packet);

		std::list<std::wstring> m_UserInfoList;
	};
}