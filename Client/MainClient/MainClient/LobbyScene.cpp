#include "stdafx.h"
#include "LobbyScene.h"
#include <random>
#include <ctime>

const int lobbyMaxUserNumber = 300;
const int lobbyMaxNumber = 5;
const int roomNameWidth = 200;
const int roomUserWidth = 95;
const int infoWidth = 150;

void Lobby::init()
{
	reqLobbyInfo();

	m_LobbyGui = GUI(GUIStyle::Default);
	m_LobbyGui.setTitle(L"Lobby");
	m_InfoGui = GUI(GUIStyle::Default);
	m_InfoGui.setTitle(L"My Info");

	/* Setting InfoGui Position & Background Color */
	Point infoGuiPos = Point(roomNameWidth + roomUserWidth + 160, 0);
	m_InfoGui.setPos(infoGuiPos);
	Graphics::SetBackground(Color(100, 160, 200));

	/* Drawing Lobby */
	makeLobbys();

	/* Drawing Info */
	m_InfoGui.addln(L"Info", GUIText::Create(L"Keun Won, Lee", infoWidth));
}

void Lobby::update()
{

	m_data->dataContainer->Update();

	auto data = ~~;
	if (data->GetHash() != lastDataHash)
	{
		applyData(data);
	}

	lobbyInfoSetting();

	/* if lobby entry button pushed */
	checkButtonClicked();
}

void Lobby::draw() const 
{

}

void Lobby::lobbyInfoSetting()
{
	static long long lastDataHash = 0;
	auto data = m_data->dataContainer->GetLobbyListData();


	if(lastDataHash)
	if (data->GetIsListLoaded())
	{
		
	}
}

void Lobby::applyData(ClientLogic::LobbyListData data)
{

}

void Lobby::makeLobbys() 
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

void Lobby::exitScene()
{
	m_LobbyVector.clear();
}

void Lobby::checkButtonClicked()
{
	for (const auto i : m_LobbyVector)
	{
		if (m_LobbyGui.button(i->LobbyButtonName).pushed)
		{
			changeScene(L"RoomList");
		}
	}
}

void Lobby::reqLobbyInfo()
{
	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_LIST_REQ, sizeof(PktHeader), nullptr);
}
