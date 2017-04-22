#pragma once

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
		void RegisterMessenger(PacketMessenger*);

		void BroadCast(std::shared_ptr<RecvPacketInfo>);
		void Subscribe(short, std::deque<std::shared_ptr<RecvPacketInfo>>*);

	private :
		
		PacketMessenger* m_MessengerAddress = nullptr;
		bool m_IsMessengerRegisterd = false;
		std::map<short, ObserverQueueVector> m_ObserverMap;
	};

}