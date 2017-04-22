#pragma once

namespace DataContainer
{
	class PacketMessenger;
	class Observer;
	using ObserverVector = std::vector<std::shared_ptr<Observer>>;

	class PacketProcessor
	{
	public :
		PacketProcessor() = default;
		~PacketProcessor() = default;

		void Update();
		void RegisterMessenger(PacketMessenger*);

		void BroadCast(RecvPacketInfo*);
		void Subscribe(short, std::shared_ptr<Observer>);
		//void UnSubscribe(short, std::shared_ptr<Observer>);

	private :
		
		PacketMessenger* m_MessengerAddress = nullptr;
		bool m_IsMessengerRegisterd = false;
		std::map<short, ObserverVector> m_ObserverMap;
	};

}