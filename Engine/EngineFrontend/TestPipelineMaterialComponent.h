#pragma once
#include "PipelineTypes.h"
#include "ComponentReg.h"

struct TestPipelineMaterial
{
	float ambientStrength;
	float specularStrength;
	float diffuseStrength;
	float shininess;
	HTexture hTexture;
};

META_DECL(TestPipelineMaterial)