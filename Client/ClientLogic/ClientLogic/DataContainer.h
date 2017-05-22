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
#include "LoginData.h"
#include "LobbyListData.h"
#include "RoomListData.h"

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

		// 데이터 컨테이너 관련 함수.
		void SendChatToRoomList(std::wstring id, std::wstring chatMsg);
		std::wstring RequestMsgFromRoomList();

		// 정보 관련 함수.
		std::shared_ptr<LoginData> GetLoginData() const;
		std::shared_ptr<LobbyListData> GetLobbyListData() const;
		std::shared_ptr<RoomListData> GetRoomListData() const;

	private :

		// Queue 등록 함수.
		void RegisterQueueToProcessor();

		std::unique_ptr<PacketMessenger> m_pPacketMessenger = nullptr;
		std::unique_ptr<PacketDistributer> m_pPacketDistributer = nullptr;
		std::shared_ptr<LoginData> m_pLoginData = nullptr;
		std::shared_ptr<LobbyListData> m_pLobbyListData = nullptr;
		std::shared_ptr<RoomListData> m_pRoomListData = nullptr;

		std::deque<std::shared_ptr<RecvPacketInfo>> m_PacketDeque;

	};
}