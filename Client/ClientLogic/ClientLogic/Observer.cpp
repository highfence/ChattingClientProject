#include "Observer.h"
#include "Util.h"
#include <stdlib.h>

namespace ClientLogic
{
	void Observer::Subscribe(short packetId, PacketDistributer* processor)
	{
		processor->Subscribe(packetId, &m_RecvQueue);
	}
}