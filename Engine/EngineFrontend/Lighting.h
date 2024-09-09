#pragma once
#include <glm/glm.hpp>
struct PointLightComponent;

namespace Light
{
	int GetPointLights(PointLightComponent* outPLC, glm::vec3* positions, size_t maxRequested);
}