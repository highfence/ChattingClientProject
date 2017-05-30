#include <list>
#include <string>
#include "Util.h"
#include "Observer.h"
#include "RoomData.h"

namespace ClientLogic
{
	// RoomData���� ���� �ִ� ��Ŷ�� �׿� �����ϴ� ó���Լ��� �������ִ� �Լ�.
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

	// ���� ä�� �����͸� ���� ��⿭�� �־��ִ� �Լ�.
	void RoomData::PushChatDataWaitingLine(std::wstring & id, std::wstring & chatMsg)
	{
		m_ResWaitingChatQueue.emplace(std::make_shared<ChatData>(id, chatMsg));
	}

	// ����� ä�� ť���� �� ������ �̾��ִ� �Լ�.
	bool RoomData::GetChatDataFromQueue(std::wstring& refOutChatData)
	{
		// ������ ���.
		if (m_ChatQueue.empty()) return false;

		refOutChatData = m_ChatQueue.front()->GetInLine();
		m_ChatQueue.pop();
		return true;
	}

	// ���� �� �������� Ȯ���ϴ� �Լ�.
	bool RoomData::GetIsLeaveRoomSuccessed()
	{
		if (m_IsLeaveRoomSuccessed)
		{
			m_IsLeaveRoomSuccessed = false;
			return true;
		}
		return false;
	}

	// RoomData�� ������ ������ �ʱ�ȭ �����ִ� �Լ�.
	void RoomData::InitializeData()
	{
		m_IsLeaveRoomSuccessed = false;
		m_UserInfoList.clear();
	}

	//-------------------------------------------------------------------------
	// ��Ŷ ó�� ���� �Լ���. 
	//-------------------------------------------------------------------------
	void RoomData::RoomEnterUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[RoomData] ���� ���� ���� ���� ����. \n");

		auto recvData = (PktRoomEnterUserInfoNtf*)packet->pData;

		auto userId = recvData->UserID;
		m_UserInfoList.emplace_back(std::move(Util::CharToWstring(userId)));

		VersionUp();
	}

	void RoomData::RoomLeaveUserInfoNtf(std::shared_ptr<RecvPacketInfo> packet)
	{ 
		OutputDebugString(L"[RoomData] ���� ���� ���� ���� ����. \n");

		// ���� ���� ���̵� ȹ��.
		auto recvData = (PktRoomLeaveUserInfoNtf*)packet->pData;
		std::wstring leavedUserId = Util::CharToWstring(recvData->UserID);

		// ������ ������ ���̵� �������ش�.
		m_UserInfoList.remove_if((
			m_UserInfoList.begin(),
			m_UserInfoList.end(),
			[leavedUserId](std::wstring existedUserId)
		{
			return leavedUserId == existedUserId;
		},
			m_UserInfoList.end()));
	}

	void RoomData::RoomLeaveRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomLeaveRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomData] �뿡�� ���� ����. \n");
			return;
		}
		OutputDebugString(L"[RoomData] �뿡�� ���� ����. \n");

		// Room���� ������ �� �ֵ��� ������ �����ߴٴ� ǥ�ø� ���ش�.
		m_IsLeaveRoomSuccessed = true;
	}

	void RoomData::RoomChatRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomChatRes*)packet->pData;

		if (recvData->ErrorCode != 0)
		{
			OutputDebugString(L"[RoomData] ä�� �۽� ����. \n");
		}
		else
		{
			OutputDebugString(L"[RoomData] ä�� ���������� �۽�. \n");
			// ä���� ���� �Ϸ�Ǿ����Ƿ� �亯�� ��ٸ��� ä�� ����ü�� �Ű��ش�.
			m_ChatQueue.emplace(m_ResWaitingChatQueue.front());
			VersionUp();
		}

		// �����̵� ���е� ť���� �ϳ��� ���ش�.
		m_ResWaitingChatQueue.pop();
	}

	void RoomData::RoomChatNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto recvData = (PktRoomChatNtf*)packet->pData;
		OutputDebugString(L"[RoomData] ä�� ���������� ����. \n");

		std::wstring userIdStr = Util::CharToWstring(recvData->UserID);
		std::wstring userMsgStr(recvData->Msg);

		m_ChatQueue.emplace(std::make_shared<ChatData>(userIdStr, userMsgStr));

		VersionUp();
	}

}