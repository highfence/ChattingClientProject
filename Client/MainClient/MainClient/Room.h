#pragma once
#include <vector>
#include <list>
#include <string>

struct Room : MyApp::Scene
{
	GUI m_ChattingGui;
	GUI m_UserGui;
	GUI m_InputGui;
	std::vector<UserInfo*> m_UserInfoVector;
	std::list<std::wstring> m_ChatList;
	String m_ChatString;
	int m_CurrentDataVersion = -1;

	void init() override;
	
	void update() override;

	void draw() const override;

	void InitialUserDataSetting();

	// Network Request
	void SendChatting(std::wstring& chatMsg);
};