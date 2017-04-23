#pragma once
#include <mutex>
#include "Packet.h"

using namespace NCommon;

namespace DataContainer
{
	class PacketProcessor;

	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = default;

		virtual void Update() = 0;
		virtual void Subscribe(std::shared_ptr<PacketProcessor>) = 0;

	protected :
		std::deque<RecvPacketInfo*> m_RecvQueue;
		std::mutex m_Mutex;
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