#pragma once
#include <Windows.h>
#include <glm.hpp>
#include "IGetWindowRect.h"

class IMouseMoveAcceptor : public IGetWindowRect
{
public:
	virtual void UpdateMousePos(const glm::vec2& lastPt) = 0;
};