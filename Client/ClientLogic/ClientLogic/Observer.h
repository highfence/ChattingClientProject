#pragma once
#include <Windows.h>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include "Packet.h"
#include "PacketProcessor.h"

// TODO :: Observer�� OnReceive() ���� �Լ����� ���� �������̽��� �����غ���. (������ 25, 26, 27)
// I~able�� ������ ���� ����.

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