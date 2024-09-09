#pragma once
#include "EngineLib.h"
#include "PipelineTypes.h"
#include <entt/entt.hpp>
#include "PipelineMeshData.h"
#include "Texture.h"
#include "ExportMacro.h"

#include "Pipeline.h"
#include <glm/glm.hpp>
#include "EntityRegWrapper.h"
#include "MeshReg.h"
#include "TextureReg.h"
#include "SystemsReg.h"

class IArchive;

namespace Scn
{
	struct ENGINE_FRONTEND_API Scene
	{
		SystemsReg sysReg;
		TextureReg textureReg;
		MeshReg meshReg;
		EntityRegWrapper entities;
		Entity rootTransformEntity;
		Entity activeCameraAntity;
	};

	bool IsSceneLoaded();
	Scene& GetScene();

	void SerializeScene(IArchive& archive);
// DEBUG TEST: TO REMOVE BEGIN
	void ForceSceneLoadedTrue();
// DEBUG TEST: TO REMOVE END
}