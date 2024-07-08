#pragma once
#include <numeric>
#include <glm.hpp>

#define M_PI 3.14159265358979323846264338327950288
#define RADIANS(degrees) (0.0174533f * degrees)

template <typename T> 
int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

bool AreSame(float a, float b, float tolerance = 10e-5);

bool PointIsSame(const glm::vec2& pt1, const glm::vec2& pt2, float tolerance = 10e-5);

bool RectTest(const CRect& rect, const glm::vec2& pos);

bool DoIntersect(const glm::vec2& p1, const glm::vec2& q1, const glm::vec2& p2, const glm::vec2& q2);

glm::vec3 CalcNormal(const glm::vec3& t1, const glm::vec3& t2, const glm::vec3& t3);

class LineSegment2D
{
public:
	LineSegment2D(glm::vec2 p1, glm::vec2 p2);
	static bool Intersects(const LineSegment2D& line, const LineSegment2D& line2, glm::vec2* intersectpt = nullptr);
private:
	glm::vec2 m_P1;
	glm::vec2 m_P2;
};