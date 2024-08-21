#pragma once
#include "EngineLib.h"
#include <glm/glm.hpp>
class Transform;

struct CameraComponent
{
	EString Name;
	float aspect;
	float fovY;
	float zNear;
	float zFar;
	bool bIsMain = false;
	bool bScreenAspect = true;

	void GetMeshEntitiesInFrustum(EVec<Entity>& outEntities, const Transform& thisEntity) const;
};