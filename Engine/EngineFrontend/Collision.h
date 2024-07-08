#pragma once
#include <glm.hpp>
using namespace glm;


struct Collision
{
	vec3 intersectionPt;
	float nearestDistance;
};

struct Ellipsoid
{
	vec3 pos;
	vec3 dim;
};

bool CheckEllipsoidTriangleMeshCollisions(Ellipsoid& e, const vec3& velocity, vec3* polys, int numVerts, Collision& outCollision);
