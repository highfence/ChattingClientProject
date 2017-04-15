#pragma once
#include <random>
#include <ctime>
#include <vector>
#include <string>

const int lobbyMaxUserNumber = 300;
const int lobbyMaxNumber = 5;

struct LobbyInfo
{
	std::wstring LobbyName;
	std::wstring LobbyId;
	std::wstring LobbyButtonName;
	int UserNumber = 0;
	int MaxUserNumber = 0;
};


struct Lobby : MyApp::Scene 
{
	GUI m_LobbyGui;
	GUI m_InfoGui;
	std::vector<LobbyInfo*> m_LobbyVector;

	const int roomNameWidth = 200;
	const int roomUserWidth = 95;
	const int infoWidth = 150;

	/* Required Initializer for Siv3D */
	void init() override
	{
		m_LobbyGui = GUI(GUIStyle::Default);
		m_LobbyGui.setTitle(L"Lobby");
		m_InfoGui = GUI(GUIStyle::Default);
		m_InfoGui.setTitle(L"My Info");

		/* Set Lobby infomations */
		lobbyInfoSetting();

		/* Setting InfoGui Position & Background Color */
		Point infoGuiPos = Point(roomNameWidth + roomUserWidth + 160, 0);
		m_InfoGui.setPos(infoGuiPos);
		Graphics::SetBackground(Color(100, 160, 200));

		/* Drawing Lobby */
		makeLobbys();

		/* Drawing Info */
		m_InfoGui.addln(L"Info", GUIText::Create(L"Keun Won, Lee", infoWidth));
	}

	/* Required Function for Siv3d */
	void update() override
	{
		/* if lobby entry button pushed */
		if (m_LobbyGui.button(L"ENTER1").pushed)
		{
			/* changeScene to chattingRoom */
			exitScene();
			changeScene(L"ChattingRoom");
		}
	}

	/* Required Function for Siv3d */
	void draw() const override
	{
		/* Empty */
	}

	/* Insert Lobby Info to vector */
	void lobbyInfoSetting()
	{
		// TODO :: data of Lobby & info window will be given by server
		/* Temporarily Data input */
		std::mt19937 rng((unsigned int)time(NULL));
		std::uniform_int_distribution<int> lobbyNumberRange(1, lobbyMaxNumber);
		std::uniform_int_distribution<int> userNumberRange(0, lobbyMaxUserNumber);

		/* Make Lobby infos */
		int lobbyNumber = lobbyNumberRange(rng);
		for (int i = 0; i < lobbyNumber; ++i)
		{
			LobbyInfo* newLobbyInfo = new LobbyInfo;
			newLobbyInfo->LobbyName = L"Chatting Server " + std::to_wstring(i + 1);
			newLobbyInfo->LobbyButtonName = L"Button" + std::to_wstring(i + 1);
			newLobbyInfo->LobbyId = L"Lobby" + std::to_wstring(i + 1);
			newLobbyInfo->MaxUserNumber = lobbyMaxUserNumber;
			newLobbyInfo->UserNumber = userNumberRange(rng);
			m_LobbyVector.push_back(newLobbyInfo);
		}
	}

	/* Make text field accord with set lobby info */
	void makeLobbys()
	{
		int idx = 1;

		for (auto i : m_LobbyVector)
		{
			std::wstring userNumberInfo = std::to_wstring(i->UserNumber) + L" / " + std::to_wstring(i->MaxUserNumber);
			m_LobbyGui.add(i->LobbyId, GUIText::Create(i->LobbyName, roomNameWidth));
			m_LobbyGui.add(L"userNumberInfo", GUIText::Create(userNumberInfo, roomUserWidth));
			m_LobbyGui.addln(i->LobbyButtonName, GUIButton::Create(L"ENTER"));

			++idx;
		}
	}

	/* Fuction which must be called when the scene changed */
	void exitScene()
	{
		m_LobbyVector.clear();
	}

};