#include "DrawMeshComponentsFunction.h"
#include "SystemsReg.h"
#include "Scene.h"
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include <entt/entt.hpp>
#include "PipelineTypes.h"
#include "EngineDLLInterface.h"
#include "IBackendApp.h"
#include "FrontendError.h"

static void Draw(Scn::Scene& s)
{
	EntityReg& reg = s.entities.GetReg();
	CameraComponent& cam = reg.get<CameraComponent>(s.activeCameraAntity);
	Transform& trans = reg.get<Transform>(s.activeCameraAntity);
	Frustum frust = cam.GetFrustum(trans);

	auto v = reg.view<MeshComponent, Transform>();
	EVec<HDrawable> drawables;
	v.each([&drawables, &frust](MeshComponent& mc, Transform& trans) {
		if (SphereOnFrustum(frust, mc.GetMeshBoundingSphere(), trans))
		{
			HDrawable d = mc.GetDrawable();
			EAssert(d != ENGINE_NULL_HANDLE);
			drawables.push_back(d);
		}
	});
	//Err::LogInfo("%i drawables drawn", drawables.size());
	if(drawables.size())
		Engine::GetAPI().DrawDrawables(&drawables[0], drawables.size());
}

static DrawSystem gDrawMeshComponentSystem(&Draw, DrawFunctionGroup::Draw1, DRAW_MESH_COMPONENT_SYSTEM_NAME);