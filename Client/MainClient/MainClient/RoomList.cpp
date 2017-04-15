#include "stdafx.h"
#include "RoomList.h"

void RoomList::init()
{
	m_RoomListGui = GUI(GUIStyle::Default);
	m_UserListGui = GUI(GUIStyle::Default);
	m_ChattingGui = GUI(GUIStyle::Default);
	Graphics::SetBackground(Color(200, 160, 100));

	m_RoomListGui.setTitle(L"Room List");
	m_UserListGui.setTitle(L"User List");


}
