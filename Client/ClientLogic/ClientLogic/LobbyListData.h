#pragma once

namespace ClientLogic
{
	class LobbyListData : public Observer
	{
	public:
		LobbyListData() = default;
		~LobbyListData() = default;

		void OnPacketReceive() override;
		void RegisterPacketProcess() override;
		void SetSubscribe(PacketDistributer* publisher);

		short GetLobbyCount() const { return m_LobbyCount; };
		const LobbyListInfo* GetLobbyListInfo(const int listIdx) const;
		bool GetIsLobbySuccesslyEntered();

	private:

		void LoadData(PktLobbyListRes* pLobbyListData);
		void LobbyListRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyEnterRes(std::shared_ptr<RecvPacketInfo> packet);
		void LobbyEnterUserNtf(std::shared_ptr<RecvPacketInfo> packet);

		short m_LobbyCount = 0;
		LobbyListInfo m_LobbyList[MAX_LOBBY_LIST_COUNT];
		bool m_IsLobbySuccesslyEntered = false;
	};
}