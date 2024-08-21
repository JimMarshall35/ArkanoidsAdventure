#include "CameraComponent.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "Frustum.h"
#include "MeshComponent.h"

void CameraComponent::GetMeshEntitiesInFrustum(EVec<Entity>& outEntities, const Transform& thisTransfom) const
{
	outEntities.clear();
	
	const EntityReg& r = Scn::GetScene().entities.GetReg();
	
	auto v = r.view<Transform, MeshComponent>();
	
	Frustum f = CreateFrustumFromCamera(thisTransfom, aspect, fovY, zNear, zFar);

	v.each([&](const Entity entity, const Transform& t, const MeshComponent m) {
		Sphere bs = m.GetMeshBoundingSphere();
		if (SphereOnFrustum(f, bs, t))
		{
			outEntities.push_back(entity);
		}
	});
}
