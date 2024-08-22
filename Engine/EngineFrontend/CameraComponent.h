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
	glm::mat4x4 GetProj()
	{
		return glm::perspective(fovY, aspect, zNear, zFar);
	}
	void GetMeshEntitiesInFrustum(EVec<Entity>& outEntities, const Transform& thisEntity) const;
};