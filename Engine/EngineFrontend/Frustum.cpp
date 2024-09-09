#include "Frustum.h"
#include "Camera.h"
#include "Sphere.h"
#include "AABB.h"
#include "TransformComponent.h"
#include "CameraComponent.h"

Frustum CreateFrustumFromCamera(const Transform& cam, CameraComponent& comComp, float aspect, float fovY, float zNear, float zFar)
{
	Frustum     frustum;
	const float halfVSide = zFar * tanf(fovY * .5f);
	const float halfHSide = halfVSide * aspect;

	glm::vec3 front = glm::normalize(cam.getForward());
	glm::vec3 right = glm::normalize(cam.getRight());
	glm::vec3 up = glm::normalize(cam.getUp());

	const glm::vec3 frontMultFar = zFar * front;

	frustum.nearFace = { cam.getGlobalPosition() + zNear * (front), front };
	frustum.farFace = { cam.getGlobalPosition() + frontMultFar, -front };
	frustum.rightFace = { cam.getGlobalPosition(), glm::cross(frontMultFar - right * halfHSide, up) };
	frustum.leftFace = { cam.getGlobalPosition(), glm::cross(up,frontMultFar + right * halfHSide) };
	frustum.topFace = { cam.getGlobalPosition(), glm::cross(right, frontMultFar - up * halfVSide) };
	frustum.bottomFace = { cam.getGlobalPosition(), glm::cross(frontMultFar + up * halfVSide, right) };

	return frustum;
}

Frustum CreateFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar)
{
    Frustum     frustum;
    const float halfVSide = zFar * tanf(fovY * .5f);
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = zFar * cam.GetLookAt();

    frustum.nearFace = { cam.GetEye() + zNear * (cam.GetFront()), cam.GetFront() };
    frustum.farFace = { cam.GetEye() + frontMultFar, -cam.GetFront() };
    frustum.rightFace = { cam.GetEye(), glm::cross(frontMultFar - cam.GetRightVector() * halfHSide, cam.GetUpVector()) };
    frustum.leftFace = { cam.GetEye(), glm::cross(cam.GetUpVector(),frontMultFar + cam.GetRightVector() * halfHSide) };
    frustum.topFace = { cam.GetEye(), glm::cross(cam.GetRightVector(), frontMultFar - cam.GetUpVector() * halfVSide) };
    frustum.bottomFace = { cam.GetEye(), glm::cross(frontMultFar + cam.GetUpVector() * halfVSide, cam.GetRightVector()) };

    return frustum;
}

Frustum CreateFrustumFromCamera(const Transform& cam, float aspect, float fovY, float zNear, float zFar)
{
	Frustum     frustum;
	const float halfVSide = zFar * tanf(fovY * .5f);
	const float halfHSide = halfVSide * aspect;
	glm::vec3 lookat = cam.getForward() + cam.getGlobalPosition();

	const glm::vec3 frontMultFar = zFar * lookat;

	frustum.nearFace = { cam.getGlobalPosition() + zNear * (cam.getForward()), cam.getForward() };
	frustum.farFace = { cam.getGlobalPosition() + frontMultFar, -cam.getForward() };
	frustum.rightFace = { cam.getGlobalPosition(), glm::cross(frontMultFar - cam.getRight() * halfHSide, cam.getUp()) };
	frustum.leftFace = { cam.getGlobalPosition(), glm::cross(cam.getUp(),frontMultFar + cam.getRight() * halfHSide) };
	frustum.topFace = { cam.getGlobalPosition(), glm::cross(cam.getRight(), frontMultFar - cam.getUp() * halfVSide) };
	frustum.bottomFace = { cam.getGlobalPosition(), glm::cross(frontMultFar + cam.getUp() * halfVSide, cam.getRight()) };

	return frustum;
}


static bool SphereOnOrForwardplane(const Plane& p, const Sphere& s)
{
    return p.getSignedDistanceToPlane(s.center) > -s.radius;
}

static bool AABBOnOrForwardplane(const Plane& p, const AABB& a)
{
	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	const float r = a.extents.x * std::abs(p.normal.x) + a.extents.y * std::abs(p.normal.y) +
		a.extents.z * std::abs(p.normal.z);

	return -r <= p.getSignedDistanceToPlane(a.center);
}


bool SphereOnFrustum(const Frustum& f, const Sphere& s, const Transform& t)
{
	return true;
	//Get global scale thanks to our transform
	const glm::vec3 globalScale = t.getGlobalScale();

	//Get our global center with process it with the global model matrix of our transform
	const glm::vec3 globalCenter{ t.getModelMatrix() * glm::vec4(s.center, 1.f) };

	//To wrap correctly our shape, we need the maximum scale scalar.
	const float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

	//Max scale is assuming for the diameter. So, we need the half to apply it to our radius
	Sphere globalSphere{ globalCenter, s.radius * (maxScale * 0.5f) };

	//Check Firstly the result that have the most chance to failure to avoid to call all functions.
	return (SphereOnOrForwardplane(f.leftFace, s) &&
		SphereOnOrForwardplane(f.rightFace, s) &&
		SphereOnOrForwardplane(f.farFace, s) &&
		SphereOnOrForwardplane(f.nearFace, s) &&
		SphereOnOrForwardplane(f.topFace, s) &&
		SphereOnOrForwardplane(f.bottomFace, s));
}

bool AABBOnFrustum(const Frustum& f, const AABB& a, const Transform& t)
{
	//Get global scale thanks to our transform
	const glm::vec3 globalCenter{ t.getModelMatrix() * glm::vec4(a.center, 1.f) };

	// Scaled orientation
	const glm::vec3 right = t.getRight() * a.extents.x;
	const glm::vec3 up = t.getUp() * a.extents.y;
	const glm::vec3 forward = t.getForward() * a.extents.z;

	const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

	const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

	const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
		std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

	const AABB globalAABB(globalCenter, newIi, newIj, newIk);

	return (AABBOnOrForwardplane(f.leftFace, a) &&
		AABBOnOrForwardplane(f.rightFace, a) &&
		AABBOnOrForwardplane(f.topFace, a) &&
		AABBOnOrForwardplane(f.bottomFace, a) &&
		AABBOnOrForwardplane(f.nearFace, a) &&
		AABBOnOrForwardplane(f.farFace, a));
}