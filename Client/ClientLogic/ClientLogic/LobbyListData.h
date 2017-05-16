#pragma once

namespace ClientLogic
{
	class LobbyListData : public Observer
	{
	public:
		LobbyListData() = default;
		~LobbyListData() = default;

		void OnPacketReceive() override;
		void SetSubscribe(PacketDistributer*);

		short GetLobbyCount() const { return m_LobbyCount; };
		const LobbyListInfo* GetLobbyListInfo(const int) const;
		bool GetIsLobbySuccesslyEntered() const { return m_IsLobbySuccesslyEntered; };

	private:

		void LoadData(PktLobbyListRes*);

		short m_LobbyCount = 0;
		LobbyListInfo m_LobbyList[MAX_LOBBY_LIST_COUNT];
		bool m_IsLobbySuccesslyEntered = false;
	};
}