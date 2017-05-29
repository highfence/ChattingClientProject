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
#include "RoomData.h"

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
		bool SendRequest(
			const short packetId,
			const short packetSize,
			char* pData);

		// 데이터 컨테이너 관련 함수.
		void SendChatToRoomList(std::wstring id, std::wstring chatMsg);
		std::wstring RequestMsgFromRoomList();

		// 정보 관련 함수.
		LoginData * GetLoginData() const;
		LobbyListData * GetLobbyListData() const;
		RoomListData * GetRoomListData() const;
		RoomData * GetRoomData() const;


	private :

		// Queue 등록 함수.
		void RegisterQueueToProcessor();

		std::unique_ptr<PacketMessenger> m_pPacketMessenger;
		std::unique_ptr<PacketDistributer> m_pPacketDistributer;
		std::unique_ptr<LoginData> m_pLoginData;
		std::unique_ptr<LobbyListData> m_pLobbyListData;
		std::unique_ptr<RoomListData> m_pRoomListData;
		std::unique_ptr<RoomData> m_pRoomData;

		std::deque<std::shared_ptr<RecvPacketInfo>> m_PacketDeque;

	};
}