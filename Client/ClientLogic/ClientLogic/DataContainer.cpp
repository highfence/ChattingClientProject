#include "DataContainer.h"

#pragma comment(lib, "Ws2_32.lib")

namespace ClientLogic
{
	void DataContainer::Init()
	{
		m_pPacketMessenger = std::make_unique<PacketMessenger>();
		m_pPacketMessenger->Init();
		m_pPacketDistributer = std::make_unique<PacketDistributer>();

		m_pLoginData = std::make_unique<LoginData>();
		m_pLoginData->SetSubscribe(m_pPacketDistributer.get());
		m_pLobbyListData = std::make_unique<LobbyListData>();
		m_pLobbyListData->SetSubscribe(m_pPacketDistributer.get());
		m_pRoomListData = std::make_unique<RoomListData>();
		m_pRoomListData->SetSubscribe(m_pPacketDistributer.get());
		m_pRoomData = std::make_unique<RoomData>();
		m_pRoomData->SetSubscribe(m_pPacketDistributer.get());

		RegisterQueueToProcessor();
	}

	void DataContainer::Release()
	{
		
	}

	void DataContainer::Update()
	{
		m_pPacketDistributer->Update();
	}

	bool DataContainer::ConnectRequest()
	{
		return m_pPacketMessenger->Connect();
	}

	bool DataContainer::DisconnectRequest()
	{
		return m_pPacketMessenger->Disconnect();
	}

	bool DataContainer::SendRequest(const short packetId, const short packetSize, char * pData)
	{
		return m_pPacketMessenger->Send(packetId, packetSize, pData);
	}

	std::wstring DataContainer::RequestMsgFromRoomList()
	{
		return m_pRoomListData->GetChatDataFromQueue();
	}

	LoginData * DataContainer::GetLoginData() const
	{
		return m_pLoginData.get();
	}

	LobbyListData * DataContainer::GetLobbyListData() const
	{
		return m_pLobbyListData.get();
	}

	RoomListData * DataContainer::GetRoomListData() const
	{
		return m_pRoomListData.get();
	}

	RoomData * DataContainer::GetRoomData() const
	{
		return m_pRoomData.get();
	}

	void DataContainer::RegisterQueueToProcessor()
	{
		m_pPacketDistributer->RegisterMessenger(m_pPacketMessenger.get());
	}

}
