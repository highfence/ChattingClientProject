#pragma once
#include <Windows.h>
#include <deque>
#include <memory>
#include <mutex>
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
		const LobbyListInfo* GetLobbyListInfo() const;
		
	private :

		void LoadData(PktLobbyListRes*);
		
		short m_LobbyCount = 0;
		LobbyListInfo m_LobbyList[MAX_LOBBY_LIST_COUNT];
	};
}