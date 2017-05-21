#pragma once
#include <stdio.h>
#include <string>
#include <stack>
#include "Definition.h"

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

		static std::wstring CharToWstring(const char* pszText)
		{
			int textLength = strlen(pszText) + 1;
			wchar_t wTemp[COMMON_INFO::MAX_PACKET_SIZE];
			mbstowcs_s(0, wTemp, textLength, pszText, _TRUNCATE);
			
			return std::wstring(wTemp);
		}

	private :
	
	};

	template <class T>
	class ObjectPool
	{
	public :
		ObjectPool() = delete;
		ObjectPool(size_t objectNumber);

	private :
		std::stack<std::shared_ptr<T>> m_Stack;
	};

	template<class T>
	inline ObjectPool<T>::ObjectPool(size_t objectNumber)
	{
		for (int i = 0; i < objectNumber; ++i)
		{
			T obj = std::make_shared<T>();
			m_Stack.push(obj);
		}
	}
}
