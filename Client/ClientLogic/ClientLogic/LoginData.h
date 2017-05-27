#pragma once

namespace ClientLogic
{
	class LoginData : public Observer
	{
	public:
		LoginData() = default;
		~LoginData() = default;

		void SetSubscribe(PacketDistributer* publisher);
		bool GetLoginSuccessed();

	private:

		void LoginInRes(std::shared_ptr<RecvPacketInfo> packet);
		bool m_IsLoginSuccessed = false;
	};

}