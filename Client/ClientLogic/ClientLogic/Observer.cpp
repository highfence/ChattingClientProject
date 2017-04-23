#include "Observer.h"

namespace ClientLogic
{
	void Observer::Subscribe(short packetId, std::shared_ptr<PacketDistributer> processor)
	{
		processor->Subscribe(packetId, &m_RecvQueue);
	}

	void LoginData::Update()
	{
		// 받은 큐가 비어있으면 일하지 않음.
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		// 큐가 있다면 빼서, LOGIN_IN_RES 패킷이 왔는지 확인.
		auto packet = m_RecvQueue.front();

		if (packet->PacketId != (short)PACKET_ID::LOGIN_IN_RES)
		{
			// 뭔가 잘못된 패킷이 옴.
			OutputDebugString(L"Invaild Packet Receive! (In LoginData Update)\n");
			return;
		}

		// 올바른 패킷이 왔다면, 에러코드 확인.
		auto i = (PktLogInRes*)packet->pData;
		if (i->ErrorCode != (short)ERROR_CODE::NONE)
		{
			// 로그인 실패
			OutputDebugString(L"[LoginData] 로그인 실패!\n");
		}
		else
		{
			// 성공
			OutputDebugString(L"[LoginData] 로그인 성공!\n");
			m_IsLoginSuccessed = true;
		}
		m_RecvQueue.pop_front();
	}

	void LoginData::SetSubscribe(std::shared_ptr<PacketDistributer> publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOGIN_IN_RES, &m_RecvQueue);
	}

	void LobbyListData::Update()
	{
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		auto packet = m_RecvQueue.front();

		if (packet->PacketId != (short)PACKET_ID::LOBBY_LIST_RES)
		{
			OutputDebugString(L"Invalid Packet Receive! (In LobbyListData Update)\n");
			return;
		}

		auto pLobbyListData = (PktLobbyListRes*)packet->pData;
		if (pLobbyListData->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LobbyListData] 로비리스트 수령 실패!\n");
		}
		else
		{
			OutputDebugString(L"[LobbyListData] 로비리스트 성공적으로 수령\n");
			LoadData(pLobbyListData);
		}
		m_RecvQueue.pop_front();
	}
	

	void LobbyListData::SetSubscribe(std::shared_ptr<PacketDistributer> publisher)
	{
		publisher->Subscribe((short)PACKET_ID::LOBBY_LIST_RES, &m_RecvQueue);
	}

	const LobbyListInfo * LobbyListData::GetLobbyListInfo() const
	{
		const LobbyListInfo* const listLobbyConstPointer = m_LobbyList;
		return listLobbyConstPointer;
	}

	void LobbyListData::LoadData(PktLobbyListRes* pLobbyListData)
	{
		m_LobbyCount = pLobbyListData->LobbyCount;
		
		for (int i = 0; i < m_LobbyCount; ++i)
		{
			m_LobbyList[i] = pLobbyListData->LobbyList[i];
		}
		m_IsListLoaded = true;
	}
}