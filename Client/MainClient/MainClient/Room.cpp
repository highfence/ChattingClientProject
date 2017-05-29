#include "stdafx.h"
#include "Room.h"

const std::wstring userName = L"User";
const int userMaxNumber = 6;

void Room::init()
{
	m_ChattingGui = GUI(GUIStyle::Default);
	m_ChattingGui.setTitle(L"Let's Chat!");

	m_UserGui = GUI(GUIStyle::Default);
	m_UserGui.setTitle(L"User List");
	m_UserGui.setPos(Point(475, 0));

	m_InputGui = GUI(GUIStyle::Default);
	m_InputGui.setPos(Point(0, 300));
	
	Graphics::SetBackground(Color(95, 0, 255));

	/* Data Setting */
	InitialUserDataSetting();

	/* Input data in GUI */
	m_ChattingGui.add(L"RoomChatting", GUITextArea::Create(10, 22));
	m_InputGui.add(L"InputField", GUITextArea::Create(1, 28));
	m_InputGui.add(L"InputButton", GUIButton::Create(L"Send"));
}

void Room::update()
{
	CheckSendPushed();
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
		std::shared_ptr<UserInfo> userData = std::make_shared<UserInfo>();
		userData->userIndex = i;
		userData->userName = L"";
		userData->isUserInfoValid = false;

		m_UserInfoVector.emplace_back(std::move(userData));
	}
}

void Room::CheckSendPushed()
{
	if (m_InputGui.button(L"InputButton").pressed || Input::KeyEnter.clicked)
	{
		if (m_InputGui.textArea(L"InputField").text == L"")
		{
			return;
		}
		
		if (m_ChatString == L"")
		{
			m_ChatString = m_ChatString + m_InputGui.textArea(L"InputField").text;
		}
		else
		{
			m_ChatString = m_ChatString + L"\n" + m_InputGui.textArea(L"InputField").text;
		}

		m_ChattingGui.textArea(L"RoomChatting").setText(m_ChatString);
		m_InputGui.textArea(L"InputField").setText(L"");
	}
}
