#pragma once

#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include <vector>
#include <string>
#include <stack>
#include <array>
#include <list>
#include <memory>
#include "InlineFrame.h"
#include "DataContainer.h"


/* 씬에서 사용할 공용 데이터 구조체. */
struct SceneData
{
	std::unique_ptr<ClientLogic::DataContainer> dataContainer;
	// 임시로 정의해 놓은 자신의 id.
	// 이 변수를 확장해서 myInfo객체로 만들던, 서버에서 다시 한 번 받아오던 그런식으로 할 생각.
	std::wstring id;
};

using MyApp = SceneManager<String, SceneData>;

struct RoomInfo
{
	std::wstring roomTitle;
	std::wstring buttonName;
	short roomIndex = 0;
	short roomUserCount = 0;
	bool isRoomInfoValid = false;
};

struct UserInfo
{
	std::wstring userName;
	int userIndex = -1;
	bool isUserInfoValid = false;
};