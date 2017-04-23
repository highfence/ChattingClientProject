#pragma once
#include <Windows.h>
#include <deque>
#include <memory>
#include <mutex>
#include "Packet.h"
#include "PacketProcessor.h"

// TODO :: Observer는 OnReceive() 가상 함수만을 가진 인터페이스로 구현해보자. (다음주 25, 26, 27)
// I~able로 구현된 것이 많음.

namespace ClientLogic
{
	class PacketDistributer;

	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = default;

		virtual void Update() = 0;
		virtual void Subscribe(short, std::shared_ptr<PacketDistributer>);

	protected :
		std::deque<std::shared_ptr<RecvPacketInfo>> m_RecvQueue;
		std::mutex m_Mutex;
	};

	class LoginData : public Observer
	{
	public :
		LoginData() = default;
		~LoginData() = default;

		void Update() override;
		void SetSubscribe(std::shared_ptr<PacketDistributer>);

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
		void SetSubscribe(std::shared_ptr<PacketDistributer>);
		
		void RequestDataCompletlyDelivered() { m_IsListLoaded = false; };
		bool GetIsListLoaded() const { return m_IsListLoaded; };
		short GetLobbyCount() const { return m_LobbyCount; };
		const LobbyListInfo* GetLobbyListInfo() const;
		long long GetHash();
		
	private :

		void LoadData(PktLobbyListRes*);
		
		short m_LobbyCount = 0;
		LobbyListInfo m_LobbyList[MAX_LOBBY_LIST_COUNT];
		bool m_IsListLoaded = false;
	};
}