#pragma once
#include <Windows.h>
#include <unordered_map>
#include <deque>
#include <list>
#include "Packet.h"


using namespace NCommon;

namespace ClientLogic
{
	class PacketMessenger;
	using pPacketFunc = std::function<void(std::shared_ptr<RecvPacketInfo>)>;
	using PacketFuncList = std::list<pPacketFunc>;

	class PacketDistributer
	{
	public :
		PacketDistributer() = default;
		~PacketDistributer() = default;

		void Update();
		void RegisterMessenger(PacketMessenger* messengerAddress);

		void BroadCast(std::shared_ptr<RecvPacketInfo>);
		void Subscribe(short, pPacketFunc);

	private :
		
		PacketMessenger* m_pMessengerAddress = nullptr;
		bool m_IsMessengerRegisterd = false;
		std::unordered_map<short, PacketFuncList> m_PacketFunctionMap;
	};

}