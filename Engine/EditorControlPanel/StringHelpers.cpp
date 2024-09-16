#include "pch.h"
#include "StringHelpers.h"
#include <windows.h>

std::wstring s2ws(const char* str)
{
	size_t len = strlen(str);
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)len, NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)len, &wstrTo[0], size_needed);
	return wstrTo;
}
