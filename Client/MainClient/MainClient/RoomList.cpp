#include <string>
#include "stdafx.h"
#include "RoomList.h"

const int roomWidth = 250;
const int buttonWidth = 50;
const int userInfoWidth = 200;
const int chattingInfoHeight = 275;
const int maxUserInfoNumber = 5;
const int roomInfoMaxNum = 6;

void RoomList::init()
{
	RequestUserInfo();
	RequestRoomInfo();

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
	RoomInfoInitialize();

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

		// ���� ���� ����Ʈ ������Ʈ.
		auto UpdateUserData = [this]()
		{
			m_UserListVector.clear();
			for (const auto& i : m_data->dataContainer->GetRoomListData()->GetRefUserInfoList())
			{
				m_UserListVector.push_back(i);
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

		// Room ���� ������Ʈ.
		auto UpdateRoomData = [this]()
		{
			// TODO :: ������ �� �̻���
			// ���� ChangedRoomInfo�� �Դٸ�, ���� Room���� ��ü�� ������Ʈ ���� �ʾƵ� �ȴ�.
			RoomInfo changedRoomInfo;
			auto roomListData = m_data->dataContainer->GetRoomListData();
			auto isRoomInfoChanged = roomListData->GetChangedRoomInfoFromQueue(
				&changedRoomInfo.roomIndex,
				&changedRoomInfo.roomUserCount,
				changedRoomInfo.roomTitle);
				
			// ChangedRoomInfo�� �Դٸ�, RoomList���� ��ü�� ������Ʈ ���� �ʾƵ� �ȴ�.
			if (isRoomInfoChanged == true)
			{
				// �ٲ� ������ ������Ʈ ���ش�.
				m_RoomInfoVector.at(changedRoomInfo.roomIndex)->roomUserCount = changedRoomInfo.roomUserCount;
				m_RoomInfoVector.at(changedRoomInfo.roomIndex)->roomTitle = changedRoomInfo.roomTitle;
			}
			// �װ� �ƴ϶��, �� ���� ��ü�� ������Ʈ �Ѵ�.
			else
			{
				// ������ �ִ� RoomInfo�� �ʱ�ȭ���ش�.
				RoomInfoInitialize();

				for (auto& i : m_RoomInfoVector)
				{
					// ���͸� ��ȸ�ϸ� �����͸� �޾ƿ´�.
					auto retval = roomListData->GetRoomInfoFromQueue(
						&i->roomIndex,
						&i->roomUserCount,
						i->roomTitle);

					// �����Ͱ� ��ȿ�ϸ� true��, ������ ���д� false ���·� ���д�.
					if (!retval) break;
					else
					{
						i->isRoomInfoValid = true;
						++m_ExistRoomIdx;
					}
				}
			}
		};

		// ���� â ������Ʈ.
		auto UserGuiUpdate = [this]()
		{
			// TODO :: ���⼭ �����ִ� ���� �����͸� �����־�� ��.
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

			while (idx < maxUserInfoNumber)
			{
				m_UserListGui.text(std::to_wstring(idx)).text = L"NONE";
				++idx;
			}
		};

		// ä��â ������Ʈ.
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

		// Room Gui ������Ʈ.
		auto RoomGuiUpdate = [this]()
		{
			for (int i = 0; i < roomInfoMaxNum; ++i)
			{
				if (m_RoomInfoVector.at(i)->isRoomInfoValid)
				{
					// TODO :: �ο����� �����ֱ�.
					m_RoomListGui.button(std::to_wstring(i)).enabled = true;
					m_RoomListGui.text(std::to_wstring(i)).text = m_RoomInfoVector.at(i)->roomTitle;
				}
				else
				{
					m_RoomListGui.button(std::to_wstring(i)).enabled = false;
					m_RoomListGui.text(std::to_wstring(i)).text = L"";
				}
			}
		};

		// �� ���� ���� ������Ʈ.
		auto RoomEnterDataUpdate = [this]()
		{
			// �� ������ �Ǿ��ٸ�, ���� ������ �ٲپ��ش�.
			auto isRoomEntered = m_data->dataContainer->GetRoomListData()->GetIsRoomSuccesslyEntered();
			if (isRoomEntered)
			{

				ExitScene(L"Room");
			}
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
		auto IsUserDataRequestNeeded = [this]()
		{
			return m_data->dataContainer->GetRoomListData()->GetIsUserDataRequestNeeded();
		};

		// �� ���� ������Ʈ�� �ʿ������� �������� �Լ�.
		auto IsRoomDataRequestNeeded = [this]()
		{
			return m_data->dataContainer->GetRoomListData()->GetIsRoomDataRequestNeeded();
		};

#pragma endregion

		const int lastestDataVersion = GetLatestDataVersion();

		if (IsDataRefreshNeeded(lastestDataVersion))
		{
			std::wstring debugLabel = L"[RoomListScene] ������ ��������! ���� ���� : " + std::to_wstring(m_CurrentDataVersion) + L", �ֽ� ���� : " + std::to_wstring(lastestDataVersion) + L"\n";
			OutputDebugString(debugLabel.c_str());
			m_UserListVector.clear();

			// ���� ������ ���� �����Ͱ� ������, ������Ʈ�� �� �ٽ� ��û���ش�.
			if (IsUserDataRequestNeeded())
			{
				auto lastestUserIndex = m_data->dataContainer->GetRoomListData()->GetReceivedLastestUserId();
				RequestUserInfo(lastestUserIndex);
			}
			// ���� ������ �� �����Ͱ� ������, ������Ʈ�� �� �ٽ� ��û���ش�.
			if (IsRoomDataRequestNeeded())
			{
				auto lastestRoomIndex = m_data->dataContainer->GetRoomListData()->GetReceivedLastestRoomId();
				RequestRoomInfo(lastestRoomIndex);
			}

			// ������ ������Ʈ.
			UpdateUserData();
			UpdateChatData();
			UpdateRoomData();

			// ������Ʈ�� �Ǿ����Ƿ�, GUI�� ǥ���Ǵ� �����͸� �ٲپ��ش�.
			UserGuiUpdate();
			UserChatWindowUpdate();
			RoomGuiUpdate();
			RoomEnterDataUpdate();

			// ������ �ִ� �������� ������ �������ش�.
			m_CurrentDataVersion = lastestDataVersion;
		};
	};

	// Room ��ư�� Ŭ���Ǿ����� Ȯ�����ִ� �Լ�.
	auto CheckRoomClicked = [this]()
	{
		for (const auto i : m_RoomInfoVector)
		{
			if (m_RoomListGui.button(std::to_wstring(i->roomIndex)).pushed)
			{
				// Room ���� ��û�� ������.
				PktRoomEnterReq enterReq;
				enterReq.IsCreate = false;
				enterReq.RoomIndex = i->roomIndex;
				wcscpy_s(enterReq.RoomTitle, MAX_ROOM_TITLE_SIZE, i->roomTitle.c_str());

				m_data->dataContainer->SendRequest(
					(short)PACKET_ID::ROOM_ENTER_REQ,
					sizeof(PktRoomEnterReq),
					(char*)&enterReq);

				OutputDebugString(L"[RoomList] �� ���� ��û. \n");
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

			m_ChatString = m_ChattingGui.textArea(L"InputField").text;

			// Send ��ư�� ������ ������Ʈ�� ��������, ������ ��ٸ���.
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
			ExitScene(L"LobbyList");
		}
	};

	// Create ��ư�� Ŭ�� �Ǿ����� Ȯ�����ִ� �Լ�.
	auto CheckCreateButtonClicked = [this]()
	{
		// CreateButton�� Ŭ���Ǿ��ٸ� ��û�� �����ش�.
		if (m_UserListGui.button(L"CreateButton").pushed)
		{
			PktRoomEnterReq enterReq;
			enterReq.IsCreate = true;
			enterReq.RoomIndex = ++m_ExistRoomIdx;
			std::wstring roomTitle = L"Room" + std::to_wstring(m_ExistRoomIdx);
			wcscpy_s(enterReq.RoomTitle, MAX_ROOM_TITLE_SIZE, roomTitle.c_str());

			m_data->dataContainer->SendRequest(
				(short)PACKET_ID::ROOM_ENTER_REQ,
				sizeof(PktRoomEnterReq),
				(char*)&enterReq);

			OutputDebugString(L"[RoomList] �� ���� ��û. \n");
		}
	};

#pragma endregion

	m_data->dataContainer->Update();

	CheckDataUpdated();
	CheckRoomClicked();
	CheckSendButtonClicked();
	CheckBackButtonClicked();
	CheckCreateButtonClicked();
}

void RoomList::draw() const
{
}


void RoomList::RoomInfoInitialize()
{
	m_RoomInfoVector.clear();
	const std::wstring roomName = L"NONE";
	for (short i = 0; i < roomInfoMaxNum; ++i)
	{
		std::shared_ptr<RoomInfo> newRoomInfo = std::make_shared<RoomInfo>();
		newRoomInfo->roomTitle = roomName + std::to_wstring(i);
		std::wstring buttonName = L"Button" + std::to_wstring(i);
		newRoomInfo->buttonName = buttonName;
		newRoomInfo->roomIndex = i;
		newRoomInfo->isRoomInfoValid = false;

		m_RoomInfoVector.emplace_back(std::move(newRoomInfo));
	}
	m_ExistRoomIdx = -1;
}

void RoomList::MakeRooms()
{
	for (const auto& i : m_RoomInfoVector)
	{
		/* Make Rooms */
		m_RoomListGui.add(std::to_wstring(i->roomIndex), GUIText::Create(L"", roomWidth));
		
		/* Make Buttons */	
		m_RoomListGui.addln(std::to_wstring(i->roomIndex), GUIButton::Create(L"ENTER", false));
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
	m_UserListGui.add(L"BackButton", GUIButton::Create(L"Back", true));
	m_UserListGui.addln(L"CreateButton", GUIButton::Create(L"Create", true));
}

void RoomList::MakeChattingGui()
{
	/* Add Chatting Window */
	m_ChattingGui.addln(L"ChattingWindow", GUITextArea::Create(5, 33));

	/* Add Divide Line */
	m_ChattingGui.add(L"Divider", GUIHorizontalLine::Create(1));
	m_ChattingGui.horizontalLine(L"Divider").style.color = Color(127);

	//* Add Input Window */
	m_ChattingGui.add(L"InputField", GUITextArea::Create(1, 26));
	m_ChattingGui.add(L"InputButton", GUIButton::Create(L"Send"));
}

void RoomList::ExitScene(wchar_t* changeSceneName)
{
	// �������� �����ٴ� �޽����� ������, ���� �ٲپ� �ش�.			
	m_data->dataContainer->SendRequest((short)PACKET_ID::LOBBY_LEAVE_REQ, 0, nullptr);
	m_RoomInfoVector.clear();
	m_UserListVector.clear();

	changeScene(changeSceneName);
}

void RoomList::SendChatting(std::wstring chatMsg)
{
	PktLobbyChatReq newChatReq;
	memcpy(newChatReq.Msg, chatMsg.c_str(), MAX_LOBBY_CHAT_MSG_SIZE);

	m_data->dataContainer->SendRequest(
		(short)PACKET_ID::LOBBY_CHAT_REQ,
		sizeof(newChatReq),
		(char*)&newChatReq);

	m_data->dataContainer->SendChatToRoomList(
		m_data->id,
		chatMsg);
}

void RoomList::RequestUserInfo(const short startUserIndex)
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

void RoomList::RequestRoomInfo(const short startRoomId)
{
	PktLobbyRoomListReq newRoomListReq;
	newRoomListReq.StartRoomIndex = startRoomId;

	std::wstring debugLabel = L"[RoomList] �� ���� ��û. \n";
	OutputDebugString(debugLabel.c_str());

	m_data->dataContainer->SendRequest(
		(short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_REQ,
		sizeof(newRoomListReq),
		(char*)&newRoomListReq);
}

