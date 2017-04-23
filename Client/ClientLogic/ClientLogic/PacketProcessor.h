#pragma once
#include <Windows.h>
#include <map>
#include <vector>
#include <deque>
#include "Packet.h"


using namespace NCommon;

namespace ClientLogic
{
	class PacketMessenger;
	using ObserverQueueVector = std::vector<std::deque<std::shared_ptr<RecvPacketInfo>>*>;

	class PacketDistributer
	{
	public :
		PacketDistributer() = default;
		~PacketDistributer() = default;

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