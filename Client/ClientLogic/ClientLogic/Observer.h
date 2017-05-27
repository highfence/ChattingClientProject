#pragma once
#include <Windows.h>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <functional>
#include "Packet.h"
#include "PacketProcessor.h"

namespace ClientLogic
{
	class Observer
	{
	public:
		Observer() = default;
		virtual ~Observer() = default;

		int GetVersion() const { return m_Version; };

	protected :
		void VersionUp() { ++m_Version; };
		
		int m_Version = 0;
	};
}