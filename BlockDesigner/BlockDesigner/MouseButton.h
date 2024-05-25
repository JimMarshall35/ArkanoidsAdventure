#pragma once

enum class MouseButton
{
	Middle,
	Left,
	Right,
	NumMouseButtons
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
