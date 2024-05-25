#pragma once
#include <Windows.h>

class IGetWindowRect
{
public:
	virtual void GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r) const = 0;
};