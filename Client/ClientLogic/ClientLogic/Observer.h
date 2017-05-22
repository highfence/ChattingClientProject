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

		void VersionUp() { ++m_Version; };
		
		std::deque<std::shared_ptr<RecvPacketInfo>> m_RecvQueue;

		/* 패킷 아이디에 따라 알맞은 함수 포인터를 들고 있는 unordered_map */
		using pPacketFunc = std::function<void(std::shared_ptr<RecvPacketInfo>)>;
		std::unordered_map<short, pPacketFunc> m_PacketFuncMap;

		int m_Version = 0;
	};


}