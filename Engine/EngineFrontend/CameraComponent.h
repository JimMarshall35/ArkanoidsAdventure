#pragma once
#include "EngineLib.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Frustum.h"
#include "ComponentReg.h"

class Transform;

struct CameraComponent
{
	float aspect;
	float fovY;
	float zNear;
	float zFar;
	bool bIsMain = false;
	bool bScreenAspect = true;
	glm::vec3 worldUp = { 0, 0, 1 };
	glm::mat4x4 viewMatrix;
	bool bViewMatrixDirty = true;
	EString Name;
	glm::mat4x4 GetView(const Transform& t);
	glm::mat4x4 GetProj();
	void GetMeshEntitiesInFrustum(EVec<Entity>& outEntities, const Transform& thisEntity);
	Frustum GetFrustum(const Transform& thisTransfomt);
	static void OnResize(int newW, int newH);

};



META_DECL(CameraComponent)