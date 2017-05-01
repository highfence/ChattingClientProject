#pragma once
#include <Windows.h>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include "Packet.h"
#include "PacketProcessor.h"

// TODO :: Observer는 OnReceive() 가상 함수만을 가진 인터페이스로 구현해보자. (다음주 25, 26, 27)
// I~able로 구현된 것이 많음.

namespace ClientLogic
{
	class PacketDistributer;

	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = default;

		virtual void Update() = 0;
		virtual void Subscribe(short, PacketDistributer* processor);
		int GetVersion() const { return m_Version; };

	protected :

		void VersionUp() { ++m_Version; };
		
		std::deque<std::shared_ptr<RecvPacketInfo>> m_RecvQueue;
		std::mutex m_Mutex;
		int m_Version = 0;
	};


}