#include "pch.h"
#include "MiscFunctions.h"

bool RectTest(const CRect& rect, const glm::vec2& pos)
{
	if (pos.x >= rect.left && pos.x <= rect.right)
	{
		if (pos.y >= rect.top && pos.y <= rect.bottom)
		{
			return true;
		}
	}
	return false;
}