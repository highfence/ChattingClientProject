#include "stdafx.h"
#include "LobbyListScene.h"
#include <random>
#include <ctime>

const int lobbyMaxUserNumber = 300;
const int lobbyMaxNumber = 5;
const int roomNameWidth = 200;
const int roomUserWidth = 95;
const int infoWidth = 150;

void LobbyList::init()
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

void LobbyList::update()
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

void LobbyList::draw() const 
{
	DrawConnectAbleLobbyInfo();
}

void LobbyList::CheckRefreshNeeded()
{
	// TODO :: 타이머를 달아서 일정한 시간마다 Refresh를 자동으로 할 수 있도록 만들기.
}

void LobbyList::RefreshData()
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

bool LobbyList::IsMyDataNeedRefreshed()
{
	int lastestVersion = m_data->dataContainer->GetLobbyListData()->GetVersion();
	if (lastestVersion > m_LastDataVersion)
	{
		return true;
	}

	return false;
}

void LobbyList::DrawConnectAbleLobbyInfo() const
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

void LobbyList::MakeLobbys()
{
	for (int i = 0; i < 5; ++i)
	{
		std::wstring LobbyName = L"Lobby" + std::to_wstring(i);
		m_LobbyGui.add(LobbyName, GUIText::Create(LobbyName, roomNameWidth));
		
		std::wstring LobbyButtonName = L"LobbyButton" + std::to_wstring(i);
		m_LobbyGui.addln(LobbyButtonName, GUIButton::Create(L"ENTER"));
	}

	m_LobbyGui.add(L"divider", GUIHorizontalLine::Create());
	m_LobbyGui.horizontalLine(L"divider").style.color = Color(127);
	m_LobbyGui.addln(L"Refresher", GUIButton::Create(L"Refresh"));
}

void LobbyList::ExitScene()
{
	m_LobbyVector.clear();
}

void LobbyList::CheckButtonClicked()
{
	for (const auto i : m_LobbyVector)
	{
		if (m_LobbyGui.button(i->LobbyButtonName).pushed)
		{
			ReqLobbyEnter(i->LobbyId);
		}
	}

	if (m_LobbyGui.button(L"Refresher").pushed)
	{
		ReqLobbyInfo();
	}
}

void LobbyList::ReqLobbyInfo()
{
	/* 로비 정보 요청은 패킷 헤더만 보내도 됨. */
	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_LIST_REQ, 0, nullptr);
}

void LobbyList::ReqLobbyEnter(short lobbyId)
{
	std::wstring debugLabel = L"[Siv3D::Lobby] 요청 진입 로비 아이디 : " + std::to_wstring(lobbyId) + L"\n";
	OutputDebugString(debugLabel.c_str());

	PktLobbyEnterReq newLobbyEnterReq;
	newLobbyEnterReq.LobbyId = lobbyId;

	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_ENTER_REQ, sizeof(newLobbyEnterReq), (char*)&newLobbyEnterReq);
}
