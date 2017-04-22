#include <Windows.h>
#include <map>
#include <memory>
#include <vector>
#include "Observer.h"
#include "PacketMessenger.h"
#include "PacketProcessor.h"

namespace DataContainer
{
	void PacketProcessor::Update()
	{
		// 메신저 등록이 안되어 있을 경우 Update를 실행하지 못함. 
		if (m_IsMessengerRegisterd == false)
		{
			return;
		}

		auto packet = m_MessengerAddress->GetPacketFromDeque();

		/* 큐에 패킷이 없으면 Update를 잠깐 쉰다. */
		if (packet == nullptr)
		{
			Sleep(0);
			return;
		}

		/* 패킷이 있다면, 구독해 놓은 옵저버들에게 브로드캐스트. */
		BroadCast(packet);
	}

	void PacketProcessor::RegisterMessenger(PacketMessenger * messengerAddress)
	{
		if (m_IsMessengerRegisterd == false)
		{
			m_MessengerAddress = messengerAddress;
			m_IsMessengerRegisterd = true;
		}
	}

	void PacketProcessor::BroadCast(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto subscribers = m_ObserverMap.find(packet->PacketId);
		if (subscribers == m_ObserverMap.end())
		{
			OutputDebugString(L"INVALID BroadCast");
			return;
		}
		else
		{
			for (auto& i : subscribers->second)
			{
				i->Update(packet);
			}
		}
	}

	void PacketProcessor::Subscribe(short interestPacketId, std::shared_ptr<Observer> observerInstance)
	{
		// 해당 패킷 아이디로 구독 등록한 옵저버가 없으면 새로운 리스트 생성.
		if (m_ObserverMap.find(interestPacketId) == m_ObserverMap.end())
		{
			ObserverVector newObserverVector;
			m_ObserverMap.emplace(interestPacketId, std::move(newObserverVector));
		}

		// 리스트에 옵저버 추가.
		m_ObserverMap.at(interestPacketId).emplace_back(observerInstance);
	}

}