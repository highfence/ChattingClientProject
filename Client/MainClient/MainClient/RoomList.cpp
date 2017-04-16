#include "stdafx.h"
#include "RoomList.h"

const int roomWidth = 250;
const int buttonWidth = 50;

void RoomList::init()
{
	m_RoomListGui = GUI(GUIStyle::Default);
	m_UserListGui = GUI(GUIStyle::Default);
	m_ChattingGui = GUI(GUIStyle::Default);
	Graphics::SetBackground(Color(200, 160, 100));

	m_RoomListGui.setTitle(L"Room List");
	m_UserListGui.setTitle(L"User List");

	/* Data Setting */
	roomInfoSetting();
	userInfoSetting();

	/* Make Room and Users */
	makeRooms();
	makeUsers();

}

void RoomList::update()
{
}

void RoomList::draw() const
{
}

void RoomList::refresh()
{
}

void RoomList::roomInfoSetting()
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

void RoomList::userInfoSetting()
{
	const int userInfoNum = 5;
	const std::wstring userName = L"User ";
	// TODO :: user info must be distributed by server
	for (int i = 0; i < userInfoNum; ++i)
	{
		UserInfo* newUserInfo = new UserInfo;
		newUserInfo->userName = userName + std::to_wstring(i + 1);
		m_UserInfoVector.push_back(newUserInfo);
	}
}

void RoomList::makeRooms()
{
	int idx = 0;
	for (const auto& i : m_RoomInfoVector)
	{
		/* Make Rooms */
		m_RoomListGui.add(i->roomName, GUIText::Create(i->roomName, roomWidth));
		
		/* Make Buttons */	
		m_RoomListGui.addln(i->buttonName, GUIButton::Create(L"ENTER", buttonWidth));
	}
}

void RoomList::makeUsers()
{

}

void RoomList::exitScene()
{
	m_RoomInfoVector.clear();
	m_UserInfoVector.clear();
}
