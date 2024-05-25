#pragma once
#include "afx.h"
#include <glm.hpp>
#include "MouseButton.h"


class IDragAcceptor
{
public:
	virtual void StartDrag(const glm::vec2& pt, MouseButton btn) = 0;
	virtual void StopDrag(const glm::vec2& pt) = 0;
	virtual void UpdateDrag(const glm::vec2& lastPt) = 0;
	virtual void GetWindowRectForDrag(LONG& t, LONG& l, LONG& b, LONG& r) const = 0;
};