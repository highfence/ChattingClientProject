#include "stdafx.h"
#include "Room.h"

const std::wstring userName = L"User";
const int userMaxNumber = 5;

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
	setUserData();

	/* Input data in GUI */
	m_ChattingGui.add(L"RoomChatting", GUITextArea::Create(10, 22));
	m_InputGui.add(L"InputField", GUITextArea::Create(1, 28));
	m_InputGui.add(L"InputButton", GUIButton::Create(L"Send"));
	drawUser();
}

void Room::update()
{
	checkSendPushed();
}

void Room::draw() const
{

}

void Room::setUserData()
{
	std::mt19937 rng((unsigned int)time(NULL));
	std::uniform_int_distribution<int> userRange(1, userMaxNumber);

	int currentUserNumber = userRange(rng);

	for (int i = 0; i < currentUserNumber; ++i)
	{
		UserInfo* newUser = new UserInfo;
		newUser->userName = userName + std::to_wstring(i + 1);
		m_UserInfoVector.push_back(newUser);
	}
}

void Room::drawUser()
{
	for (const auto& i : m_UserInfoVector)
	{
		m_UserGui.addln(L"RoomUsingUser", GUIText::Create(i->userName, 150));
	}
}

void Room::checkSendPushed()
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
