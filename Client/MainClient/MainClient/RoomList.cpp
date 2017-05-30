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

	// 데이터가 최신 버전인지 확인해주는 함수.
	auto CheckDataUpdated = [this]()
	{
#pragma region Update Functions...

		// 유저 정보 리스트 업데이트.
		auto UpdateUserData = [this]()
		{
			m_UserListVector.clear();
			for (const auto& i : m_data->dataContainer->GetRoomListData()->GetRefUserInfoList())
			{
				m_UserListVector.push_back(i);
			}
		};

		// 채팅 정보 리스트 업데이트.
		auto UpdateChatData = [this]()
		{
			auto recvMsgString = m_data->dataContainer->RequestMsgFromRoomList();
			while (recvMsgString != L"")
			{
				m_ChatList.push_back(recvMsgString);
				recvMsgString = m_data->dataContainer->RequestMsgFromRoomList();
			}
		};

		// Room 정보 업데이트.
		auto UpdateRoomData = [this]()
		{
			// TODO :: 구조가 좀 이상함
			// 만약 ChangedRoomInfo가 왔다면, 구지 Room정보 전체를 업데이트 하지 않아도 된다.
			RoomInfo changedRoomInfo;
			auto roomListData = m_data->dataContainer->GetRoomListData();
			auto isRoomInfoChanged = roomListData->GetChangedRoomInfoFromQueue(
				&changedRoomInfo.roomIndex,
				&changedRoomInfo.roomUserCount,
				changedRoomInfo.roomTitle);
				
			// ChangedRoomInfo가 왔다면, RoomList정보 전체를 업데이트 하지 않아도 된다.
			if (isRoomInfoChanged == true)
			{
				// 바뀐 정보만 업데이트 해준다.
				m_RoomInfoVector.at(changedRoomInfo.roomIndex)->roomUserCount = changedRoomInfo.roomUserCount;
				m_RoomInfoVector.at(changedRoomInfo.roomIndex)->roomTitle = changedRoomInfo.roomTitle;
			}
			// 그게 아니라면, 룸 정보 전체를 업데이트 한다.
			else
			{
				// 가지고 있던 RoomInfo를 초기화해준다.
				RoomInfoInitialize();

				for (auto& i : m_RoomInfoVector)
				{
					// 벡터를 순회하며 데이터를 받아온다.
					auto retval = roomListData->GetRoomInfoFromQueue(
						&i->roomIndex,
						&i->roomUserCount,
						i->roomTitle);

					// 데이터가 유효하면 true를, 마지막 실패는 false 상태로 놔둔다.
					if (!retval) break;
					else
					{
						i->isRoomInfoValid = true;
						++m_ExistRoomIdx;
					}
				}
			}
		};

		// 유저 창 업데이트.
		auto UserGuiUpdate = [this]()
		{
			// TODO :: 여기서 남아있던 유저 데이터를 지워주어야 함.
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

		// 채팅창 업데이트.
		auto UserChatWindowUpdate = [this]()
		{
			// 채팅 메시지를 담을 wstring
			std::wstring ChatMsg;

			for (auto iter = m_ChatList.begin(); iter != m_ChatList.end(); ++iter)
			{
				ChatMsg = ChatMsg + (*iter);
			}

			m_ChattingGui.textArea(L"ChattingWindow").setText(ChatMsg);
		};

		// Room Gui 업데이트.
		auto RoomGuiUpdate = [this]()
		{
			for (int i = 0; i < roomInfoMaxNum; ++i)
			{
				if (m_RoomInfoVector.at(i)->isRoomInfoValid)
				{
					// TODO :: 인원수도 적어주기.
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

		// 방 입장 정보 업데이트.
		auto RoomEnterDataUpdate = [this]()
		{
			// 방 입장이 되었다면, 씬을 방으로 바꾸어준다.
			auto isRoomEntered = m_data->dataContainer->GetRoomListData()->GetIsRoomSuccesslyEntered();
			if (isRoomEntered)
			{

				ExitScene(L"Room");
			}
		};

#pragma endregion 
#pragma region Util Functions...
		// 가장 최신의 버전을 가져오는 함수.
		auto GetLatestDataVersion = [this]()
		{
			return m_data->dataContainer->GetRoomListData()->GetVersion();
		};

		// 현재 자신이 가지고 있는 버전이 최신 버전인지 확인하는 함수.
		auto IsDataRefreshNeeded = [this](int newDataVersion)
		{
			if (m_CurrentDataVersion < newDataVersion)
			{
				return true;
			}
			return false;
		};

		// 유저 정보 리퀘스트가 필요한지를 가져오는 함수.
		auto IsUserDataRequestNeeded = [this]()
		{
			return m_data->dataContainer->GetRoomListData()->GetIsUserDataRequestNeeded();
		};

		// 룸 정보 리퀘스트가 필요한지를 가져오는 함수.
		auto IsRoomDataRequestNeeded = [this]()
		{
			return m_data->dataContainer->GetRoomListData()->GetIsRoomDataRequestNeeded();
		};

#pragma endregion

		const int lastestDataVersion = GetLatestDataVersion();

		if (IsDataRefreshNeeded(lastestDataVersion))
		{
			std::wstring debugLabel = L"[RoomListScene] 데이터 리프레시! 현재 버전 : " + std::to_wstring(m_CurrentDataVersion) + L", 최신 버전 : " + std::to_wstring(lastestDataVersion) + L"\n";
			OutputDebugString(debugLabel.c_str());
			m_UserListVector.clear();

			// 아직 못받은 유저 데이터가 있으면, 리퀘스트를 또 다시 요청해준다.
			if (IsUserDataRequestNeeded())
			{
				auto lastestUserIndex = m_data->dataContainer->GetRoomListData()->GetReceivedLastestUserId();
				RequestUserInfo(lastestUserIndex);
			}
			// 아직 못받은 룸 데이터가 있으면, 리퀘스트를 또 다시 요청해준다.
			if (IsRoomDataRequestNeeded())
			{
				auto lastestRoomIndex = m_data->dataContainer->GetRoomListData()->GetReceivedLastestRoomId();
				RequestRoomInfo(lastestRoomIndex);
			}

			// 데이터 업데이트.
			UpdateUserData();
			UpdateChatData();
			UpdateRoomData();

			// 업데이트가 되었으므로, GUI에 표현되는 데이터를 바꾸어준다.
			UserGuiUpdate();
			UserChatWindowUpdate();
			RoomGuiUpdate();
			RoomEnterDataUpdate();

			// 가지고 있는 데이터의 버전을 갱신해준다.
			m_CurrentDataVersion = lastestDataVersion;
		};
	};

	// Room 버튼이 클릭되었는지 확인해주는 함수.
	auto CheckRoomClicked = [this]()
	{
		for (const auto i : m_RoomInfoVector)
		{
			if (m_RoomListGui.button(std::to_wstring(i->roomIndex)).pushed)
			{
				// Room 입장 요청을 보낸다.
				PktRoomEnterReq enterReq;
				enterReq.IsCreate = false;
				enterReq.RoomIndex = i->roomIndex;
				wcscpy_s(enterReq.RoomTitle, MAX_ROOM_TITLE_SIZE, i->roomTitle.c_str());

				m_data->dataContainer->SendRequest(
					(short)PACKET_ID::ROOM_ENTER_REQ,
					sizeof(PktRoomEnterReq),
					(char*)&enterReq);

				OutputDebugString(L"[RoomList] 방 입장 요청. \n");
			}
		}
	};

	// Send 버튼이 클릭되었는지 확인해주는 함수.
	auto CheckSendButtonClicked = [this]()
	{
		if (m_ChattingGui.button(L"InputButton").pressed || Input::KeyEnter.clicked)
		{
			if (m_ChattingGui.textArea(L"InputField").text == L"")
			{
				return;
			}

			m_ChatString = m_ChattingGui.textArea(L"InputField").text;

			// Send 버튼이 눌리면 리퀘스트를 보내놓고, 응답을 기다린다.
			SendChatting(m_ChatString.c_str());
			m_ChatString.erase();
			m_ChattingGui.textArea(L"InputField").setText(L"");
		}
	};

	// Back 버튼이 클릭 되었는지 확인해주는 함수.
	auto CheckBackButtonClicked = [this]()
	{
		if (m_UserListGui.button(L"BackButton").pushed)
		{
			ExitScene(L"LobbyList");
		}
	};

	// Create 버튼이 클릭 되었는지 확인해주는 함수.
	auto CheckCreateButtonClicked = [this]()
	{
		// CreateButton이 클릭되었다면 요청을 보내준다.
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

			OutputDebugString(L"[RoomList] 방 생성 요청. \n");
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
	// 서버에게 나간다는 메시지를 보낸뒤, 씬을 바꾸어 준다.			
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

	std::wstring debugLabel = L"[RoomList] 유저 인포 요청. \n";
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

	std::wstring debugLabel = L"[RoomList] 룸 정보 요청. \n";
	OutputDebugString(debugLabel.c_str());

	m_data->dataContainer->SendRequest(
		(short)PACKET_ID::LOBBY_ENTER_ROOM_LIST_REQ,
		sizeof(newRoomListReq),
		(char*)&newRoomListReq);
}

