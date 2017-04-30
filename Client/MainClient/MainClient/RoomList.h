#pragma once

class TextScorllBox;


struct RoomList : MyApp::Scene
{
	GUI m_RoomListGui;
	GUI m_UserListGui;
	GUI m_ChattingGui;
	TextScorllBox* m_pChatTextBox;
	int m_CurrentDataVersion = -1;
	std::vector<RoomInfo*> m_RoomInfoVector;
	std::vector<std::wstring> m_UserListVector;
	std::wstring m_ChattingGuiString;
	String m_ChatString;
	std::deque<std::wstring> m_ChatQueue;

	/* Required Initializer for Siv3D */
	void init() override;

	void update() override;

	void draw() const override;

	/* Refreshing All Gui */
	void Refresh();

	/* Insert Data to member vector */
	void RoomInfoSetting();

	/* Make Room and Users accord with data */
	void MakeRooms();
	void MakeUsers();
	void MakeChattingGui();
	void UserListUpdate();

	/* Update Functions */
	void CheckRoomClicked();
	void CheckSendClicked();
	void CheckDataUpdated();
	void CheckChattingSuccessed();


	/* Function must be called when scene changed */
	void ExitScene();

	/* Network Function */
	void SendChatting(std::wstring);
	void RequestUserInfo(const int startUserId = 0);
};