#include "Observer.h"
#include "LobbyListData.h"

namespace ClientLogic
{
	void LobbyListData::OnPacketReceive()
	{
		/* ���� ť�� ����ٸ� ���ʸ� �ѱ��. */
		if (m_RecvQueue.empty())
		{
			Sleep(0);
			return;
		}

		/* ť�� ��Ŷ�� �ִٸ� packet�� �޴´�. */
		auto packet = m_RecvQueue.front();

		/* ������ ��Ŷ ���̵�� unorded_map ���� �����Ǵ� �Լ��� ã�´�. */
		auto packetProcessFunc = m_PacketFuncMap.find(packet->PacketId);

		/* �����Ǵ� �Լ��� ���� ���. */
		if (packetProcessFunc == m_PacketFuncMap.end())
		{
			OutputDebugString(L"[LobbyListData] �������� ���� ��Ŷ�� ���۹���. \n");
		}
		/* �����Ǵ� �Լ��� �ִ� ���. */
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

		/* ������ ��Ŷ ���̵� �����Ǵ� Function���� ������ �־���Ѵ�. */
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