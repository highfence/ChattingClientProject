#pragma once

using namespace NCommon;

namespace DataContainer
{
	class PacketProcessor;

	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = 0;

		virtual void Update() = 0;
		virtual void Subscribe(std::shared_ptr<PacketProcessor>) = 0;

	protected :
		std::deque<std::shared_ptr<RecvPacketInfo>> m_RecvQueue;
	};

	class LoginData : public Observer
	{
	public :
		LoginData() = default;
		~LoginData() = default;

		void Update() override;
		void Subscribe(std::shared_ptr<PacketProcessor>) override;

		bool GetLoginSuccessed() const { return m_IsLoginSuccessed; };

	private :

		bool m_IsLoginSuccessed = false;
	};
}