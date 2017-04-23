#include "stdafx.h"
#include "LoginScene.h"
#include "DataContainer.h"
#include "Util.h"

void Login::init()
{
	using namespace ClientLogic;
	m_LoginGui = GUI(GUIStyle::Default);
	Graphics::SetBackground(Color(160, 200, 100));

	m_LoginGui.setTitle(L"Login!");

	/* 텍스트 필드 */
	m_LoginGui.add(L"idLabel", GUIText::Create(L"ID :   "));
	m_LoginGui.addln(L"idField", GUITextField::Create(none));
	m_LoginGui.add(L"passwordLabel", GUIText::Create(L"PW : "));
	m_LoginGui.addln(L"passwordField", GUITextField::Create(none));

	/* 버튼 구성 */
	m_LoginGui.add(L"loginBtn", GUIButton::Create(L"LOGIN"));
	m_LoginGui.addln(L"exitBtn", GUIButton::Create(L"EXIT"));

	/* 창 위치 지정 */
	m_LoginGui.setCenter(Window::Center());

	m_data->dataContainer = new DataContainer;
	m_data->dataContainer->Init();
	m_data->dataContainer->ConnectRequest();
}

void Login::update()
{
	/* Exit button pushed */
	if (m_LoginGui.button(L"exitBtn").pushed)
	{
		System::Exit();
	}
	/* Login button pushed */
	if (m_LoginGui.button(L"loginBtn").pushed)
	{
		TryLogin();
	}
	/* Enter key pushed */
	if (Input::KeyEnter.clicked)
	{
		TryLogin();
	}
	m_data->dataContainer->Update();
	CheckLoginSuccessed();
}

void Login::CheckLoginSuccessed()
{
	if (m_data->dataContainer->GetLoginData()->GetLoginSuccessed())
	{
		OutputDebugString(L"로그인 성공.");
		changeScene(L"Lobby");
	}
}

void Login::draw() const
{
	/* Empty */
}

bool Login::TryLogin()
{
	/* Save Id & Password */
	m_IdStr = m_LoginGui.textField(L"idField").text;
	m_PasswordStr = m_LoginGui.textField(L"passwordField").text;

	/* Making Send Packet */
	PktLogInReq newLoginReq;
	MakeLoginReqPacket(&newLoginReq);

	m_data->dataContainer->SendRequest((short)PACKET_ID::LOGIN_IN_REQ, sizeof(newLoginReq), (char*)&newLoginReq);
	return true;
}

void Login::MakeLoginReqPacket(PktLogInReq* packet)
{
	char szID[MAX_USER_ID_SIZE] = { 0, };
	char szPW[MAX_USER_PASSWORD_SIZE] = { 0, };

	ClientLogic::Util::UnicodeToAnsi(m_IdStr.c_str(), MAX_USER_ID_SIZE, szID);
	ClientLogic::Util::UnicodeToAnsi(m_PasswordStr.c_str(), MAX_USER_PASSWORD_SIZE, szPW);

	strncpy_s(packet->szID, MAX_USER_ID_SIZE + 1, szID, MAX_USER_ID_SIZE);
	strncpy_s(packet->szPW, MAX_USER_PASSWORD_SIZE + 1, szPW, MAX_USER_PASSWORD_SIZE);
}