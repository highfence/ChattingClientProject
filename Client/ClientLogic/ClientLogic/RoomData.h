#pragma once


namespace ClientLogic
{
	class RoomData : public Observer
	{
	public:
		RoomData() = default;
		~RoomData() = default;

		void SetSubscribe(PacketDistributer* publisher);

	private:

		void InitializeData();
		void RoomEnterRes(std::shared_ptr<RecvPacketInfo> packet);
		std::list<std::wstring> m_UserInfoList;

		bool m_IsRoomSuccesslyEntered = false;
	};
}