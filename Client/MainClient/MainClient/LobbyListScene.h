#pragma once


struct LobbyInfo
{
	short LobbyId;
	short LobbyUser = 0;
	std::wstring LobbyButtonName;
};


struct LobbyList : MyApp::Scene 
{
	GUI m_LobbyGui;
	GUI m_InfoGui;
	int m_LastDataVersion = -1;
	int m_AbleLobbyNumber = 0;
	Font m_Font = Font(15);

	std::vector<std::shared_ptr<LobbyInfo>> m_LobbyVector;

	/* Required Initializer for Siv3D */
	void init() override;

	/* Required Function for Siv3d */
	void update() override;

	/* Required Function for Siv3d */
	void draw() const override;

	/* Data Handling */
	void RefreshData();
	bool IsMyDataNeedRefreshed();
	void DrawConnectAbleLobbyInfo() const;

	/* Make text field accord with set lobby info */
	void MakeLobbys();

	/* Function which must be called when the scene changed */
	void ExitScene();

	/* Check which button clicked */
	void CheckButtonClicked();

	/* Request Lobby Info */
	void ReqLobbyInfo();
	
	/* Send Lobby Enter */
	void ReqLobbyEnter(short);
};
