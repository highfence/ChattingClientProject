#pragma once

struct Login : MyApp::Scene
{
	GUI m_LoginGui;
	String m_IdStr;
	String m_PasswordStr;

	/* Required Initializer for Siv3D */
	void init() override
	{
		m_LoginGui = GUI(GUIStyle::Default);
		Graphics::SetBackground(Color(160, 200, 100));

		m_LoginGui.setTitle(L"Login!");

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
	}

	/* Required Function for Siv3d */
	void update() override
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
	}

	/* Required Function for Siv3d */
	void draw() const override
	{
		/* Empty */
	}

	/* �α��� �õ� �Լ� */
	bool tryLogin()
	{
		/* Save Id & Password */
		m_IdStr = m_LoginGui.textField(L"idField").text;
		m_PasswordStr = m_LoginGui.textField(L"PasswordField").text;

		/* If connect successed, change Lobby Scene */
		changeScene(L"Lobby");
		return true;
	}
};