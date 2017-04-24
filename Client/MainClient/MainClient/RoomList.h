#pragma once

class TextScorllBox;


struct RoomList : MyApp::Scene
{
	GUI m_RoomListGui;
	GUI m_UserListGui;
	GUI m_ChattingGui;
	TextScorllBox* m_pChatTextBox;
	std::vector<RoomInfo*> m_RoomInfoVector;
	std::vector<UserInfo*> m_UserInfoVector;
	std::stack<std::wstring*> m_ChatStack;
	String m_ChatString;

	/* Required Initializer for Siv3D */
	void init() override;

	void update() override;

	void draw() const override;

	/* Refreshing All Gui */
	void Refresh();

	/* Insert Data to member vector */
	void RoomInfoSetting();
	void UserInfoSetting();

	/* Make Room and Users accord with data */
	void MakeRooms();
	void MakeUsers();
	void MakeChattingGui();

	/* Update Functions */
	void CheckRoomClicked();
	void CheckSendClicked();

	/* Function must be called when scene changed */
	void ExitScene();

	/* Network Function */
	void SendChatting(std::wstring);
};