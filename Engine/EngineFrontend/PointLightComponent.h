#pragma once
#include "ExportMacro.h"
#include <glm/glm.hpp>
#include "EngineLib.h"
#include "ComponentReg.h"

struct PointLightComponent
{
	glm::vec3 colour;
};
META_DECL(PointLightComponent)

