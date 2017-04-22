#pragma once
#include <map>
#include <vector>
#include <deque>
#include "Packet.h"

using namespace NCommon;

namespace DataContainer
{
	class PacketMessenger;
	class Observer;
	using ObserverQueueVector = std::vector<std::deque<RecvPacketInfo*>*>;

	class PacketProcessor
	{
	public :
		PacketProcessor() = default;
		~PacketProcessor() = default;

		void Update();
		void RegisterMessenger(std::shared_ptr<PacketMessenger>);

		void BroadCast(RecvPacketInfo*);
		void Subscribe(short, std::deque<RecvPacketInfo*>*);

	private :
		
		std::shared_ptr<PacketMessenger> m_pMessengerAddress = nullptr;
		bool m_IsMessengerRegisterd = false;
		std::map<short, ObserverQueueVector> m_ObserverMap;
	};

}