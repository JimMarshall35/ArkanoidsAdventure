#pragma once
#include "EngineLib.h"

namespace Scn
{
	struct Scene;
}

namespace BasicScn
{
	Entity Load(Scn::Scene& scn); // return value for expediency to test - returns the entity with the mesh
};