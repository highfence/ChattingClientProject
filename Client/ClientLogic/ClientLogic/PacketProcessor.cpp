
#include "PacketMessenger.h"
#include "PacketProcessor.h"
#include <string>

namespace ClientLogic
{
	void PacketDistributer::Update()
	{
		// 메신저 등록이 안되어 있을 경우 Update를 실행하지 못함. 
		if (m_IsMessengerRegisterd == false)
		{
			return;
		}

		auto packet = m_pMessengerAddress->GetPacketFromDeque();

		/* 큐에 패킷이 없으면 Update를 잠깐 쉰다. */
		if (packet == nullptr)
		{
			Sleep(0);
			return;
		}

		/* 패킷이 있다면, 구독해 놓은 옵저버들에게 브로드캐스트. */
		std::wstring debugLabel = L"[PacketDistributer] ID : " + std::to_wstring(packet->PacketId) + L"브로드 캐스트\n";
		OutputDebugString(debugLabel.c_str());
		BroadCast(packet);
	}

	void PacketDistributer::RegisterMessenger(PacketMessenger* messengerAddress)
	{
		if (m_IsMessengerRegisterd == false)
		{
			m_pMessengerAddress = messengerAddress;
			m_IsMessengerRegisterd = true;
		}
	}

	void PacketDistributer::BroadCast(std::shared_ptr<RecvPacketInfo> packet)
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
				i->push_back(packet);
			}
		}
	}

	void PacketDistributer::Subscribe(short interestPacketId, ObserverQueue* observerPacketQueue)
	{
		// 해당 패킷 아이디로 구독 등록한 옵저버 큐가 없으면 새로운 리스트 생성.
		if (m_ObserverMap.find(interestPacketId) == m_ObserverMap.end())
		{
			ObserverQueueVector newObserverVector;
			m_ObserverMap.emplace(interestPacketId, std::move(newObserverVector));

			std::wstring debugLabel = L"[PacketDistributer] 옵져버 큐벡터 생성 ID : " + std::to_wstring(interestPacketId) + L"\n";
			OutputDebugString(debugLabel.c_str());
		}

		// 리스트에 옵저버 큐 추가.
		m_ObserverMap.at(interestPacketId).emplace_back(observerPacketQueue);
	}
}