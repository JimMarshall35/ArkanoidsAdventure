#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <entt/entt.hpp>
#include "Scene.h"
#include "EngineLib.h"
#include "FrontendError.h"
#include "ComponentReg.h"
#include "ExportMacro.h"

class ENGINE_FRONTEND_API Transform
{
protected:
	//Local space information
	glm::vec3 m_pos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_eulerRot = { 0.0f, 0.0f, 0.0f }; //In degrees
	glm::vec3 m_scale = { 1.0f, 1.0f, 1.0f };

	//Global space information concatenate in matrix
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);

	//Dirty flag
	bool m_bIsDirty = true;

	entt::entity m_hParent = entt::null;
	EVec<entt::entity> m_children;
protected:
	glm::mat4 getLocalModelMatrix();
	
public:
	//friend bool operator==(const Transform& a, const Transform& b);
	static void AddChild(entt::entity parent, entt::entity child);
	void computeModelMatrix();
	void setLocalPosition(const glm::vec3& newPosition);
	void setLocalRotation(const glm::vec3& newRotation);
	void setLocalScale(const glm::vec3& newScale);
	const glm::vec3& getGlobalPosition() const;
	const glm::vec3& getLocalPosition() const;
	const glm::vec3& getLocalRotation() const;
	const glm::vec3& getLocalScale() const;
	const glm::mat4& getModelMatrix() const;
	glm::mat4& getModelMatrixMut();
	glm::vec3 getRight() const;
	glm::vec3 getUp() const;
	glm::vec3 getBackward() const;
	glm::vec3 getForward() const;
	glm::vec3 getGlobalScale() const;
	void Rotate(float x, float y, float z);
	void Translate(float x, float y, float z);

	bool isDirty() const;
	static void SerializeC(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg);
};

//bool operator==(const Transform& a, const Transform& b)
//{
//	bool bChildrenSame = a.m_children.size() == b.m_children.size();
//	if (bChildrenSame)
//	{
//		for (int i = 0; i < a.m_children.size(); i++)
//		{
//			if (a.m_children[i] != b.m_children[i])
//			{
//				bChildrenSame = false;
//			}
//		}
//	}
//	return
//		bChildrenSame &&
//		a.m_pos == b.m_pos &&
//		a.m_eulerRot == b.m_eulerRot &&
//		a.m_scale == b.m_scale &&
//		a.m_modelMatrix == b.m_modelMatrix &&
//		a.m_isDirty == b.m_isDirty &&
//		a.m_hParent == b.m_hParent;
//}

META_DECL(Transform)
