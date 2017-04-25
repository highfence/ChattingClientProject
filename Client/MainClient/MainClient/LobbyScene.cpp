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
	std::wstring myId = L"ID : " + m_data->id;
	m_InfoGui.addln(L"Info", GUIText::Create(myId, infoWidth));

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

	// TODO :: 룸리스트 받기.
	if (m_data->dataContainer->GetLobbyListData()->GetIsLobbySuccesslyEntered())
	{
		changeScene(L"RoomList");
	}
}

void Lobby::draw() const 
{
	DrawConnectAbleLobbyInfo();
}

void Lobby::RefreshData()
{
	OutputDebugString(L"[Lobby] 데이터 리프레시 \n");
	m_LobbyVector.clear();

	m_AbleLobbyNumber = m_data->dataContainer->GetLobbyListData()->GetLobbyCount();

	for (int i = 0; i < m_AbleLobbyNumber; ++i)
	{
		LobbyInfo* newLobbyInfo = new LobbyInfo;
		newLobbyInfo->LobbyId = m_data->dataContainer->GetLobbyListData()->GetLobbyListInfo(i)->LobbyId;
		newLobbyInfo->LobbyUser = m_data->dataContainer->GetLobbyListData()->GetLobbyListInfo(i)->LobbyUserCount;
		newLobbyInfo->LobbyButtonName = L"LobbyButton" + std::to_wstring(i);
		m_LobbyVector.emplace_back(std::move(newLobbyInfo));
	}
	m_LastDataVersion = m_data->dataContainer->GetLobbyListData()->GetVersion();
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

void Lobby::DrawConnectAbleLobbyInfo() const
{
	std::wstring ableLobbyInfo = L"현재 접속가능한 로비 : " + std::to_wstring(m_AbleLobbyNumber);
	m_Font(ableLobbyInfo).draw(40, 250);

	int idx = 0;
	for (auto& i : m_LobbyVector)
	{
		std::wstring lobbyUser = L"  유저 수 : " + std::to_wstring(i->LobbyUser) + L" / 50";
		std::wstring lobbyName = L"Lobby" + std::to_wstring(idx);
		m_LobbyGui.text(lobbyName).text = lobbyName + lobbyUser;
		++idx;
	}
}

void Lobby::MakeLobbys()
{
	for (int i = 0; i < 5; ++i)
	{
		std::wstring LobbyName = L"Lobby" + std::to_wstring(i);
		m_LobbyGui.add(LobbyName, GUIText::Create(LobbyName, roomNameWidth));
		
		std::wstring LobbyButtonName = L"LobbyButton" + std::to_wstring(i);
		m_LobbyGui.addln(LobbyButtonName, GUIButton::Create(L"ENTER"));
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
			ReqLobbyEnter(i->LobbyId);
		}
	}
}

void Lobby::ReqLobbyInfo()
{
	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_LIST_REQ, sizeof(PktHeader), nullptr);
}

void Lobby::ReqLobbyEnter(short lobbyId)
{
	std::wstring debugLabel = L"[Siv3D::Lobby] 요청 진입 로비 아이디 : " + std::to_wstring(lobbyId) + L"\n";
	OutputDebugString(debugLabel.c_str());

	PktLobbyEnterReq newLobbyEnterReq;
	newLobbyEnterReq.LobbyId = lobbyId;

	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_ENTER_REQ, sizeof(newLobbyEnterReq), (char*)&newLobbyEnterReq);
}
