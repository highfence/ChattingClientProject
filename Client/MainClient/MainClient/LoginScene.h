#pragma once

struct Login : MyApp::Scene
{
	GUI m_LoginGui;
	String m_IdStr;
	String m_PasswordStr;

	/* Required Initializer for Siv3D */
	void init() override;

	/* Required Function for Siv3d */
	void update() override;
	void CheckLoginSuccessed();

	/* Required Function for Siv3d */
	void draw() const override;

	/* 로그인 관련 함수 */
	bool TryLogin();
	void MakeLoginReqPacket(PktLogInReq*);
};