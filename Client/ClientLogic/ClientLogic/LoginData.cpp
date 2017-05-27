#include <functional>
#include "Observer.h"
#include "LoginData.h"

namespace ClientLogic
{
	void LoginData::SetSubscribe(PacketDistributer* publisher)
	{
		// 패킷 처리 함수를 PacketFunctionMap에 건네준다. 
		publisher->Subscribe(
			(short)PACKET_ID::LOGIN_IN_RES,
			std::bind(
				&LoginData::LoginInRes,
				this,
				std::placeholders::_1));
	}

	bool LoginData::GetLoginSuccessed()
	{
		if (m_IsLoginSuccessed == true)
		{
			m_IsLoginSuccessed = false;
			return true;
		}
		return false;
	}

	// PACKET_ID::LOGIN_IN_RES를 처리하는 함수.
	void LoginData::LoginInRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		// 에러코드 확인.
		auto i = (PktLogInRes*)packet->pData;
		if (i->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LoginData] 로그인 실패!\n");
		}
		else
		{
			// 로그인이 성공했다면 로그인이 성공했고 변수를 바꿔준다.
			OutputDebugString(L"[LoginData] 로그인 성공!\n");
			m_IsLoginSuccessed = true;
			VersionUp();
		}
	}
}