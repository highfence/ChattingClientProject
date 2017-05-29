#include <functional>
#include "Observer.h"
#include "LobbyListData.h"

namespace ClientLogic
{
	void LobbyListData::SetSubscribe(PacketDistributer* publisher)
	{
		// 패킷 처리 함수를 PacketFunctionMap에 건네준다. 
		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_LIST_RES,
			std::bind(&LobbyListData::LobbyListRes, this, std::placeholders::_1));

		publisher->Subscribe(
			(short)PACKET_ID::LOBBY_ENTER_RES,
			std::bind(&LobbyListData::LobbyEnterRes, this, std::placeholders::_1));
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

	void LobbyListData::LobbyListRes(std::shared_ptr<RecvPacketInfo> packet)
	{
#pragma region Load Function
		auto LoadData = [this](PktLobbyListRes* pLobbyListData)
		{
			m_LobbyCount = pLobbyListData->LobbyCount;

			for (int i = 0; i < m_LobbyCount; ++i)
			{
				m_LobbyList[i] = pLobbyListData->LobbyList[i];
			}
		};
#pragma endregion

		auto pLobbyListData = (PktLobbyListRes*)packet->pData;
		if (pLobbyListData->ErrorCode != (short)ERROR_CODE::NONE)
		{
			OutputDebugString(L"[LobbyListData] 로비리스트 수령 실패!\n");
		}
		else
		{
			OutputDebugString(L"[LobbyListData] 로비리스트 성공적으로 수령\n");
			// 로비리스트를 수령한 경우, m_LobbyList에 데이터를 넣어준다.
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
}