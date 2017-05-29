#include <list>
#include <string>
#include "Util.h"
#include "Observer.h"
#include "RoomData.h"

namespace ClientLogic
{
	// RoomData에서 관심 있는 패킷과 그에 대응하는 처리함수를 구독해주는 함수.
	void RoomData::SetSubscribe(PacketDistributer * publisher)
	{
		publisher->Subscribe(
			(short)PACKET_ID::ROOM_ENTER_USER_NTF,
			std::bind(&RoomData::RoomEnterUserInfoNtf, this, std::placeholders::_1));
		
		publisher->Subscribe(
			(short)PACKET_ID::ROOM_LEAVE_USER_NTF,
			std::bind(&RoomData::RoomLeaveUserInfoNtf, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::ROOM_LEAVE_RES,
			std::bind(&RoomData::RoomLeaveRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::ROOM_CHAT_RES,
			std::bind(&RoomData::RoomChatRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::ROOM_CHAT_NTF,
			std::bind(&RoomData::RoomChatNtf, this, std::placeholders::_1));
	}

	// RoomData의 데이터 정보를 초기화 시켜주는 함수.
	void RoomData::InitializeData()
	{
		m_UserInfoList.clear();
	}

	//-------------------------------------------------------------------------
	// 패킷 처리 관련 함수들. 
	//-------------------------------------------------------------------------
	void RoomData::RoomEnterUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomData] 유저 정보 수령 성공. \n");

		auto recvData = (PktRoomEnterUserInfoNtf*)packet->pData;

		auto userId = recvData->UserID;
		m_UserInfoList.emplace_back(std::move(Util::CharToWstring(userId)));

		VersionUp();
	}

	void RoomData::RoomLeaveUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
	}

	void RoomData::RoomLeaveRes(std::shared_ptr<RecvPacketInfo> packet)
	{
	}

	void RoomData::RoomChatRes(std::shared_ptr<RecvPacketInfo> packet)
	{
	}

	void RoomData::RoomChatNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
	}


}