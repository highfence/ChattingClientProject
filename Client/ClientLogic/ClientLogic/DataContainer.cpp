#include "DataContainer.h"
#include "PacketMessenger.h"
#include "PacketProcessor.h"

namespace DataContainer
{
	void DataContainer::Init()
	{
		m_PacketMessenger = std::make_unique<PacketMessenger>();
		m_PacketProcessor = std::make_unique<PacketProcessor>();
	}

	void DataContainer::Release()
	{
		
	}

	bool DataContainer::ConnectRequest()
	{
		return m_PacketMessenger->Connect();
	}

	bool DataContainer::DisconnectRequest()
	{
		return m_PacketMessenger->Disconnect();
	}

}
