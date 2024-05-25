#pragma once
#include <numeric>
#include <glm.hpp>

inline bool AreSame(float a, float b, float tolerance = 10e-5)
{
	float diff = fabs(a - b);
	return diff < tolerance;
}

bool RectTest(const CRect& rect, const glm::vec2& pos);