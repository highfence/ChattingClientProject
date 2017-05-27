
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
		// 들어온 패킷에 등록해놓은 함수를 찾는다.
		auto subscribedFunctions = m_PacketFunctionMap.find(packet->PacketId);

		// 대응되는 함수가 없다면 오류를 로깅한다.
		if (subscribedFunctions == m_PacketFunctionMap.end())
		{
			OutputDebugString(L"INVALID BroadCast");
			return;
		}
		else
		{
			// 대응되는 함수를 실행시킨다.
			for (auto& function : subscribedFunctions->second)
			{
				function(packet);
			}
		}
	}

	void PacketDistributer::Subscribe(short interestPacketId, pPacketFunc functor)
	{
		// 해당 패킷 아이디로 구독 등록한 옵저버 큐가 없으면 새로운 리스트 생성.
		if (m_PacketFunctionMap.find(interestPacketId) == m_PacketFunctionMap.end())
		{
			PacketFuncList functionList;
			m_PacketFunctionMap.emplace(interestPacketId, std::move(functionList));

			std::wstring debugLabel = L"[PacketDistributer] 옵져버 함수 큐벡터 생성 ID : " + std::to_wstring(interestPacketId) + L"\n";
			OutputDebugString(debugLabel.c_str());
		}

		m_PacketFunctionMap.find(interestPacketId)->second.emplace_back(std::move(functor));
	}
}