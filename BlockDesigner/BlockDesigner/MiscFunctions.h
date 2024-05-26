#pragma once
#include <numeric>
#include <glm.hpp>

bool AreSame(float a, float b, float tolerance = 10e-5);

bool PointIsSame(const glm::vec2& pt1, const glm::vec2& pt2, float tolerance = 10e-5);

bool RectTest(const CRect& rect, const glm::vec2& pos);

bool DoIntersect(const glm::vec2& p1, const glm::vec2& q1, const glm::vec2& p2, const glm::vec2& q2);

glm::vec3 CalcNormal(const glm::vec3& t1, const glm::vec3& t2, const glm::vec3& t3);