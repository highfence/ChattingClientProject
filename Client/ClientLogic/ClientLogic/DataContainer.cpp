#include "DataContainer.h"

#pragma comment(lib, "Ws2_32.lib")

namespace ClientLogic
{
	void DataContainer::Init()
	{
		m_pPacketMessenger = std::make_shared<PacketMessenger>();
		m_pPacketMessenger->Init();
		m_pPacketProcessor = std::make_shared<PacketDistributer>();
		m_pLoginData = std::make_shared<LoginData>();
		m_pLoginData->SetSubscribe(m_pPacketProcessor);
		m_pLobbyListData = std::make_shared<LobbyListData>();
		m_pLobbyListData->SetSubscribe(m_pPacketProcessor);

		RegisterQueueToProcessor();
	}

	void DataContainer::Release()
	{
		
	}

	void DataContainer::Update()
	{
		m_pPacketProcessor->Update();
		m_pLoginData->Update();
		m_pLobbyListData->Update();
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

	std::shared_ptr<LoginData> DataContainer::GetLoginData() const
	{
		return m_pLoginData;
	}

	std::shared_ptr<LobbyListData> DataContainer::GetLobbyListData() const
	{
		return m_pLobbyListData;
	}

	void DataContainer::RegisterQueueToProcessor()
	{
		m_pPacketProcessor->RegisterMessenger(m_pPacketMessenger);
	}

}
