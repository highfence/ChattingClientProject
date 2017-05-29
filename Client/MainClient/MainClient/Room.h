#pragma once

struct Room : MyApp::Scene
{
	GUI m_ChattingGui;
	GUI m_UserGui;
	GUI m_InputGui;
	std::vector<std::shared_ptr<UserInfo>> m_UserInfoVector;
	String m_ChatString;

	void init() override;
	
	void update() override;

	void draw() const override;

	void InitialUserDataSetting();

	void CheckSendPushed();

};