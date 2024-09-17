#pragma once
#include "TransformComponent.h"
#include "IArchive.h"
#include <cmath>

glm::mat4 Transform::getLocalModelMatrix()
{
	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(m_eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	// Y * X * Z
	const glm::mat4 rotationMatrix = transformY * transformX * transformZ;
	// translation * rotation * scale (also know as TRS matrix)
	return glm::translate(glm::mat4(1.0f), m_pos) * rotationMatrix * glm::scale(glm::mat4(1.0f), m_scale);
}

void Transform::AddChild(entt::entity parent, entt::entity child)
{
	//EAssert(Scn::IsSceneLoaded());
	Scn::Scene& scn = Scn::GetScene();
	Transform* childTransform = scn.entities.GetReg().try_get<Transform>(child);
	Transform* parentTransform = scn.entities.GetReg().try_get<Transform>(parent);
	bool bOK = true;

	if (!childTransform)
	{
		bOK = false;
		Err::ReportError({ "Transform::AddChild: entity you're trying to add as a child transform to doesn't have transform component", Err::FrontendErrorSeverity::Warning });
	}

	if (!parentTransform)
	{
		bOK = false;
		Err::ReportError({ "Transform::AddChild: entity you're trying to add as a parent transform to doesn't have transform component", Err::FrontendErrorSeverity::Warning });
	}

	if (std::find(parentTransform->m_children.begin(), parentTransform->m_children.end(), child) != parentTransform->m_children.end())
	{
		bOK = false;
		Err::ReportError({ "Transform::AddChild already added as child", Err::FrontendErrorSeverity::Warning });
	}

	if (!bOK)
	{
		return;
	}

	if (childTransform->m_hParent != entt::null)
	{
		Transform* parentTransform = scn.entities.GetReg().try_get<Transform>(parent);
		auto itr = std::find(parentTransform->m_children.begin(), parentTransform->m_children.end(), child);
		parentTransform->m_children.erase(itr);
	}

	childTransform->m_hParent = parent;
	parentTransform->m_children.push_back(child);
}

void Transform::setLocalPosition(const glm::vec3& newPosition)
{
	m_pos = newPosition;
	m_bIsDirty = true;
}

void Transform::setLocalRotation(const glm::vec3& newRotation)
{
	m_eulerRot = newRotation;
	m_bIsDirty = true;
}

void Transform::setLocalScale(const glm::vec3& newScale)
{
	m_scale = newScale;
	m_bIsDirty = true;
}

const glm::vec3& Transform::getLocalPosition() const
{
	return m_pos;
}

const glm::vec3& Transform::getGlobalPosition() const
{
	return m_modelMatrix[3];
}
const glm::vec3& Transform::getLocalRotation() const
{
	return m_eulerRot;
}
glm::vec3 Transform::getRight() const
{
	return m_modelMatrix[0];
}
void Transform::computeModelMatrix()
{
	m_modelMatrix = getLocalModelMatrix();
	if (m_hParent != entt::null)
	{
		EAssert(Scn::IsSceneLoaded());
		Scn::Scene& scn = Scn::GetScene();
		Transform* parentTransform = scn.entities.GetReg().try_get<Transform>(m_hParent);
		if (parentTransform->m_bIsDirty)
		{
			parentTransform->computeModelMatrix();
		}
		m_modelMatrix = parentTransform->m_modelMatrix * m_modelMatrix;
	}
	m_bIsDirty = false;
}

glm::vec3 Transform::getUp() const
{
	return m_modelMatrix[1];
}

glm::vec3 Transform::getBackward() const
{
	return m_modelMatrix[2];
}

const glm::vec3& Transform::getLocalScale() const
{
	return m_scale;
}

glm::vec3 Transform::getForward() const
{
	return -m_modelMatrix[2];
}

const glm::mat4& Transform::getModelMatrix() const
{
	return m_modelMatrix;
}

void Transform::Rotate(float x, float y, float z)
{
	m_eulerRot = {
		fmod(m_eulerRot.x + x,360.0),
		fmod(m_eulerRot.y + y,360.0),
		fmod(m_eulerRot.z + z,360.0)
	};
	m_bIsDirty = true;
}

void Transform::Translate(float x, float y, float z)
{
	m_pos.x += x;
	m_pos.y += y;
	m_pos.z += z;
	m_bIsDirty = true;
}

glm::vec3 Transform::getGlobalScale() const
{
	return { glm::length(getRight()), glm::length(getUp()), glm::length(getBackward()) };
}

bool Transform::isDirty() const
{
	return m_bIsDirty;
}
static void MetaReg(Comp::ComponentMeta* m)
{
	using namespace entt::literals;
	entt::meta_factory<Transform> factory = entt::meta<Transform>().type("TransformComponent"_hs);
	factory/*.ctor<HMesh, HPipeline, Entity>()*/
		.data<&Transform::setLocalPosition, &Transform::getLocalPosition, entt::as_is_t>("position"_hs)
		.data<&Transform::setLocalScale, &Transform::getLocalScale, entt::as_is_t>("scale"_hs)
		.data<&Transform::setLocalRotation, &Transform::getLocalRotation, entt::as_is_t>("rotation"_hs);

}

void Transform::SerializeC(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg)
{
	const i32 version = 1;
	
	if (ar->IsStoring())
	{
		Transform* t = reg.try_get<Transform>(e);
		if (!t) return;

		ar->PushObj("Transform");
			ar->PushObj("Version");
				*ar << 1;
			ar->PopObj();
			ar->PushObj("Pos");
				*ar << t->m_pos;
			ar->PopObj();
			ar->PushObj("Rotation");
				*ar << t->m_eulerRot;
			ar->PopObj();
			ar->PushObj("Scale");
				*ar << t->m_scale;
			ar->PopObj();
			ar->PushObj("Model");
				*ar << t->m_modelMatrix;
			ar->PopObj();
			ar->PushObj("IsDirty");
				*ar << t->m_bIsDirty;
			ar->PopObj();
			ar->PushObj("Parent");
				*ar << t->m_hParent;
			ar->PopObj();
			ar->PushObj("Children");
				*ar << t->m_children.size();
				for (Entity t : t->m_children)
				{
					ar->PushObj("Child");
					*ar << t;
					ar->PopObj();
				}
			ar->PopObj();
		ar->PopObj();
	}
	else
	{
		if (!ar->PushObj("Transform")) return;
			Transform* t = reg.try_get<Transform>(e);
			if (!t)
			{
				reg.emplace<Transform>(e);
				t = reg.try_get<Transform>(e);
			}
			ar->PushObj("Version");
				int version = 1;
				*ar >> version;
			ar->PopObj();
			switch (version)
			{
			case 1:
			{
				ar->PushObj("Pos");
					*ar >> t->m_pos;
				ar->PopObj();
				ar->PushObj("Rotation");
					*ar >> t->m_eulerRot;
				ar->PopObj();
				ar->PushObj("Scale");
					*ar >> t->m_scale;
				ar->PopObj();
				ar->PushObj("Model");
					*ar >> t->m_modelMatrix;
				ar->PopObj();
				ar->PushObj("IsDirty");
					*ar >> t->m_bIsDirty;
				ar->PopObj();
				ar->PushObj("Parent");
					*ar >> t->m_hParent;
				ar->PopObj();
				ar->PushObj("Children");
					size_t sz = 0;
					*ar >> sz;//t->m_children.size();
					t->m_children.resize(sz);
					for (Entity& t : t->m_children)
					{
						ar->PushObj("Child");
						*ar >> t;
						ar->PopObj();
					}
				ar->PopObj();
				break;
			}
			default:
				break;
			}
		ar->PopObj();

	}
}

META_IMPL(Transform, MetaReg, Transform::SerializeC)