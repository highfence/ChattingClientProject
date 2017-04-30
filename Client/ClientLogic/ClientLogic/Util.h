#pragma once
#include <stdio.h>
#include <string>

namespace ClientLogic
{
	class Util
	{
	public:
		Util() = default;
		~Util() = default;

		static void UnicodeToAnsi(const wchar_t* pszText, const int destSize, char* pszDest)
		{
			_snprintf_s(pszDest, destSize, _TRUNCATE, "%S", pszText);
		}

		static void CharToWstring(const char* charText, const int charTextSize, std::wstring destString)
		{
			for (int i = 0; i < charTextSize; ++i)
			{
				destString = destString + std::to_wstring(charText[i]);
			}
		}


	};
}
