#include "stdafx.h"
#include "Room.h"

const std::wstring userName = L"User";
const int userMaxNumber = 6;

void Room::init()
{
#pragma region Gui Setting Functions

	// ���� ����Ʈ�� �����͸� ��Ͻ����ش�.
	auto RegisterUserData = [this]()
	{
		for (const auto& i : m_UserInfoVector)
		{
			m_UserGui.addln(std::to_wstring(i->userIndex), GUIText::Create(L""));
		}
	};

#pragma endregion

	m_ChattingGui = GUI(GUIStyle::Default);
	m_ChattingGui.setTitle(L"Let's Chat!");

	m_UserGui = GUI(GUIStyle::Default);
	m_UserGui.setTitle(L"User List");
	m_UserGui.setPos(Point(475, 0));

	m_InputGui = GUI(GUIStyle::Default);
	m_InputGui.setPos(Point(0, 300));
	
	Graphics::SetBackground(Color(95, 0, 255));

	// Data Setting 
	InitialUserDataSetting();

	// Input data in GUI 
	RegisterUserData();

	m_ChattingGui.add(L"ChattingWindow", GUITextArea::Create(10, 22));
	m_InputGui.add(L"InputField", GUITextArea::Create(1, 28));
	m_InputGui.add(L"InputButton", GUIButton::Create(L"Send"));

	m_UserGui.add(L"Divider", GUIHorizontalLine::Create());
	m_UserGui.add(L"BackButton", GUIButton::Create(L"Back"));
}

void Room::update()
{
#pragma region Update Functions

	// �����͸� ������Ʈ ���ִ� �Լ�.
	auto UpdateData = [this]()
	{
#pragma region UpdateData inner Functions

		// �ֽ� ���� �����Ϳ� ���ؼ� ������Ʈ�� �ʿ����� Ȯ�����ִ� �Լ�.
		auto IsDataUpdateNeeded = [this]()->bool
		{
			int lastestVersion = m_data->dataContainer->GetRoomData()->GetVersion();

			if (lastestVersion > m_CurrentDataVersion)
			{
				return true;
			}
			else return false;
		};

		// ������ �������� �α׸� �����ִ� �Լ�.
		auto LogDataRefresh = [this]()
		{
			std::wstring debugLabel = L"[Room] ������ ��������! ���� ���� : " + std::to_wstring(m_CurrentDataVersion) + L", �ֽ� ���� : "
				+ std::to_wstring(m_data->dataContainer->GetRoomData()->GetVersion()) + L"\n";

			OutputDebugString(debugLabel.c_str());
		};

		// ���� ���͸� �����ϴ� �Լ�.
		auto UpdateUserData = [this]()
		{
			auto sourceData = m_data->dataContainer->GetRoomData()->GetRefUserInfoList();
			auto sourceDataSize = sourceData.size();

			// �����͸� �����ϱ� �� ���͸� ����ش�.
			m_UserInfoVector.clear();
			m_UserInfoVector.reserve(sourceDataSize);

			// �����͸� �������ش�.
			int idx = 0;
			for (const auto& i : sourceData)
			{
				UserInfo* newUserInfo = new UserInfo;
				newUserInfo->userIndex = idx;
				newUserInfo->userName = i;
				newUserInfo->isUserInfoValid = true;

				m_UserInfoVector.emplace_back(std::move(newUserInfo));
				++idx;
			}
		};

		// ä�� �����͸� �����ϴ� �Լ�.
		auto UpdateChattingData = [this]()
		{
			auto recvMsgString = m_data->dataContainer->RequestMsgFromRoomData();
			while (recvMsgString != L"")
			{
				m_ChatList.push_back(recvMsgString);
				recvMsgString = m_data->dataContainer->RequestMsgFromRoomData();
			}
		};

		// ���� GUI�� �����ϴ� �Լ�.
		auto UpdateUserGui = [this]()
		{
			for (const auto& i : m_UserInfoVector)
			{
				// Valid ���� ���� �����Ͷ�� �ؽ�Ʈ�� �����ش�.
				if (!i->isUserInfoValid)
				{
					m_UserGui.text(std::to_wstring(i->userIndex)).text = L"           ";

				}
				else
				{
					// Valid �� �����Ͷ��, index�� ��ġ�ϴ� �ؽ�Ʈ�� �̸��� �ٲپ��ش�.				
					m_UserGui.text(std::to_wstring(i->userIndex)).text = i->userName;
				}
			}
		};

		// ä�� GUI�� �����ϴ� �Լ�.
		auto UpdateChattingGui = [this]()
		{
			// ä�� �޽����� ���� wstring
			std::wstring chatMsg;

			for (auto iter = m_ChatList.begin(); iter != m_ChatList.end(); ++iter)
			{
				chatMsg = chatMsg + (*iter);
			}

			m_ChattingGui.textArea(L"ChattingWindow").setText(chatMsg);
		};

		// ������ ������Ʈ ���ִ� �Լ�.
		auto UpdateVersion = [this]()
		{
			m_CurrentDataVersion = m_data->dataContainer->GetRoomData()->GetVersion();
		};

#pragma endregion

		// ������Ʈ�� �ʿ���ٸ�, �Լ��� ������.
		if (!IsDataUpdateNeeded()) return;
		// ������Ʈ�� �ʿ��ϴٸ�, �α׸� �����.
		else LogDataRefresh();

		// �������͸� �����Ѵ�.
		UpdateUserData();

		// ä�� ����Ʈ�� �����Ѵ�.
		UpdateChattingData();

		// ������ �����Ϳ� �����Ͽ� GUI�� �ٲپ��ش�.
		UpdateUserGui();
		UpdateChattingGui();

		// ������Ʈ�� ��� �������Ƿ� ������ ���Ž�Ų��.
		UpdateVersion();
	};

	// Send ��ư�� ���ȴ��� üũ�ϴ� �Լ�.
	auto CheckSendButtonPushed = [this]()
	{
		if (m_InputGui.button(L"InputButton").pressed || Input::KeyEnter.clicked)
		{
			if (m_InputGui.textArea(L"InputField").text == L"")
			{
				return;
			}

			m_ChatString = m_InputGui.textArea(L"InputField").text;

			// Send ��ư�� ������ ������Ʈ�� ��������, ä�� �����͸� ���� ��⿭�� �о�־� �ش�.
			SendChatting(m_ChatString.c_str());
			m_data->dataContainer->PushChatDataToRoomData(m_data->id, m_ChatString.c_str());
			m_ChatString.erase();
			m_InputGui.textArea(L"InputField").setText(L"");
		}
	};

	// Back ��ư�� ���ȴ��� üũ�ϴ� �Լ�.
	auto CheckBackButtonPushed = [this]()
	{
		if (m_UserGui.button(L"BackButton").pressed)
		{
			// �������� �����ٴ� �޽����� ��û�Ѵ�.
			m_data->dataContainer->SendRequest((short)PACKET_ID::ROOM_LEAVE_REQ, 0, nullptr);
		}
	};

	// Room�� ���� ��Ȳ���� Ȯ�����ִ� �Լ�.
	auto CheckRoomLeaved = [this]()
	{
		// �����κ��� ���� �����ٴ� ��Ŷ�� �޾Ҵٸ�,
		if (m_data->dataContainer->GetRoomData()->GetIsLeaveRoomSuccessed())
		{
			// ���� �ٲپ��ش�.
			ExitScene(L"RoomList");
		}
	};

#pragma endregion

	m_data->dataContainer->Update();

	UpdateData();
	CheckSendButtonPushed();
	CheckBackButtonPushed();
	CheckRoomLeaved();
}

void Room::draw() const
{
}

void Room::InitialUserDataSetting()
{
	m_UserInfoVector.clear();
	m_UserInfoVector.reserve(userMaxNumber);

	for (int i = 0; i < userMaxNumber; ++i)
	{
		UserInfo* userData = new UserInfo;
		userData->userIndex = i;
		userData->userName = L"           ";
		userData->isUserInfoValid = false;

		m_UserInfoVector.emplace_back(std::move(userData));
	}
}

void Room::ExitScene(wchar_t * changeSceneName)
{
	// �������� �����ٴ� �޽����� ���� ��, ���� �ٲپ� �ش�.
	m_UserInfoVector.clear();
	m_ChatList.clear();

	changeScene(changeSceneName);
}

void Room::SendChatting(std::wstring chatMsg)
{
	PktRoomChatReq newChatReq;
	memcpy(newChatReq.Msg, chatMsg.c_str(), MAX_ROOM_CHAT_MSG_SIZE);

	// ä�� ���� ��û�� ������.
	m_data->dataContainer->SendRequest(
		(short)PACKET_ID::ROOM_CHAT_REQ,
		sizeof(newChatReq),
		(char*)&newChatReq);

}
