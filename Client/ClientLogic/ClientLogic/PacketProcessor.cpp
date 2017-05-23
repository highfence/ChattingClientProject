
#include "PacketMessenger.h"
#include "PacketProcessor.h"
#include <string>

namespace ClientLogic
{
	void PacketDistributer::Update()
	{
		// �޽��� ����� �ȵǾ� ���� ��� Update�� �������� ����. 
		if (m_IsMessengerRegisterd == false)
		{
			return;
		}

		auto packet = m_pMessengerAddress->GetPacketFromDeque();

		/* ť�� ��Ŷ�� ������ Update�� ��� ����. */
		if (packet == nullptr)
		{
			Sleep(0);
			return;
		}

		/* ��Ŷ�� �ִٸ�, ������ ���� �������鿡�� ��ε�ĳ��Ʈ. */
		std::wstring debugLabel = L"[PacketDistributer] ID : " + std::to_wstring(packet->PacketId) + L"��ε� ĳ��Ʈ\n";
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
		// �ش� ��Ŷ ���̵�� ���� ����� ������ ť�� ������ ���ο� ����Ʈ ����.
		if (m_ObserverMap.find(interestPacketId) == m_ObserverMap.end())
		{
			ObserverQueueVector newObserverVector;
			m_ObserverMap.emplace(interestPacketId, std::move(newObserverVector));

			std::wstring debugLabel = L"[PacketDistributer] ������ ť���� ���� ID : " + std::to_wstring(interestPacketId) + L"\n";
			OutputDebugString(debugLabel.c_str());
		}

		// ����Ʈ�� ������ ť �߰�.
		m_ObserverMap.at(interestPacketId).emplace_back(observerPacketQueue);
	}
}