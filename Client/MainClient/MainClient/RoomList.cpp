#include "stdafx.h"
#include "RoomList.h"
#include "TextScrollBox.h"

const int roomWidth = 250;
const int buttonWidth = 50;
const int userInfoWidth = 200;
const int chattingInfoHeight = 275;
	const int maxUserInfoNumber = 5;

void RoomList::init()
{
	RequestUserInfo();

	m_RoomListGui = GUI(GUIStyle::Default);
	m_RoomListGui.setTitle(L"Room List");

	m_UserListGui = GUI(GUIStyle::Default);
	m_UserListGui.setTitle(L"User List");
	Point userGuiPos = Point(roomWidth + buttonWidth + 110, 0);
	m_UserListGui.setPos(userGuiPos);

	m_ChattingGui = GUI(GUIStyle::Default);
	Point chattingGuiPos = Point(0, chattingInfoHeight);
	m_ChattingGui.setPos(chattingGuiPos);

	Graphics::SetBackground(Color(200, 160, 100));

	/* Data Setting */
	RoomInfoSetting();

	/* Input data in Gui */
	MakeRooms();
	MakeUsers();
	MakeChattingGui();
}

void RoomList::update()
{
	m_data->dataContainer->Update();

	CheckSendClicked();
	CheckRoomClicked();
	CheckDataUpdated();
}

void RoomList::draw() const
{
}

void RoomList::Refresh()
{

}

void RoomList::RoomInfoSetting()
{
	const int roomInfoNum = 6;
	const std::wstring roomName = L"Room ";
	// TODO :: room info must be distributed by server
	for (int i = 0; i < roomInfoNum; ++i)
	{
		RoomInfo* newRoomInfo = new RoomInfo;
		newRoomInfo->roomName = roomName + std::to_wstring(i + 1);
		std::wstring buttonName = L"Button" + std::to_wstring(i + 1);
		newRoomInfo->buttonName = buttonName;

		m_RoomInfoVector.push_back(newRoomInfo);
	}
}

void RoomList::MakeRooms()
{
	for (const auto& i : m_RoomInfoVector)
	{
		/* Make Rooms */
		m_RoomListGui.add(i->roomName, GUIText::Create(i->roomName, roomWidth));
		
		/* Make Buttons */	
		m_RoomListGui.addln(i->buttonName, GUIButton::Create(L"ENTER", buttonWidth));
	}
}

void RoomList::MakeUsers()
{
	for (int i = 0; i < maxUserInfoNumber; ++i)
	{
		/* Make Users */
		m_UserListGui.addln(std::to_wstring(i), GUIText::Create(L"NONE", userInfoWidth));
	}
}

void RoomList::MakeChattingGui()
{
	/* Add Chatting Window */
	m_ChattingGui.addln(L"ChattingWindow", GUITextArea::Create(5, 33));
	//m_pChatTextBox = new TextScorllBox(Rect(30, chattingInfoHeight, 585, 130));

	/* Add Divide Line */
	m_ChattingGui.add(L"Divider", GUIHorizontalLine::Create(1));
	m_ChattingGui.horizontalLine(L"Divider").style.color = Color(127);

	//* Add Input Window */
	//m_ChattingGui.setPos(15, chattingInfoHeight + 140);
	m_ChattingGui.add(L"InputField", GUITextArea::Create(1, 26));
	m_ChattingGui.add(L"InputButton", GUIButton::Create(L"Send"));
}

void RoomList::UserListUpdate()
{
	int idx = 0;
	for (auto i : m_UserListVector)
	{
		if (idx >= maxUserInfoNumber)
		{
			break;
		}

		m_UserListGui.text(std::to_wstring(idx)).text = i;
		++idx;
	}
}

void RoomList::CheckRoomClicked()
{
	for (const auto i : m_RoomInfoVector)
	{
		if (m_RoomListGui.button(i->buttonName).pushed)
		{
			changeScene(L"Room");
		}
	}
}

void RoomList::CheckSendClicked()
{
	if (m_ChattingGui.button(L"InputButton").pressed || Input::KeyEnter.clicked)
	{
		if (m_ChattingGui.textArea(L"InputField").text == L"")
		{
			return;
		}

		if (m_ChatString == L"")
		{
			m_ChatString = m_ChatString + m_ChattingGui.textArea(L"InputField").text;
		}
		else
		{
			m_ChatString = m_ChatString + L"\n" + m_ChattingGui.textArea(L"InputField").text;
		}

		SendChatting(m_ChatString.c_str());
		m_ChatQueue.push_back(m_ChatString.c_str());
		m_ChattingGui.textArea(L"InputField").setText(L"");
	}
}

void RoomList::ExitScene()
{
	m_RoomInfoVector.clear();
	m_UserListVector.clear();
}

void RoomList::SendChatting(std::wstring chat)
{
	PktLobbyChatReq newChatReq;
	memcpy(newChatReq.Msg, chat.c_str(), MAX_LOBBY_CHAT_MSG_SIZE);

	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_CHAT_REQ, sizeof(newChatReq), (char*)&newChatReq);
}

void RoomList::RequestUserInfo(const int startUserIndex)
{
	PktLobbyUserListReq newLobbyUserReq;
	newLobbyUserReq.StartUserIndex = startUserIndex;

	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_ENTER_USER_LIST_REQ, sizeof(newLobbyUserReq), (char*)&newLobbyUserReq);
}

void RoomList::CheckDataUpdated()
{
	const int lastestDataVersion = m_data->dataContainer->GetRoomListData()->GetVersion();

	if (m_CurrentDataVersion < lastestDataVersion)
	{
		std::wstring debugLabel = L"[RoomListScene] 데이터 리프레시! 현재 버전 : " + std::to_wstring(m_CurrentDataVersion) + L", 최신 버전 : " + std::to_wstring(lastestDataVersion);
		OutputDebugString(debugLabel.c_str());
		m_UserListVector.clear();

		if (m_data->dataContainer->GetRoomListData()->GetIsRequestNeeded())
		{
			// 아직 못받은 데이터가 있으면, 리퀘스트를 또 다시 요청해준다.
			// 근데 이거는 구조적으로 좀 고치면 좋지 않을까 싶음.
			RequestUserInfo(m_data->dataContainer->GetRoomListData()->GetReceivedLastestUserId());
		}
		else
		{
			// 데이터를 다 받았으므로 데이터를 업데이트 해주면 된다.
			for (auto i : m_data->dataContainer->GetRoomListData()->GetUserInfoVector())
			{
				m_UserListVector.push_back(i.second);
			}

			// 업데이트가 되었으므로, GUI에 표현되는 데이터를 바꾸어준다.
			UserListUpdate();
		}
		
		m_CurrentDataVersion = lastestDataVersion;
	}
}


void RoomList::CheckChattingSuccessed()
{
	if (m_data->dataContainer->GetRoomListData()->GetIsChatDelivered())
	{
		for (auto i : m_ChatQueue)
		{
			// TODO :: 여기서 부터 짜면 됩니다.
			//m_ChattingGuiString = m_ChattingGuiString
		}
		m_ChatQueue.clear();
	}
}
