
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
		// ���� ��Ŷ�� ����س��� �Լ��� ã�´�.
		auto subscribedFunctions = m_PacketFunctionMap.find(packet->PacketId);

		// �����Ǵ� �Լ��� ���ٸ� ������ �α��Ѵ�.
		if (subscribedFunctions == m_PacketFunctionMap.end())
		{
			OutputDebugString(L"INVALID BroadCast");
			return;
		}
		else
		{
			// �����Ǵ� �Լ��� �����Ų��.
			for (auto& function : subscribedFunctions->second)
			{
				function(packet);
			}
		}
	}

	void PacketDistributer::Subscribe(short interestPacketId, pPacketFunc functor)
	{
		// �ش� ��Ŷ ���̵�� ���� ����� ������ ť�� ������ ���ο� ����Ʈ ����.
		if (m_PacketFunctionMap.find(interestPacketId) == m_PacketFunctionMap.end())
		{
			PacketFuncList functionList;
			m_PacketFunctionMap.emplace(interestPacketId, std::move(functionList));

			std::wstring debugLabel = L"[PacketDistributer] ������ �Լ� ť���� ���� ID : " + std::to_wstring(interestPacketId) + L"\n";
			OutputDebugString(debugLabel.c_str());
		}

		m_PacketFunctionMap.find(interestPacketId)->second.emplace_back(std::move(functor));
	}
}