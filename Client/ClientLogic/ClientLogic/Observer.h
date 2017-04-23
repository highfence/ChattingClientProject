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
	class PacketProcessor;

	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = default;

		virtual void Update() = 0;
		virtual void Subscribe(short, std::shared_ptr<PacketProcessor>);

	protected :
		std::deque<RecvPacketInfo*> m_RecvQueue;
		std::mutex m_Mutex;
	};

	class LoginData : public Observer
	{
	public :
		LoginData() = default;
		~LoginData() = default;

		void Update() override;
		void SetSubscribe(std::shared_ptr<PacketProcessor>);

		bool GetLoginSuccessed() const { return m_IsLoginSuccessed; };

	private :

		bool m_IsLoginSuccessed = false;
	};

	//class LobbyListData : public Observer
	//{
	//public :
	//	LobbyListData() = default;
	//	~LobbyListData() = default;

	//	void Update() override;
	//	
	//private :
	//	

	//};
}