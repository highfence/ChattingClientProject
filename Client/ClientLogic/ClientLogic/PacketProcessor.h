#pragma once
#include <map>

namespace DataContainer
{
	class PacketMessenger;
	class Observer;
	using ObserverQueueVector = std::vector<std::shared_ptr<std::deque<RecvPacketInfo>*>>;

	class PacketProcessor
	{
	public :
		PacketProcessor() = default;
		~PacketProcessor() = default;

		void Update();
		void RegisterMessenger(std::shared_ptr<PacketMessenger>);

		void BroadCast(std::shared_ptr<RecvPacketInfo>);
		void Subscribe(short, std::deque<std::shared_ptr<RecvPacketInfo>>*);

	private :
		
		std::shared_ptr<PacketMessenger> m_pMessengerAddress = nullptr;
		bool m_IsMessengerRegisterd = false;
		std::map<short, ObserverQueueVector> m_ObserverMap;
	};

}