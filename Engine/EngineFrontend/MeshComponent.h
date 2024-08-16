#pragma once
#include "PipelineTypes.h"
#include <glm/glm.hpp>
#include <entt/entt.hpp>

struct Transform;

class MeshComponent
{
public:
	MeshComponent(HMesh mesh, HPipeline pipeline, entt::entity e);
	~MeshComponent();

	HDrawable GetDrawable() const { return m_hDrawable; }

private:
	HDrawable m_hDrawable = ENGINE_NULL_HANDLE;
};