#pragma once
#include "EngineLib.h"
#include <glm/glm.hpp>

struct Sphere
{
	glm::vec3 center;
	float radius;
};

Sphere RitterBoundingSphere(const EVec<glm::vec3> points);
