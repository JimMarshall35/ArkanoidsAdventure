#include "Scene.h"
#include <entt/entt.hpp>
#include <optional>

namespace Scn
{
	static std::optional<Scene> gTheScene;

	bool IsSceneLoaded()
	{
		return gTheScene.has_value();
	}

	Scene& GetScene()
	{
		gTheScene.value();
	}
}
