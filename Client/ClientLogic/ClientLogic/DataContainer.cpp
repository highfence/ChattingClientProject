#include "DataContainer.h"

#pragma comment(lib, "Ws2_32.lib")

namespace ClientLogic
{
	void DataContainer::Init()
	{
		m_pPacketMessenger = std::make_unique<PacketMessenger>();
		m_pPacketMessenger->Init();
		m_pPacketDistributer = std::make_unique<PacketDistributer>();

		m_pLoginData = std::make_shared<LoginData>();
		m_pLoginData->SetSubscribe(m_pPacketDistributer.get());
		m_pLobbyListData = std::make_shared<LobbyListData>();
		m_pLobbyListData->SetSubscribe(m_pPacketDistributer.get());
		m_pRoomListData = std::make_shared<RoomListData>();
		m_pRoomListData->SetSubscribe(m_pPacketDistributer.get());

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

	void DataContainer::SendChatToRoomList(std::wstring id, std::wstring chatMsg)
	{
		m_pRoomListData->PushChatData(id, chatMsg);
	}

	std::wstring DataContainer::RequestMsgFromRoomList()
	{
		return m_pRoomListData->GetDataFromChatQueue();
	}

	std::shared_ptr<LoginData> DataContainer::GetLoginData() const
	{
		return m_pLoginData;
	}

	std::shared_ptr<LobbyListData> DataContainer::GetLobbyListData() const
	{
		return m_pLobbyListData;
	}

	std::shared_ptr<RoomListData> DataContainer::GetRoomListData() const
	{
		return m_pRoomListData;
	}

	void DataContainer::RegisterQueueToProcessor()
	{
		m_pPacketDistributer->RegisterMessenger(m_pPacketMessenger.get());
	}

}
