#pragma once
#include <glm.hpp>
#include "MouseButton.h"
#include "IGetWindowRect.h"

class IMouseDownAcceptor : public IGetWindowRect
{
public:
	virtual void MouseDown(const glm::vec2& pt, MouseButton btn) = 0;
};