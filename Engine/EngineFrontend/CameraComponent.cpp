#include "CameraComponent.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "MeshComponent.h"
#include "IArchive.h"
#include "TransformComponent.h"

glm::mat4x4 CameraComponent::GetView(const Transform& t)
{
	if (bViewMatrixDirty)
	{
		viewMatrix = glm::inverse(t.getModelMatrix());//glm::lookAt(t.getGlobalPosition(), t.getGlobalPosition() + glm::normalize(t.getForward()), glm::normalize(worldUp));
		bViewMatrixDirty = false;
	}
	return viewMatrix;
}

glm::mat4x4 CameraComponent::GetProj()
{
	return glm::perspective(fovY, aspect, zNear, zFar);
}
void CameraComponent::GetMeshEntitiesInFrustum(EVec<Entity>& outEntities, const Transform& thisTransfom)
{
	outEntities.clear();
	
	const EntityReg& r = Scn::GetScene().entities.GetReg();
	
	auto v = r.view<Transform, MeshComponent>();
	
	Frustum f = CreateFrustumFromCamera(thisTransfom, *this, aspect, fovY, zNear, zFar);

	v.each([&](const Entity entity, const Transform& t, const MeshComponent& m) {
		Sphere bs = m.GetMeshBoundingSphere();
		if (SphereOnFrustum(f, bs, t))
		{
			outEntities.push_back(entity);
		}
	});
}

Frustum CameraComponent::GetFrustum(const Transform& thisTransfom)
{
	return CreateFrustumFromCamera(thisTransfom, *this, aspect, fovY, zNear, zFar);
}

void CameraComponent::OnResize(int newW, int newH)
{
	auto v = Scn::GetScene().entities.GetReg().view<CameraComponent>();
	v.each([newW, newH](CameraComponent& cam) {
		if (cam.bScreenAspect)
		{
			cam.aspect = (float)newW / (float)newH;
		}
	});
}

static void Meta1(Comp::ComponentMeta* m)
{

}

static void SerializeCs(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg)
{
	const i32 ver = 1;
	if (ar->IsStoring())
	{
		CameraComponent* cam = (reg.try_get<CameraComponent>(e));
		if (!cam)
		{
			return;
		}
		ar->PushObj("CameraComponent");
			*ar << ver;
			ar->PushObj("Name");
				*ar << cam->Name;
			ar->PopObj();
			ar->PushObj("aspect");
				*ar << cam->aspect;
			ar->PopObj();
			ar->PushObj("fovY");
				*ar << cam->fovY;
			ar->PopObj();
			ar->PushObj("zNear");
				*ar << cam->zNear;
			ar->PopObj();
			ar->PushObj("zFar");
				*ar << cam->zFar;
			ar->PopObj();
			ar->PushObj("bIsMain");
				*ar << cam->bIsMain;
			ar->PopObj();
			ar->PushObj("bScreenAspect");
				*ar << cam->bScreenAspect;
			ar->PopObj();
			ar->PushObj("worldUp");
				*ar << cam->worldUp;
			ar->PopObj();
		ar->PopObj();
	}
	else
	{
		if (!ar->PushObj("CameraComponent")) return;
			reg.emplace<CameraComponent>(e);
			CameraComponent* cam = reg.try_get<CameraComponent>(e);
			i32 fileVersion;
			*ar >> fileVersion;
			switch (fileVersion)
			{
			case 1:
			{
				ar->PushObj("Name");
					*ar >> cam->Name;
				ar->PopObj();
				ar->PushObj("aspect");
					*ar >> cam->aspect;
				ar->PopObj();
				ar->PushObj("fovY");
					*ar >> cam->fovY;
				ar->PopObj();
				ar->PushObj("zNear");
					*ar >> cam->zNear;
				ar->PopObj();
				ar->PushObj("zFar");
					*ar >> cam->zFar;
				ar->PopObj();
				ar->PushObj("bIsMain");
					*ar >> cam->bIsMain;
				ar->PopObj();
				ar->PushObj("bScreenAspect");
					*ar >> cam->bScreenAspect;
				ar->PopObj();
				ar->PushObj("worldUp");
					*ar >> cam->worldUp;
				ar->PopObj();
				break;
			}
			default:

				break;
			}
		ar->PopObj();
	}

}

META_IMPL(CameraComponent, Meta1, SerializeCs)