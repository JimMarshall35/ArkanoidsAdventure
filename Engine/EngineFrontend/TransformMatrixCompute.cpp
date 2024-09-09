#include "TransformMatrixCompute.h"
#include "Scene.h"
#include "TransformComponent.h"

static void UpdateTransformModelMatrix(Scn::Scene& s, double deltaT)
{
	EntityReg& r = s.entities.GetReg();
	auto v = r.view<Transform>();
	v.each([](Transform& t) -> void
	{
		if (t.isDirty())
		{
			t.computeModelMatrix();
		}
	});
}

static UpdateSystem gTransformMatrixUpdate = { &UpdateTransformModelMatrix, UpdateFunctionGroup::PostPhysics, TRANSFORM_MATRIX_UPDATE_SYSTEM_NAME };

