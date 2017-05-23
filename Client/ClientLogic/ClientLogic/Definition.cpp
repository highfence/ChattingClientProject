#include "Definition.h"

ChatData::ChatData(std::wstring id, std::wstring chat)
	: m_Id(id), m_Chat(chat)
{
	// chatData�� ������� ������ �ð� ����.
	m_ChatTime = std::chrono::system_clock::now();
}

void ChatData::DataSetting(std::wstring id, std::wstring chat)
{
	m_Id = id;
	m_ChatTime = std::chrono::system_clock::now();
	m_Chat = chat;
}

std::wstring ChatData::GetInLine()
{
	std::wstring chatData;
	
	/* �ð� ������ ���� */
	std::time_t tTime = std::chrono::system_clock::to_time_t(m_ChatTime);

	// ���� ������ ����(0�� 0�� 0��)�� ����� ����.
	std:tm tmToday;
	tmToday.tm_year = std::localtime(&tTime)->tm_year;
	tmToday.tm_mon = std::localtime(&tTime)->tm_mon;
	tmToday.tm_mday = std::localtime(&tTime)->tm_mday;
	tmToday.tm_hour = 0;
	tmToday.tm_min = 0;
	tmToday.tm_sec = 0;

	// time_point ������ ���� ������ �ð��� �޴´�.
	std::chrono::system_clock::time_point todayMidnight = std::chrono::system_clock::from_time_t(std::mktime(&tmToday));

	// ���� �ð� ���.
	auto hour = std::chrono::duration_cast<std::chrono::hours>(m_ChatTime - todayMidnight).count();
	auto min = std::chrono::duration_cast<std::chrono::minutes>(m_ChatTime - todayMidnight).count() - 60 * hour;
	auto sec = std::chrono::duration_cast<std::chrono::seconds>(m_ChatTime - todayMidnight).count() - 3600 * hour - 60 * min;

	std::wstring timeStr = L" [" + std::to_wstring(hour) + L":" + std::to_wstring(min) + L":" + std::to_wstring(sec) + L"]";

	// id [00:00:00] blah blah �� ���·� ä�� ������ �̾Ƴ���. 
	chatData = m_Id + timeStr + L" " + m_Chat + L"\n";

	return chatData;
}

UserData::UserData(int userNumber, std::wstring userId)
	: m_UserNumber(userNumber), m_UserId(userId) {}

