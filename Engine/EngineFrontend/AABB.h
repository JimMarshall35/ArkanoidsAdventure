#pragma once
#include <glm/glm.hpp>

struct AABB
{
	AABB() {}
	AABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
		: center{ inCenter }, extents{ iI, iJ, iK }
	{}


	glm::vec3 center{ 0.f, 0.f, 0.f };
	glm::vec3 extents{ 0.f, 0.f, 0.f };

};