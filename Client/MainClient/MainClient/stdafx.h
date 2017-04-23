#pragma once

#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include <vector>
#include <string>
#include <stack>
#include <array>
#include "InlineFrame.h"
#include "DataContainer.h"


/* 씬에서 사용할 공용 데이터 구조체. */
struct SceneData
{
	int32 sceneNumber;
	ClientLogic::DataContainer dataContainer;
};

using MyApp = SceneManager<String, SceneData>;

struct RoomInfo
{
	std::wstring roomName;
	std::wstring buttonName;
};

struct UserInfo
{
	std::wstring userName;
};