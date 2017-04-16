#pragma once

struct RoomInfo
{
	std::wstring roomName;
	std::wstring buttonName;
};

struct UserInfo
{
	std::wstring userName;
};

struct RoomList : MyApp::Scene
{
	GUI m_RoomListGui;
	GUI m_UserListGui;
	GUI m_ChattingGui;
	std::vector<RoomInfo*> m_RoomInfoVector;
	std::vector<UserInfo*> m_UserInfoVector;

	/* Required Initializer for Siv3D */
	void init() override;

	void update() override;

	void draw() const override;

	/* Refreshing All Gui */
	void refresh();

	/* Insert Data to member vector */
	void roomInfoSetting();
	void userInfoSetting();

	/* Make Room and Users accord with data */
	void makeRooms();
	void makeUsers();

	/* Function must be called when scene changed */
	void exitScene();
};