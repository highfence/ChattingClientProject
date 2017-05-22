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

		// �ʱ�ȭ & ���� �Լ�.
		void Init();
		void Release();

		// ������Ʈ
		void Update();

		// ��Ʈ��ũ ���� �Լ�.
		bool ConnectRequest();
		bool DisconnectRequest();
		bool SendRequest(const short, const short, char*);

		// ������ �����̳� ���� �Լ�.
		void SendChatToRoomList(std::wstring id, std::wstring chatMsg);
		std::wstring RequestMsgFromRoomList();

		// ���� ���� �Լ�.
		std::shared_ptr<LoginData> GetLoginData() const;
		std::shared_ptr<LobbyListData> GetLobbyListData() const;
		std::shared_ptr<RoomListData> GetRoomListData() const;

	private :

		// Queue ��� �Լ�.
		void RegisterQueueToProcessor();

		std::unique_ptr<PacketMessenger> m_pPacketMessenger = nullptr;
		std::unique_ptr<PacketDistributer> m_pPacketDistributer = nullptr;
		std::shared_ptr<LoginData> m_pLoginData = nullptr;
		std::shared_ptr<LobbyListData> m_pLobbyListData = nullptr;
		std::shared_ptr<RoomListData> m_pRoomListData = nullptr;

		std::deque<std::shared_ptr<RecvPacketInfo>> m_PacketDeque;

	};
}