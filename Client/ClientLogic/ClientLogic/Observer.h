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