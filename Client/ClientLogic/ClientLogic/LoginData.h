#pragma once

namespace ClientLogic
{
	class LoginData : public Observer
	{
	public:
		LoginData() = default;
		~LoginData() = default;

		void OnPacketReceive() override;
		void RegisterPacketProcess() override;
		void SetSubscribe(PacketDistributer* publisher);

		bool GetLoginSuccessed() const { return m_IsLoginSuccessed; };

	private:

		void LoginInRes(std::shared_ptr<RecvPacketInfo> packet);
		bool m_IsLoginSuccessed = false;
	};

}