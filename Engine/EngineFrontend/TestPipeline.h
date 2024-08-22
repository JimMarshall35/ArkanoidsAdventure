#pragma once
#include "PipelineTypes.h"
#include <glm/glm.hpp>
#include "Pipeline.h"
#include "ExportMacro.h"
#include "ComponentReg.h"

struct TestPipelineMaterial
{
	glm::mat4 m;
	float ambientStrength;
	float speculatStrength;
	float diffuseStrength;
	float shininess;
	HTexture hTexture;
};

META_DECL(TestPipelineMaterial)

class TestPipeline : public PipeLine
{
public:

	TestPipeline();
	// Inherited via PipeLine
	virtual void PerDrawUniform(int pipelineStage) override;

	virtual void PerInstanceUniform(int pipelineStage, HDrawable drawable, Entity e) override;
	virtual void Create() override;
private:
	HPipelineUniformProperty hLightPos, hLightColour, hModel, hView, hProjection, hAmbientStrength, hSpecularStrength, hDiffuseStrength, hShininess, hDiffuseAtlas;

};
