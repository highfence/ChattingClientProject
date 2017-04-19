#include "stdafx.h"
#include "LoginScene.h"

void Login::init()
{
	m_LoginGui = GUI(GUIStyle::Default);
	Graphics::SetBackground(Color(160, 200, 100));

	m_LoginGui.setTitle(L"Login!");
	packetProcess = new ClientLogic::PacketProcess();

	/* �ؽ�Ʈ �ʵ� */
	m_LoginGui.add(L"idLabel", GUIText::Create(L"ID :   "));
	m_LoginGui.addln(L"idField", GUITextField::Create(none));
	m_LoginGui.add(L"passwordLabel", GUIText::Create(L"PW : "));
	m_LoginGui.addln(L"passwordLabel", GUITextField::Create(none));

	/* ��ư ���� */
	m_LoginGui.add(L"loginBtn", GUIButton::Create(L"LOGIN"));
	m_LoginGui.addln(L"exitBtn", GUIButton::Create(L"EXIT"));

	/* â ��ġ ���� */
	m_LoginGui.setCenter(Window::Center());

	packetProcess = new ClientLogic::PacketProcess();
	clientNetwork = new ClientLogic::ClientNetwork();
	clientNetwork->Init();
	packetProcess->RegisterClientNetwork(clientNetwork);
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
		tryLogin();
	}
	/* Enter key pushed */
	if (Input::KeyEnter.clicked)
	{
		tryLogin();
	}
	packetProcess->Update();
	/* If connect successed, change Lobby Scene */
	if (packetProcess->GetIsLoginSuccessed())
	{
		packetProcess->SetIsLoginSuccedded(false);
		changeScene(L"Lobby");
	}
}

void Login::draw() const
{
	/* Empty */
}

bool Login::tryLogin()
{
	/* Save Id & Password */
	m_IdStr = m_LoginGui.textField(L"idField").text;
	m_PasswordStr = m_LoginGui.textField(L"PasswordField").text;

	if (!clientNetwork->Connect())
	{
		//
		OutputDebugString(L"Ŀ��Ʈ ����.");
	}
	else
	{
		clientNetwork->SendLogin(m_IdStr.c_str(), m_PasswordStr.c_str());
		OutputDebugString(L"Ŀ��Ʈ ����.");
	}


	return true;
}
