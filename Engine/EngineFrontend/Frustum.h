#pragma once

#include "Plane.h"

class Camera;
struct Sphere;
struct AABB;
class Transform;
struct CameraComponent;

struct Frustum
{
    Plane topFace;
    Plane bottomFace;

    Plane rightFace;
    Plane leftFace;

    Plane farFace;
    Plane nearFace;
};

Frustum CreateFrustumFromCamera(const Transform& cam, CameraComponent& comComp, float aspect, float fovY, float zNear, float zFar);

Frustum CreateFrustumFromCamera(const Camera& cam, float aspect, float fovY, float zNear, float zFar);

bool SphereOnFrustum(const Frustum& f, const Sphere& s, const Transform& t);

bool AABBOnFrustum(const Frustum& f, const AABB& a, const Transform& t);