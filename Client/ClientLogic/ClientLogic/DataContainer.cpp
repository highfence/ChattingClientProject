#include "DataContainer.h"
#include "PacketMessenger.h"
#include "PacketProcessor.h"

namespace DataContainer
{
	void DataContainer::Init()
	{
		m_pPacketMessenger = std::make_shared<PacketMessenger>();
		m_pPacketProcessor = std::make_shared<PacketProcessor>();
		m_pLoginData = std::make_shared<LoginData>();

		m_pLoginData->Subscribe(m_pPacketProcessor);
	}

	void DataContainer::Release()
	{
		
	}

	bool DataContainer::ConnectRequest()
	{
		return m_pPacketMessenger->Connect();
	}

	bool DataContainer::DisconnectRequest()
	{
		return m_pPacketMessenger->Disconnect();
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
