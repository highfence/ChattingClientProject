#pragma once

#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include <vector>
#include <string>
#include <stack>
#include <array>
#include "InlineFrame.h"
#include "Network.h"


/* ������ ����� ���� ������ ����ü. */
struct SceneData
{
	int32 sceneNumber;
};

using MyApp = SceneManager<String, SceneData>;

static ClientLogic::ClientNetwork* clientNetwork;
static ClientLogic::PacketProcess* packetProcess;

struct RoomInfo
{
	std::wstring roomName;
	std::wstring buttonName;
};

struct UserInfo
{
	std::wstring userName;
};