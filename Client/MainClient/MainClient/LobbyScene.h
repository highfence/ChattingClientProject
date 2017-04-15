#pragma once

struct Lobby : MyApp::Scene 
{
	GUI m_LobbyGui;
	GUI m_InfoGui;

	const int roomNameWidth = 300;
	const int infoWidth = 160;

	/* Required Initializer for Siv3D */
	void init() override
	{
		m_LobbyGui = GUI(GUIStyle::Default);
		m_InfoGui = GUI(GUIStyle::Default);

		/* Setting InfoGui Position & Background Color */
		Point infoGuiPos = Point(roomNameWidth + infoWidth, 0);
		m_InfoGui.setPos(infoGuiPos);
		Graphics::SetBackground(Color(100, 160, 200));

		// TODO :: data of Lobby & info window will be given by server
		/* Setting Lobby */
		m_LobbyGui.setTitle(L"Lobby");
		m_LobbyGui.add(L"RoomList1", GUIText::Create(L"Chat with me!", roomNameWidth));
		m_LobbyGui.addln(L"ENTER1", GUIButton::Create(L"ENTER"));

		/* Setting Info */
		m_InfoGui.setTitle(L"Info");
		m_InfoGui.addln(L"Name", GUIText::Create(L"Keun Won, Lee", infoWidth));
	}

	/* Required Function for Siv3d */
	void update() override
	{
		/* if lobby entry button pushed */
		if (m_LobbyGui.button(L"ENTER1").pushed)
		{
			/* changeScene to chattingRoom */
			changeScene(L"ChattingRoom");
		}
	}

	/* Required Function for Siv3d */
	void draw() const override
	{
		/* Empty */
	}
};