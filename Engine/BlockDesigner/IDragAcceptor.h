#pragma once
#include "afx.h"
#include <glm.hpp>
#include "MouseButton.h"
#include "IGetWindowRect.h"

class IDragAcceptor : public IGetWindowRect
{
public:
	virtual void StartDrag(const glm::vec2& pt, MouseButton btn) = 0;
	virtual void StopDrag(const glm::vec2& pt) = 0;
	virtual void UpdateDrag(const glm::vec2& lastPt) = 0;
};