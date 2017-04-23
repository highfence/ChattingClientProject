#pragma once


struct LobbyInfo
{
	std::wstring LobbyName;
	std::wstring LobbyId;
	std::wstring LobbyButtonName;
	int UserNumber = 0;
	int MaxUserNumber = 50;
};


struct Lobby : MyApp::Scene 
{
	GUI m_LobbyGui;
	GUI m_InfoGui;
	std::vector<LobbyInfo*> m_LobbyVector;

	/* Required Initializer for Siv3D */
	void init() override;

	/* Required Function for Siv3d */
	void update() override;

	/* Required Function for Siv3d */
	void draw() const override;

	/* Insert Lobby Info to vector */
	void lobbyInfoSetting();

	/* Make text field accord with set lobby info */
	void makeLobbys();

	/* Function which must be called when the scene changed */
	void exitScene();

	/* Check which button clicked */
	void checkButtonClicked();

	/* Request Lobby Info */
	void reqLobbyInfo();
};
