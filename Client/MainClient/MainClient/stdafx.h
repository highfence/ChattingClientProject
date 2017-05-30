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


/* ������ ����� ���� ������ ����ü. */
struct SceneData
{
	std::unique_ptr<ClientLogic::DataContainer> dataContainer;
	// �ӽ÷� ������ ���� �ڽ��� id.
	// �� ������ Ȯ���ؼ� myInfo��ü�� �����, �������� �ٽ� �� �� �޾ƿ��� �׷������� �� ����.
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