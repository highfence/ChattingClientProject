#include "Observer.h"
#include "LobbyListData.h"

namespace ClientLogic
{
	void LobbyListData::OnPacketReceive()
	{
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		auto packet = m_RecvQueue.front();

		auto packetProcessFunc = m_PacketFuncMap.find(packet->PacketId);

		if (packetProcessFunc == m_PacketFuncMap.end())
		{
			OutputDebugString(L"[LobbyListData] �������� ���� ��Ŷ�� ���۹���. \n");
		}
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
		/* ������ ��Ŷ�� ���ؼ��� PacketProcess�� ������ְ�, ������־�� ��. */
		publisher->Subscribe((short)PACKET_ID::LOBBY_LIST_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_RES, &m_RecvQueue);
		publisher->Subscribe((short)PACKET_ID::LOBBY_ENTER_USER_NTF, &m_RecvQueue);
		RegisterPacketProcess();
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

	void LobbyListData::LobbyListRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto pLobbyListData = (PktLobbyListRes*)packet->pData;
		if (pLobbyListData->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LobbyListData] �κ񸮽�Ʈ ���� ����!\n");
		}
		else
		{
			OutputDebugString(L"[LobbyListData] �κ񸮽�Ʈ ���������� ����\n");
			LoadData(pLobbyListData);
			VersionUp();
		}
	}

	void LobbyListData::LobbyEnterRes(std::shared_ptr<RecvPacketInfo> packet)
	{
		auto pLobbyEnterResData = (PktLobbyEnterRes*)packet->pData;
		if (pLobbyEnterResData->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LobbyListData] �κ� ���� ������ ���� ����!\n");
		}
		else
		{
			OutputDebugString(L"[LobbyListData] �κ� ���� ������ ���������� ����\n");
			m_IsLobbySuccesslyEntered = true;
			VersionUp();
		}
	}

	void LobbyListData::LobbyEnterUserNtf(std::shared_ptr<RecvPacketInfo> packet)
	{
		OutputDebugString(L"[LobbyListData] LOBBY_ENTER_USER_NTF ��Ŷ ����\n");
	}
}