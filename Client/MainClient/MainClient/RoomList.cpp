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

#pragma region GUI Setting
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
#pragma endregion

	/* Data Setting */
	RoomInfoSetting();

	/* Input data in Gui */
	MakeRooms();
	MakeUsers();
	MakeChattingGui();
}

void RoomList::update()
{
#pragma region CheckFunctions
	// �����Ͱ� �ֽ� �������� Ȯ�����ִ� �Լ�.
	auto CheckDataUpdated = [this]()
	{
#pragma region Update Functions...
		// ���� ���� ���� ������Ʈ.
		auto UpdateUserData = [this]()
		{
			for (auto i : m_data->dataContainer->GetRoomListData()->GetUserInfoVector())
			{
				m_UserListVector.push_back(i.second);
			}
		};

		// ä�� ���� ����Ʈ ������Ʈ.
		auto UpdateChatData = [this]()
		{
			auto recvMsgString = m_data->dataContainer->RequestMsgFromRoomList();
			while (recvMsgString != L"")
			{
				m_ChatList.push_back(recvMsgString);
				recvMsgString = m_data->dataContainer->RequestMsgFromRoomList();
			}
		};

		auto UserGuiUpdate = [this]()
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
		};

		auto UserChatWindowUpdate = [this]()
		{
			// ä�� �޽����� ���� wstring
			std::wstring ChatMsg;

			for (auto iter = m_ChatList.begin(); iter != m_ChatList.end(); ++iter)
			{
				ChatMsg = ChatMsg + (*iter);
			}

			m_ChattingGui.textArea(L"ChattingWindow").setText(ChatMsg);
		};
#pragma endregion 
#pragma region Util Functions...
		// ���� �ֽ��� ������ �������� �Լ�.
		auto GetLatestDataVersion = [this]()
		{
			return m_data->dataContainer->GetRoomListData()->GetVersion();
		};

		// ���� �ڽ��� ������ �ִ� ������ �ֽ� �������� Ȯ���ϴ� �Լ�.
		auto IsDataRefreshNeeded = [this](int newDataVersion)
		{
			if (m_CurrentDataVersion < newDataVersion)
			{
				return true;
			}
			return false;
		};

		// ���� ���� ������Ʈ�� �ʿ������� �������� �Լ�.
		auto IsUsetDataRequestNeeded = [this]()
		{
			return m_data->dataContainer->GetRoomListData()->GetIsRequestNeeded();
		};
#pragma endregion

		const int lastestDataVersion = GetLatestDataVersion();

		if (IsDataRefreshNeeded(lastestDataVersion))
		{
			std::wstring debugLabel = L"[RoomListScene] ������ ��������! ���� ���� : " + std::to_wstring(m_CurrentDataVersion) + L", �ֽ� ���� : " + std::to_wstring(lastestDataVersion) + L"\n";
			OutputDebugString(debugLabel.c_str());
			m_UserListVector.clear();

			// ���� ������ �����Ͱ� ������, ������Ʈ�� �� �ٽ� ��û���ش�.
			if (IsUsetDataRequestNeeded())
			{
				// �ٵ� �̰Ŵ� ���������� �� ��ġ�� ���� ������ ����.
				RequestUserInfo(m_data->dataContainer->GetRoomListData()->GetReceivedLastestUserId());
			}
			else
			{
				// ������ ������Ʈ.
				UpdateUserData();
				UpdateChatData();

				// ������Ʈ�� �Ǿ����Ƿ�, GUI�� ǥ���Ǵ� �����͸� �ٲپ��ش�.
				UserGuiUpdate();
				UserChatWindowUpdate();
			}

			// ������ �ִ� �������� ������ �������ش�.
			m_CurrentDataVersion = lastestDataVersion;
		}
	};

	// Room ��ư�� Ŭ���Ǿ����� Ȯ�����ִ� �Լ�.
	auto CheckRoomClicked = [this]()
	{
		for (const auto i : m_RoomInfoVector)
		{
			if (m_RoomListGui.button(i->buttonName).pushed)
			{
				changeScene(L"Room");
			}
		}
	};

	// Send ��ư�� Ŭ���Ǿ����� Ȯ�����ִ� �Լ�.
	auto CheckSendButtonClicked = [this]()
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

			// Send��ư�� ������ ������Ʈ�� ��������, �ϴ� Queue�� ������ ���´�.
			SendChatting(m_ChatString.c_str());
			m_ChatString.erase();
			m_ChattingGui.textArea(L"InputField").setText(L"");
		}
	};

	// Back ��ư�� Ŭ�� �Ǿ����� Ȯ�����ִ� �Լ�.
	auto CheckBackButtonClicked = [this]()
	{
		if (m_UserListGui.button(L"BackButton").pushed)
		{
			changeScene(L"LobbyList");
		}
	};
#pragma endregion

	m_data->dataContainer->Update();

	CheckDataUpdated();
	CheckRoomClicked();
	CheckSendButtonClicked();
	CheckBackButtonClicked();
}

void RoomList::draw() const
{
}


void RoomList::RoomInfoSetting()
{
	const int roomInfoNum = 6;
	const std::wstring roomName = L"Room ";
	for (int i = 0; i < roomInfoNum; ++i)
	{
		std::shared_ptr<RoomInfo> newRoomInfo = std::make_shared<RoomInfo>();
		newRoomInfo->roomName = roomName + std::to_wstring(i + 1);
		std::wstring buttonName = L"Button" + std::to_wstring(i + 1);
		newRoomInfo->buttonName = buttonName;

		m_RoomInfoVector.emplace_back(std::move(newRoomInfo));
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

	m_UserListGui.add(L"UserListDivider", GUIHorizontalLine::Create(1));
	m_UserListGui.addln(L"BackButton", GUIButton::Create(L"Back", 33));
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

void RoomList::ExitScene()
{
	m_RoomInfoVector.clear();
	m_UserListVector.clear();
}

void RoomList::SendChatting(std::wstring chat)
{
	PktLobbyChatReq newChatReq;
	memcpy(newChatReq.Msg, chat.c_str(), MAX_LOBBY_CHAT_MSG_SIZE);

	m_data->dataContainer->SendRequest(
		(short)PACKET_ID::LOBBY_CHAT_REQ,
		sizeof(newChatReq),
		(char*)&newChatReq);
	m_data->dataContainer->SendChatToRoomList(
		m_data->id,
		chat);
}

void RoomList::RequestUserInfo(const int startUserIndex)
{
	PktLobbyUserListReq newLobbyUserReq;
	newLobbyUserReq.StartUserIndex = startUserIndex;

	std::wstring debugLabel = L"[RoomList] ���� ���� ��û. \n";
	OutputDebugString(debugLabel.c_str());

	m_data->dataContainer->SendRequest(
		(short)PACKET_ID::LOBBY_ENTER_USER_LIST_REQ,
		sizeof(newLobbyUserReq),
		(char*)&newLobbyUserReq);
}

