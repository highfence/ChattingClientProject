#pragma once
#include <Windows.h>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <functional>
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

		virtual void OnPacketReceive() = 0;
		virtual void RegisterPacketProcess() = 0;
		virtual void Subscribe(short, PacketDistributer* processor);
		int GetVersion() const { return m_Version; };

	protected :
		using pPacketFunc = std::function<void(std::shared_ptr<RecvPacketInfo>)>;

		void VersionUp() { ++m_Version; };
		
		std::deque<std::shared_ptr<RecvPacketInfo>> m_RecvQueue;
		std::unordered_map<short, pPacketFunc> m_PacketFuncMap;
		std::mutex m_Mutex;
		int m_Version = 0;
	};


}