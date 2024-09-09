#pragma once
#include "EngineLib.h"
#include <glm/glm.hpp>
class IArchive;

struct Sphere
{
	glm::vec3 center;
	float radius;
	void Serialize(IArchive& a);
};

Sphere RitterBoundingSphere(const EVec<glm::vec3>& points);
