#pragma once
#include <array>

namespace ClientLogic
{
	class LobbyListData : public Observer
	{
	public:
		LobbyListData() = default;
		~LobbyListData() = default;

		void SetSubscribe(PacketDistributer* publisher);

		short GetLobbyCount() const { return m_LobbyCount; };
		const LobbyListInfo* GetLobbyListInfo(const int listIdx) const;
		bool GetIsLobbySuccesslyEntered();

	private:

		// 패킷을 처리할 함수들.
		void LobbyListRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyEnterRes(std::shared_ptr<RecvPacketInfo> packet);

		short m_LobbyCount = 0;
		std::array<LobbyListInfo, MAX_LOBBY_LIST_COUNT> m_LobbyList;
		bool m_IsLobbySuccesslyEntered = false;
	};
}