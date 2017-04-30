#pragma once
#include <Windows.h>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include "Packet.h"
#include "PacketProcessor.h"

// TODO :: Observer�� OnReceive() ���� �Լ����� ���� �������̽��� �����غ���. (������ 25, 26, 27)
// I~able�� ������ ���� ����.

namespace ClientLogic
{
	class PacketDistributer;

	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = default;

		virtual void Update() = 0;
		virtual void Subscribe(short, PacketDistributer* processor);
		int GetVersion() const { return m_Version; };

	protected :

		void VersionUp() { ++m_Version; };
		
		std::deque<std::shared_ptr<RecvPacketInfo>> m_RecvQueue;
		std::mutex m_Mutex;
		int m_Version = 0;
	};

	class LoginData : public Observer
	{
	public :
		LoginData() = default;
		~LoginData() = default;

		void Update() override;
		void SetSubscribe(PacketDistributer* publisher);

		bool GetLoginSuccessed() const { return m_IsLoginSuccessed; };

	private :

		bool m_IsLoginSuccessed = false;
	};

	class LobbyListData : public Observer
	{
	public :
		LobbyListData() = default;
		~LobbyListData() = default;

		void Update() override;
		void SetSubscribe(PacketDistributer*);
		
		short GetLobbyCount() const { return m_LobbyCount; };
		const LobbyListInfo* GetLobbyListInfo(const int) const;
		bool GetIsLobbySuccesslyEntered() const { return m_IsLobbySuccesslyEntered; };
		
	private :

		void LoadData(PktLobbyListRes*);
		
		short m_LobbyCount = 0;
		LobbyListInfo m_LobbyList[MAX_LOBBY_LIST_COUNT];
		bool m_IsLobbySuccesslyEntered = false;
	};

	class RoomListData : public Observer
	{
	public :
		RoomListData() = default;
		~RoomListData() = default;

		void Update() override;
		void SetSubscribe(PacketDistributer*);

		bool GetIsRequestNeeded() const { return m_IsRequestNeeded; };
		bool GetIsChatDelivered();
		int GetReceivedLastestUserId() const { return m_ReceivedLastestUserId; };
		std::wstring GetChatFromQueue();
		std::vector<std::pair<int, std::wstring>>& GetUserInfoVector() { return m_UserInfoVector; };

	private :

		std::vector<std::pair<int, std::wstring>> m_UserInfoVector;
		std::deque<std::wstring> m_ChatQueue;
		bool m_IsRequestNeeded = false;
		bool m_IsChatDelivered = false;
		int m_ReceivedLastestUserId = 0;
	};
}