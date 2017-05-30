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

	// 채팅을 보내놓고 응답이 오기를 기다리도록 하는 함수.
	void RoomData::PushChatDataForWaitRes(std::wstring id, std::wstring chatMsg)
	{
		m_WaitingResChatQueue.emplace(std::make_shared<ChatData>(id, chatMsg));
	}

	// 저장된 채팅 큐에서 한 라인을 뽑아주는 함수.
	std::wstring RoomData::GetChatDataFromQueue()
	{
		if (m_ChatQueue.empty()) return L"";

		auto returnString = m_ChatQueue.front()->GetInLine();
		m_ChatQueue.pop();
		return returnString;
	}

	// 룸을 잘 나갔는지 확인하는 함수.
	bool RoomData::GetIsLeaveRoomSuccessed()
	{
		if (m_IsLeaveRoomSuccessed)
		{
			m_IsLeaveRoomSuccessed = false;
			return true;
		}
		return false;
	}

	// RoomData의 데이터 정보를 초기화 시켜주는 함수.
	void RoomData::InitializeData()
	{
		m_IsLeaveRoomSuccessed = false;
		m_UserInfoList.clear();
	}

	//-------------------------------------------------------------------------
	// 패킷 처리 관련 함수들. 
	//-------------------------------------------------------------------------
	void RoomData::RoomEnterUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomData] 유저 입장 정보 수령 성공. \n");

		auto recvData = (PktRoomEnterUserInfoNtf*)packet->pData;

		auto userId = recvData->UserID;
		m_UserInfoList.emplace_back(std::move(Util::CharToWstring(userId)));

		VersionUp();
	}

	void RoomData::RoomLeaveUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{ 
		OutputDebugString(L"[RoomData] 유저 퇴장 정보 수령 성공. \n");

		// 퇴장 유저 아이디 획득.
		auto recvData = (PktRoomLeaveUserInfoNtf*)packet->pData;
		std::wstring leavedUserId = Util::CharToWstring(recvData->UserID);

		// 아이디가 같은지 판별해줄 람다 함수식.
		auto IsIdEqual = [&leavedUserId](const std::wstring existedUserId)->bool
		{
			return existedUserId == leavedUserId;
		};

		// 퇴장한 유저의 아이디를 찾아 삭제해준다.
		m_UserInfoList.remove_if(IsIdEqual);

		VersionUp();
	}

	void RoomData::RoomLeaveRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomLeaveRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomData] 룸에서 퇴장 실패. \n");
			return;
		}
		OutputDebugString(L"[RoomData] 룸에서 퇴장 성공. \n");

		// Room에서 참조할 수 있도록 퇴장이 성공했다는 표시를 해준다.
		m_IsLeaveRoomSuccessed = true;
	}

	void RoomData::RoomChatRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomChatRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomData] 채팅 송신 실패. \n");
		}
		else
		{
			OutputDebugString(L"[RoomData] 채팅 성공적으로 송신. \n");

			// 채팅 데이터를 응답 대기열에서 채팅 큐로 옮겨준다.
			auto getResChatData = m_WaitingResChatQueue.front();
			m_WaitingResChatQueue.pop();

			m_ChatQueue.emplace(std::move(getResChatData));

			VersionUp();
		}
	}

	void RoomData::RoomChatNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomChatNtf*)packet->pData;
		OutputDebugString(L"[RoomData] 채팅 성공적으로 수신. \n");

		std::wstring userIdStr = Util::CharToWstring(recvData->UserID);
		std::wstring userMsgStr(recvData->Msg);

		m_ChatQueue.emplace(std::make_shared<ChatData>(userIdStr, userMsgStr));

		VersionUp();
	}

}