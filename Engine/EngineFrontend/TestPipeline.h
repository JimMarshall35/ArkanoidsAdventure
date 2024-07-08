#pragma once
#include "PipelineTypes.h"
#include <glm/glm.hpp>
struct BackendAPI;
class PipeLine;

struct PerDrawUniforms
{
	glm::vec3 lightPos;
	glm::vec3 lightColour;
	glm::mat4 v;
	glm::mat4 p;
};

struct PerInstanceUniforms
{
	glm::mat4 m;
	float ambientStrength;
	float speculatStrength;
	float diffuseStrength;
	float shininess;
	HTexture hTexture;
};

HPipeline BuildTestPipeline(PipeLine& outPipe, const BackendAPI& api);

HDrawable GetTestPipelineDrawable(HMesh mesh, PerInstanceUniforms* pUniformPtr);


PerDrawUniforms& GetPipelineUniforms();