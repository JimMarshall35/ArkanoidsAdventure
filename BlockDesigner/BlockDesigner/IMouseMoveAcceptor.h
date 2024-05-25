#pragma once
#include <Windows.h>
#include <glm.hpp>

class IMouseMoveAcceptor
{
public:
	virtual void UpdateMousePos(const glm::vec2& lastPt) = 0;
	virtual void GetWindowRectForMove(LONG& t, LONG& l, LONG& b, LONG& r) const = 0;
};