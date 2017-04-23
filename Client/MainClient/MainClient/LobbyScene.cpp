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
	ReqLobbyInfo();

	m_LobbyGui = GUI(GUIStyle::Default);
	m_LobbyGui.setTitle(L"Lobby");
	m_InfoGui = GUI(GUIStyle::Default);
	m_InfoGui.setTitle(L"My Info");

	/* Setting InfoGui Position & Background Color */
	Point infoGuiPos = Point(roomNameWidth + roomUserWidth + 160, 0);
	m_InfoGui.setPos(infoGuiPos);
	Graphics::SetBackground(Color(100, 160, 200));

	/* Drawing Info */
	m_InfoGui.addln(L"Info", GUIText::Create(L"Keun Won, Lee", infoWidth));

	MakeLobbys();
}

void Lobby::update()
{
	m_data->dataContainer->Update();

	if (IsMyDataNeedRefreshed())
	{
		RefreshData();
	}

	/* if lobby entry button pushed */
	CheckButtonClicked();
}

void Lobby::draw() const 
{
}

void Lobby::RefreshData()
{
	OutputDebugString(L"[Siv3D::Lobby] 로비 데이터 리프레시");
	m_LobbyVector.clear();

	int lobbyNumber = m_data->dataContainer->GetLobbyListData()->GetLobbyCount();

	for (int i = 0; i < lobbyNumber; ++i)
	{
		LobbyInfo* newLobbyInfo = new LobbyInfo;
		newLobbyInfo->LobbyId = m_data->dataContainer->GetLobbyListData()->GetLobbyListInfo()->LobbyId;
		newLobbyInfo->LobbyUser = m_data->dataContainer->GetLobbyListData()->GetLobbyListInfo()->LobbyUserCount;
		newLobbyInfo->LobbyButtonName = L"LobbyButton" + std::to_wstring(i);
		m_LobbyVector.emplace_back(std::move(newLobbyInfo));
	}
}

bool Lobby::IsMyDataNeedRefreshed()
{
	int lastestVersion = m_data->dataContainer->GetLobbyListData()->GetVersion();
	if (lastestVersion > m_LastDataVersion)
	{
		return true;
	}

	return false;
}

void Lobby::MakeLobbys()
{
	for (int i = 0; i < 5; ++i)
	{
		std::wstring LobbyName = L"Lobby" + std::to_wstring(i + 1);
		m_LobbyGui.add(LobbyName, GUIText::Create(LobbyName, roomNameWidth));
		
		std::wstring LobbyButtonName = L"LobbyButton" + std::to_wstring(i + 1);
		m_LobbyGui.addln(LobbyButtonName, GUIButton::Create(L"ENTER", false));
	}
}

void Lobby::ExitScene()
{
	m_LobbyVector.clear();
}

void Lobby::CheckButtonClicked()
{
	for (const auto i : m_LobbyVector)
	{
		if (m_LobbyGui.button(i->LobbyButtonName).pushed)
		{
			// TODO :: 여기서 룸 진입한다는 패킷을 보내야겠지?
			changeScene(L"RoomList");
		}
	}
}

void Lobby::ReqLobbyInfo()
{
	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_LIST_REQ, sizeof(PktHeader), nullptr);
}
