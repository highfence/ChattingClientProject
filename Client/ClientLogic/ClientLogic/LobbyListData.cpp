#include "Observer.h"
#include "LobbyListData.h"

namespace ClientLogic
{
	void LobbyListData::Update()
	{
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		auto packet = m_RecvQueue.front();

		/* 로비 리스트 답변 */
		if (packet->PacketId == (short)PACKET_ID::LOBBY_LIST_RES)
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
		/* 로비 진입 데이터 답변 */
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_RES)
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
		/* 다른 사람의 로비 진입 데이터 */
		else if (packet->PacketId == (short)PACKET_ID::LOBBY_ENTER_USER_NTF)
		{
			OutputDebugString(L"[LobbyListData] LOBBY_ENTER_USER_NTF 패킷 수령\n");
		}
		else
		{
			OutputDebugString(L"Invalid Packet Receive! (In LobbyListData Update)\n");
			return;
		}

		m_RecvQueue.pop_front();
	}


	void LobbyListData::SetSubscribe(PacketDistributer* publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_LIST_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);
	}

	const LobbyListInfo * LobbyListData::GetLobbyListInfo(const int idx) const
	{
		const auto listLobbyConstPointer = &m_LobbyList[idx];
		return listLobbyConstPointer;
	}

	void LobbyListData::LoadData(PktLobbyListRes* pLobbyListData)
	{
		m_LobbyCount = pLobbyListData->LobbyCount;

		for (int i = 0; i < m_LobbyCount; ++i)
		{
			m_LobbyList[i] = pLobbyListData->LobbyList[i];
		}
	}
}