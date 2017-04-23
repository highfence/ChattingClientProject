#include "DataContainer.h"
#include "PacketMessenger.h"
#include "PacketProcessor.h"

namespace DataContainer
{
	void DataContainer::Init()
	{
		m_pPacketMessenger = std::make_shared<PacketMessenger>();
		m_pPacketMessenger->Init();
		m_pPacketProcessor = std::make_shared<PacketProcessor>();
		m_pLoginData = std::make_shared<LoginData>();

		RegisterQueueToProcessor();
		m_pLoginData->Subscribe(m_pPacketProcessor);
	}

	void DataContainer::Release()
	{
		
	}

	void DataContainer::Update()
	{
		m_pPacketProcessor->Update();
		m_pLoginData->Update();
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

	void DataContainer::RegisterQueueToProcessor()
	{
		m_pPacketProcessor->RegisterMessenger(m_pPacketMessenger);
	}

}
