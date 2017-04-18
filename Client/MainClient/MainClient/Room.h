#pragma once

struct Room : MyApp::Scene
{
	GUI m_ChattingGui;
	std::vector<UserInfo*> m_UserInfoVector;

	void init() override;
	
	void update() override;

	void draw() const override;

	void setUserData();

	void drawUser();
};