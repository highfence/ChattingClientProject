#pragma once

struct RoomList : MyApp::Scene
{
	GUI m_RoomListGui;
	GUI m_UserListGui;
	GUI m_ChattingGui;

	/* Required Initializer for Siv3D */
	void init() override;

	void update() override;

	void draw() const override;

	void refresh();
};