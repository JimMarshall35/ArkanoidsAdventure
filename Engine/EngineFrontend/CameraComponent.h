#pragma once
#include "EngineLib.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Frustum.h"
#include "ComponentReg.h"

class Transform;

struct CameraComponent
{
	/*EString Name;
	float aspect;
	float fovY;
	float zNear;
	float zFar;
	bool bIsMain = false;
	bool bScreenAspect = true;
	glm::vec3 worldUp = {0, 0, 1};
	glm::vec3 lookAt = { 0, 0, 1 };
	glm::mat4x4 viewMatrix;
	bool bViewMatrixDirty = true;
	glm::mat4x4 GetProj()
	{
		return glm::perspective(fovY, aspect, zNear, zFar);
	}
	glm::mat4x4 GetView(const Transform& t);
	static void OnResize(int newW, int newH);
	glm::vec3 GetRightVector(const Transform& t) { return glm::transpose(GetView(t))[0]; }
	glm::vec3 GetFrontVector(const Transform& t);*/
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