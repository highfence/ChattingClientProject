#pragma once
#include <vector>
#include <map>
#include "Packet.h"

using namespace NCommon;

namespace DataContainer
{
	class PacketMessenger;
	class Observer;
	using ObserverQueueVector = std::vector<std::deque<std::shared_ptr<RecvPacketInfo>>*>;

	class PacketProcessor
	{
	public :
		PacketProcessor() = default;
		~PacketProcessor() = default;

		void Update();
		void RegisterMessenger(PacketMessenger*);

		void BroadCast(std::shared_ptr<RecvPacketInfo>);
		void Subscribe(short, std::deque<std::shared_ptr<RecvPacketInfo>>*);

	private :
		
		PacketMessenger* m_MessengerAddress = nullptr;
		bool m_IsMessengerRegisterd = false;
		std::map<short, ObserverQueueVector> m_ObserverMap;
	};

}