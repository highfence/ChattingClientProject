#include "Observer.h"
#include "LobbyListData.h"

namespace ClientLogic
{
	void LobbyListData::OnPacketReceive()
	{
		/* 받은 큐가 비었다면 차례를 넘긴다. */
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		/* 큐에 패킷이 있다면 packet을 받는다. */
		auto packet = m_RecvQueue.front();

		/* 구독한 패킷 아이디로 unorded_map 에서 대응되는 함수를 찾는다. */
		auto packetProcessFunc = m_PacketFuncMap.find(packet->PacketId);

		/* 대응되는 함수가 없을 경우. */
		if (packetProcessFunc == m_PacketFuncMap.end())
		{
			OutputDebugString(L"[LobbyListData] 구독하지 않은 패킷을 전송받음. \n");
		}
		/* 대응되는 함수가 있는 경우. */
		else
		{
			packetProcessFunc->second(packet);
		}

		m_RecvQueue.pop_front();
	}

	void LobbyListData::RegisterPacketProcess()
	{
		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_LIST_RES,
				[this](std::shared_ptr<RecvPacketInfo> packet) { this->LobbyListRes(packet); }));

		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_ENTER_RES,
				[this](std::shared_ptr<RecvPacketInfo> packet) { this->LobbyEnterRes(packet); }));

		m_PacketFuncMap.emplace(
			std::make_pair<short, pPacketFunc>(
				(short)PACKET_ID::LOBBY_ENTER_USER_NTF,
				[this](std::shared_ptr<RecvPacketInfo> packet) { this->LobbyEnterUserNtf(packet); }));
	}

	void LobbyListData::SetSubscribe(PacketDistributer* publisher)
	{
		/* 구독한 패킷에 대해서는 PacketProcess를 만들어주고, 등록해주어야 함. */
		publisher->Subscribe((short)PACKET_ID::LOBBY_LIST_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);

		/* 구독한 패킷 아이디에 대응되는 Function들을 가지고 있어야한다. */
		RegisterPacketProcess();
	}

	const LobbyListInfo * LobbyListData::GetLobbyListInfo(const int listIdx) const
	{
		const auto listLobbyConstPointer = &m_LobbyList[listIdx];
		return listLobbyConstPointer;
	}

	bool LobbyListData::GetIsLobbySuccesslyEntered() 
	{
		if (m_IsLobbySuccesslyEntered)
		{
			m_IsLobbySuccesslyEntered = false;
			return true;
		}
		return false;
	}

	void LobbyListData::LoadData(PktLobbyListRes* pLobbyListData)
	{
		m_LobbyCount = pLobbyListData->LobbyCount;

		for (int i = 0; i < m_LobbyCount; ++i)
		{
			m_LobbyList[i] = pLobbyListData->LobbyList[i];
		}
	}

	void LobbyListData::LobbyListRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto pLobbyListData = (PktLobbyListRes*)packet->pData;
		if (pLobbyListData->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LobbyListData] 로비리스트 수령 실패!\n");
		}
		else
		{
			OutputDebugString(L"[LobbyListData] 로비리스트 성공적으로 수령\n");
			LoadData(pLobbyListData);
			VersionUp();
		}
	}

	void LobbyListData::LobbyEnterRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto pLobbyEnterResData = (PktLobbyEnterRes*)packet->pData;
		if (pLobbyEnterResData->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LobbyListData] 로비 진입 데이터 수령 실패!\n");
		}
		else
		{
			OutputDebugString(L"[LobbyListData] 로비 진입 데이터 성공적으로 수령\n");
			m_IsLobbySuccesslyEntered = true;
			VersionUp();
		}
	}

	void LobbyListData::LobbyEnterUserNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[LobbyListData] LOBBY_ENTER_USER_NTF 패킷 수령\n");
	}
}