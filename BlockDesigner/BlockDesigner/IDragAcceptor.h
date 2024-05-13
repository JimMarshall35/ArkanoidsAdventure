#pragma once
#include "afx.h"
#include <glm.hpp>
enum class MouseButton
{
	None = 0,
	Middle = 1,
	Left = 2,
	Right = 4
};

inline MouseButton operator|(MouseButton a, MouseButton b)
{
	return static_cast<MouseButton>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline void operator|=(MouseButton& a, MouseButton b)
{
	a = static_cast<MouseButton>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline void operator&=(MouseButton& a, MouseButton b)
{
	a = static_cast<MouseButton>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}
inline unsigned int operator&(MouseButton a, MouseButton b)
{
	return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

inline unsigned int operator~(MouseButton a)
{
	return ~(static_cast<unsigned int>(a));
}

class IDragAcceptor
{
public:
	virtual void StartDrag(const glm::vec2& pt, MouseButton btn) = 0;
	virtual void StopDrag(const glm::vec2& pt) = 0;
	virtual void UpdateDrag(const glm::vec2& lastPt) = 0;
	virtual void GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r) = 0;
};