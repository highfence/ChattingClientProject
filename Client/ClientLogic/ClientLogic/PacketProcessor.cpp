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
		// �޽��� ����� �ȵǾ� ���� ��� Update�� �������� ����. 
		if (m_IsMessengerRegisterd == false)
		{
			return;
		}

		auto packet = m_MessengerAddress->GetPacketFromDeque();

		/* ť�� ��Ŷ�� ������ Update�� ��� ����. */
		if (packet == nullptr)
		{
			Sleep(0);
			return;
		}

		/* ��Ŷ�� �ִٸ�, ������ ���� �������鿡�� ��ε�ĳ��Ʈ. */
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

	void PacketProcessor::BroadCast(RecvPacketInfo* packet)
	{

	}

	void PacketProcessor::Subscribe(short interestPacketId, std::shared_ptr<Observer> observerInstance)
	{
		// �ش� ��Ŷ ���̵�� ���� ����� �������� ������ ���ο� ����Ʈ ����.
		if (m_ObserverMap.find(interestPacketId) == m_ObserverMap.end())
		{
			ObserverVector newObserverVector;
			m_ObserverMap.emplace(interestPacketId, std::move(newObserverVector));
		}

		// ����Ʈ�� ������ �߰�.
		m_ObserverMap.at(interestPacketId).emplace_back(observerInstance);
	}

	//void PacketProcessor::UnSubscribe(short subscribedPacketId, std::shared_ptr<Observer> observerInstance)
	//{
	//	auto iterVector = m_ObserverMap.find(subscribedPacketId);

	//	if (iterVector == m_ObserverMap.end())
	//	{
	//		// �߸��� UnSubscribe ��û
	//		OutputDebugString(L"INVALID UnSubscribe Call!");
	//		return;
	//	}
	//	else
	//	{
	//		
	//	}
	//}

}