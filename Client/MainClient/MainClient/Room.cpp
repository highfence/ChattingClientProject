#include <string>
#include "stdafx.h"
#include "Room.h"

const std::wstring userName = L"User";
const int userMaxNumber = 6;

void Room::init()
{
#pragma region Gui Setting Functions

	// 유저 리스트의 데이터를 등록시켜준다.
	auto RegisterUserData = [this]()
	{
		for (const auto& i : m_UserInfoVector)
		{
			m_UserGui.addln(std::to_wstring(i->userIndex), GUIText::Create(L""));
		}
	};

#pragma endregion

	m_ChattingGui = GUI(GUIStyle::Default);
	m_ChattingGui.setTitle(L"Let's Chat!");

	m_UserGui = GUI(GUIStyle::Default);
	m_UserGui.setTitle(L"User List");
	m_UserGui.setPos(Point(475, 0));

	m_InputGui = GUI(GUIStyle::Default);
	m_InputGui.setPos(Point(0, 300));
	
	Graphics::SetBackground(Color(95, 0, 255));

	// Data Setting 
	InitialUserDataSetting();

	// Input data in GUI 
	RegisterUserData();

	m_ChattingGui.add(L"RoomChatting", GUITextArea::Create(10, 22));
	m_InputGui.add(L"InputField", GUITextArea::Create(1, 28));
	m_InputGui.add(L"InputButton", GUIButton::Create(L"Send"));

}

void Room::update()
{
#pragma region Update Functions

	// 데이터를 업데이트 해주는 함수.
	auto UpdateData = [this]()
	{
#pragma region UpdateData inner Functions

		// 최신 버전 데이터와 비교해서 업데이트가 필요한지 확인해주는 함수.
		auto IsDataUpdateNeeded = [this]()->bool
		{
			int lastestVersion = m_data->dataContainer->GetRoomData()->GetVersion();

			if (lastestVersion > m_CurrentDataVersion)
			{
				return true;
			}
			else return false;
		};

		// 데이터 리프레시 로그를 남겨주는 함수.
		auto LogDataRefresh = [this]()
		{
			std::wstring debugLabel = L"[Room] 데이터 리프레시! 현재 버전 : " + std::to_wstring(m_CurrentDataVersion) + L", 최신 버전 : "
				+ std::to_wstring(m_data->dataContainer->GetRoomData()->GetVersion()) + L"\n";

			OutputDebugString(debugLabel.c_str());
		};

		// 유저 벡터를 갱신하는 함수.
		auto UpdateUserData = [this]()
		{
			auto sourceData = m_data->dataContainer->GetRoomData()->GetRefUserInfoList();
			auto sourceDataSize = sourceData.size();

			// 데이터를 갱신하기 전 벡터를 비워준다.
			m_UserInfoVector.clear();
			m_UserInfoVector.reserve(sourceDataSize);

			// 데이터를 갱신해준다.
			int idx = 0;
			for (const auto& i : sourceData)
			{
				UserInfo* newUserInfo = new UserInfo;
				newUserInfo->userIndex = idx;
				newUserInfo->userName = i;
				newUserInfo->isUserInfoValid = true;

				m_UserInfoVector.emplace_back(std::move(newUserInfo));
				++idx;
			}
		};

		// 유저 GUI를 갱신하는 함수.
		auto UpdateUserGui = [this]()
		{
			for (const auto& i : m_UserInfoVector)
			{
				// Valid 하지 않은 데이터라면 텍스트를 없애준다.
				if (!i->isUserInfoValid)
				{
					m_UserGui.text(std::to_wstring(i->userIndex)).text = L"           ";

				}
				else
				{
					// Valid 한 데이터라면, index가 일치하는 텍스트의 이름을 바꾸어준다.				
					m_UserGui.text(std::to_wstring(i->userIndex)).text = i->userName;
				}
			}
		};

		// 버전을 업데이트 해주는 함수.
		auto UpdateVersion = [this]()
		{
			m_CurrentDataVersion = m_data->dataContainer->GetRoomData()->GetVersion();
		};

#pragma endregion

		// 업데이트가 필요없다면, 함수를 끝낸다.
		if (!IsDataUpdateNeeded()) return;
		// 업데이트가 필요하다면, 로그를 남긴다.
		else LogDataRefresh();

		// 유저벡터를 갱신한다.
		UpdateUserData();

		// 갱신한 데이터에 상응하여 GUI를 바꾸어준다.
		UpdateUserGui();

		// 업데이트가 모두 끝났으므로 버전을 갱신시킨다.
		UpdateVersion();
	};

	// Send 버튼이 눌렸는지 체크하는 함수.
	auto CheckSendButtonPushed = [this]()
	{
		if (m_InputGui.button(L"InputButton").pressed || Input::KeyEnter.clicked)
		{
			if (m_InputGui.textArea(L"InputField").text == L"")
			{
				return;
			}

			if (m_ChatString == L"")
			{
				m_ChatString = m_ChatString + m_InputGui.textArea(L"InputField").text;
			}
			else
			{
				m_ChatString = m_ChatString + L"\n" + m_InputGui.textArea(L"InputField").text;
			}

			m_ChattingGui.textArea(L"RoomChatting").setText(m_ChatString);
			m_InputGui.textArea(L"InputField").setText(L"");
		}
	};

#pragma endregion

	m_data->dataContainer->Update();

	UpdateData();
	CheckSendButtonPushed();
}

void Room::draw() const
{
}

void Room::InitialUserDataSetting()
{
	m_UserInfoVector.clear();
	m_UserInfoVector.reserve(userMaxNumber);

	for (int i = 0; i < userMaxNumber; ++i)
	{
		UserInfo* userData = new UserInfo;
		userData->userIndex = i;
		userData->userName = L"           ";
		userData->isUserInfoValid = false;

		m_UserInfoVector.emplace_back(std::move(userData));
	}
}
