#pragma once
#include <Windows.h>
#include <unordered_map>
#include <vector>
#include <deque>
#include "Packet.h"


using namespace NCommon;

namespace ClientLogic
{
	class PacketMessenger;
	using ObserverQueue = std::deque<std::shared_ptr<RecvPacketInfo>>;
	using ObserverQueueVector = std::vector<ObserverQueue*>;

	class PacketDistributer
	{
	public :
		PacketDistributer() = default;
		~PacketDistributer() = default;

		void Update();
		void RegisterMessenger(PacketMessenger* messengerAddress);

		void BroadCast(std::shared_ptr<RecvPacketInfo>);
		void Subscribe(short, ObserverQueue*);

	private :
		
		PacketMessenger* m_pMessengerAddress = nullptr;
		bool m_IsMessengerRegisterd = false;
		std::unordered_map<short, ObserverQueueVector> m_ObserverMap;
	};

}