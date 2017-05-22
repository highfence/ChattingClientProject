#pragma once

class TextScorllBox;


struct RoomList : MyApp::Scene
{
	GUI m_RoomListGui;
	GUI m_UserListGui;
	GUI m_ChattingGui;
	TextScorllBox* m_pChatTextBox;
	int m_CurrentDataVersion = -1;
	String m_ChatString;
	std::wstring m_ChattingGuiString;

	std::vector<std::shared_ptr<RoomInfo>> m_RoomInfoVector;
	std::vector<std::wstring> m_UserListVector;
	std::list<std::wstring> m_ChatList;

	/* Required Initializer for Siv3D */
	void init() override;

	void update() override;

	void draw() const override;


	/* Insert Data to member vector */
	void RoomInfoSetting();

	/* Make Room and Users accord with data */
	void MakeRooms();
	void MakeUsers();
	void MakeChattingGui();
	void userListUpdate();
	void chatListUpdate();

	/* Update Functions */
	void CheckRoomClicked();
	void CheckSendClicked();
	void CheckDataUpdated();

	/* Function must be called when scene changed */
	void ExitScene();

	/* Network Function */
	void SendChatting(std::wstring chat);
	void RequestUserInfo(const int startUserId = 0);
};