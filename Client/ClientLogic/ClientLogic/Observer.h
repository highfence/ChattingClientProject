#pragma once

using namespace NCommon;

namespace DataContainer
{
	class PacketProcessor;

	class Observer
	{
	public:

		virtual void Update(std::shared_ptr<RecvPacketInfo*>) = 0;
		virtual void Subscribe(std::shared_ptr<PacketProcessor*>) = 0;
	};

	class LoginData : public Observer
	{
	public :
		void Subscribe(std::shared_ptr<PacketProcessor*>) override;
		void Update(std::shared_ptr<RecvPacketInfo*>) override;

		bool GetLoginSuccessed() const { return m_IsLoginSuccessed; };

	private :

		bool m_IsLoginSuccessed = false;
	};
}