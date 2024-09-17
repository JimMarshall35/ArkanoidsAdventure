#pragma once
#include "PipelineTypes.h"
#include "ComponentReg.h"

struct TestPipelineMaterial
{
	float ambientStrength;
	float speculatStrength;
	float diffuseStrength;
	float shininess;
	HTexture hTexture;
};

META_DECL(TestPipelineMaterial)