#pragma once
#include <list>
#include <queue>
#include <string>
#include "Definition.h"


namespace ClientLogic
{
	class RoomData : public Observer
	{
	public:
		RoomData() = default;
		~RoomData() = default;

		void SetSubscribe(PacketDistributer* publisher);
		void PushChatDataWaitingLine(std::wstring& id, std::wstring& chatMsg);

		//----------------------------------------------------------
		// Getter, Setter
		//----------------------------------------------------------
		std::list<std::wstring>& GetRefUserInfoList() { return m_UserInfoList; };
		bool GetChatDataFromQueue(std::wstring& pOutChatData);
		bool GetIsLeaveRoomSuccessed();

	private:

		void InitializeData();

		void RoomEnterUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet);
		void RoomLeaveUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet);
		void RoomLeaveRes(std::shared_ptr<RecvPacketInfo> packet);
		void RoomChatRes(std::shared_ptr<RecvPacketInfo> packet);
		void RoomChatNtf(std::shared_ptr<RecvPacketInfo> packet);

		std::list<std::wstring> m_UserInfoList;
		std::queue<std::shared_ptr<ChatData>> m_ResWaitingChatQueue;
		std::queue<std::shared_ptr<ChatData>> m_ChatQueue;

		bool m_IsLeaveRoomSuccessed = false;
	};
}