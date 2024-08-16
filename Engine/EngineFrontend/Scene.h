#pragma once
#include "EngineLib.h"
#include "PipelineTypes.h"
#include <entt/entt.hpp>

namespace Scn
{

	struct Scene
	{
		entt::registry Reg;
		EVec<HMesh> meshes;
		entt::entity rootTransformEntity;
	};

	bool IsSceneLoaded();
	Scene& GetScene();
}