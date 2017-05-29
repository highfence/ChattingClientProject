#include <list>
#include <string>
#include "Observer.h"
#include "RoomData.h"

namespace ClientLogic
{
	void RoomData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe(
			(short)PACKET_ID::ROOM_ENTER_RES,
			std::bind(&RoomData::RoomEnterRes, this, std::placeholders::_1));
	}

	void RoomData::InitializeData()
	{
		m_IsRoomSuccesslyEntered = false;
	}

	void RoomData::RoomEnterRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomEnterRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomData] Room 진입 실패 \n");
		}
		else
		{
			OutputDebugString(L"[RoomData] Room 진입 성공 \n");
			m_IsRoomSuccesslyEntered = true;
			VersionUp();
		}
	}
}