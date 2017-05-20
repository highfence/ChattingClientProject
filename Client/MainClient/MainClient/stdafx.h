#pragma once

#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include <vector>
#include <string>
#include <stack>
#include <array>
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
	std::wstring roomName;
	std::wstring buttonName;
};

struct UserInfo
{
	std::wstring userName;
};