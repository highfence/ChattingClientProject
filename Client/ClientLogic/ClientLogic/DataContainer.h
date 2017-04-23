#pragma once

#include <WinSock2.h>
//#include <WS2tcpip.h>
#include <stdlib.h>

#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <map>
#include <deque>
#include <vector>
#include <mutex>
#include <memory>
#include <thread>
#include <string>

#include "Packet.h"
#include "PacketID.h"
#include "ErrorCode.h"
#include "Util.h"
#include "DataContainer.h"
#include "Definition.h"
#include "Observer.h"

#include "PacketMessenger.h"
#include "PacketProcessor.h"

using namespace NCommon;

namespace ClientLogic
{

	class DataContainer
	{
	public :

		DataContainer() = default;
		~DataContainer() = default;

		// 초기화 & 해제 함수.
		void Init();
		void Release();

		// 업데이트
		void Update();

		// 네트워크 관련 함수.
		bool ConnectRequest();
		bool DisconnectRequest();
		bool SendRequest(const short, const short, char*);

		// 정보 관련 함수.
		std::shared_ptr<LoginData> GetLoginData() const;

	private :

		// Queue 등록 함수.
		void RegisterQueueToProcessor();

		std::shared_ptr<PacketMessenger> m_pPacketMessenger = nullptr;
		std::shared_ptr<PacketProcessor> m_pPacketProcessor = nullptr;
		std::shared_ptr<LoginData> m_pLoginData = nullptr;
		std::shared_ptr<LobbyListData> m_pLobbyListData = nullptr;

		std::deque<RecvPacketInfo*> m_PacketDeque;

	};
}