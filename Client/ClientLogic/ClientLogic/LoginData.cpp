#include <functional>
#include "Observer.h"
#include "LoginData.h"

namespace ClientLogic
{
	void LoginData::SetSubscribe(PacketDistributer* publisher)
	{
		// ��Ŷ ó�� �Լ��� PacketFunctionMap�� �ǳ��ش�. 
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

	// PACKET_ID::LOGIN_IN_RES�� ó���ϴ� �Լ�.
	void LoginData::LoginInRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		// �����ڵ� Ȯ��.
		auto i = (PktLogInRes*)packet->pData;
		if (i->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LoginData] �α��� ����!\n");
		}
		else
		{
			// �α����� �����ߴٸ� �α����� �����߰� ������ �ٲ��ش�.
			OutputDebugString(L"[LoginData] �α��� ����!\n");
			m_IsLoginSuccessed = true;
			VersionUp();
		}
	}
}