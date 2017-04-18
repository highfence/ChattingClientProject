#include "stdafx.h"
#include "Room.h"

const std::wstring userName = L"User";
const int userMaxNumber = 5;

void Room::init()
{
	m_ChattingGui = GUI(GUIStyle::Default);
	m_ChattingGui.setTitle(L"Let's Chat!");
	
	Graphics::SetBackground(Color(95, 0, 255));

	/* Data Setting */
	setUserData();

	/* Input data in GUI */
	m_ChattingGui.add(L"RoomChatting", GUITextArea::Create(10, 26));
	drawUser();
}

void Room::update()
{

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

	return;
}

void Room::drawUser()
{

}
