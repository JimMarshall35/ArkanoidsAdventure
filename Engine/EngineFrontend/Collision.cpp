#include "Collision.h"

#include <gtx/norm.hpp>


using namespace std;


//https://www.peroxide.dk/papers/collision/collision.pdf
typedef unsigned int uint32;
#define in(a) ((uint32&) a)

bool CheckPointInTriangle(const vec3& point,
	const vec3& pa, const vec3& pb, const vec3& pc)
{
	vec3 e10 = pb - pa;
	vec3 e20 = pc - pa;
	float a = dot(e10, e10);
	float b = dot(e10, e20);
	float c = dot(e20, e20);
	float ac_bb = (a * c) - (b * b);
	vec3 vp(point.x - pa.x, point.y - pa.y, point.z - pa.z);
	float d = dot(vp, e10);
	float e = dot(vp, e20);
	float x = (d * c) - (e * b);
	float y = (e * a) - (d * b);
	float z = x + y - ac_bb;
	return ((in(z) & ~(in(x) | in(y))) & 0x80000000);
}


class PLANE {
public:
	float equation[4];
	vec3 origin;
	vec3 normal;
	PLANE(const vec3& origin, const vec3& normal);
	PLANE(const vec3& p1, const vec3& p2, const vec3& p3);
	bool isFrontFacingTo(const vec3& direction) const;
	double signedDistanceTo(const vec3& point) const;
};
PLANE::PLANE(const vec3& origin, const vec3& normal) {
	this->normal = normal;
	this->origin = origin;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x * origin.x + normal.y * origin.y
		+ normal.z * origin.z);
}
// Construct from triangle:
PLANE::PLANE(const vec3& p1, const vec3& p2, const vec3& p3)
{
	normal = cross((p2 - p1), (p3 - p1));
	normal = normalize(normal);
	origin = p1;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x * origin.x + normal.y * origin.y
		+ normal.z * origin.z);
}
bool PLANE::isFrontFacingTo(const vec3& direction) const {
	double dotprod = dot(normal, direction);//normal.dot(direction);
	return (dotprod <= 0);
}
double PLANE::signedDistanceTo(const vec3& point) const {
	return dot(point, normal) + equation[3];
}


mat3 R3ToEllipsoidSpaceMatrix(const Ellipsoid& elipsoid)
{
	return mat3(
		1.0f / elipsoid.dim.x, 0.0f, 0.0f,
		0.0f, 1.0f / elipsoid.dim.y, 0.0f,
		0.0f, 0.0f, 1.0f / elipsoid.dim.z
	);
}


vec3 ToEllipsoidSpace(const Ellipsoid& e, const vec3 in)
{
	return in * vec3(1.0 / e.dim.x, 1.0 / e.dim.y, 1.0 / e.dim.z);
}

inline bool IsOutside0To1(float val)
{
	return (val < 0.0f) || (val > 1.0f);
}

/*
	Below is a snippet of code that solves a quadratic equation and returns the
	lowest root, below a certain treshold (the maxR parameter):
*/
bool getLowestRoot(float a, float b, float c, float maxR, float* root) {
	// Check if a solution exists
	float determinant = b * b - 4.0f * a * c;
	// If determinant is negative it means no solutions.
	if (determinant < 0.0f) return false;
	// calculate the two roots: (if determinant == 0 then
	// x1==x2 but let’s disregard that slight optimization)
	float sqrtD = sqrt(determinant);
	float r1 = (-b - sqrtD) / (2 * a);
	float r2 = (-b + sqrtD) / (2 * a);
	// Sort so x1 <= x2
	if (r1 > r2) {
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}
	// Get lowest root:
	if (r1 > 0 && r1 < maxR) {
		*root = r1;
		return true;
	}
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < maxR) {
		*root = r2;
		return true;
	}
	// No (valid) solutions
	return false;
}


bool CheckEllipsoidTriangleMeshCollisions(Ellipsoid& e, const vec3& velocity, vec3* polys, int numVerts, Collision& outCollision)
{
	vec3 eCenterESpace = ToEllipsoidSpace(e, e.pos);
	vec3 wAfterMove = e.pos * velocity;
	vec3 eAfterMove = ToEllipsoidSpace(e, eAfterMove);
	assert(numVerts % 3 == 0);
	bool bFoundCollision = false;

	for (int i = 0; i < numVerts; i += 3)
	{
		PLANE p = PLANE(
			polys[i],
			polys[i + 1],
			polys[i + 2]);

		/*
			t0 = (1 − SignedDistance(basePoint) / (N · velocity)
			t1 = (1 − SignedDistance(basePoint) / (N · velocity)
		*/
		/*
			"Notice that there is a special case if N · velocity = 0 - then we obviously
			can’t use the formulas above. But when does this happen? That happens
			when the velocity vector is perpendicular to the plane normal - in other words
			when the sphere is travelling in parallel to the triangle plane. In this case
			there are two possibilities...
		*/
		float signedDistance = p.signedDistanceTo(eCenterESpace);
		float signedDistanceAfter = p.signedDistanceTo(eAfterMove);
		float dotprod = dot(p.normal, velocity);
		float t0, t1;
		bool bEmbeddedInPlane = false;
		if (dotprod == 0.0f)
		{
			/*
				"either the absolute distance from basePoint to the
				triangle plane is smaller than 1 and the sphere is embedded in the triangle
				plane. If this is the case then we set t0 = 0 and t1 = 1 as the swept sphere
				intersects the plane at all times. "

					O ->
				_____________________________________

			*/
			if (abs(signedDistance) < 1.0f)
			{
				t0 = 0;
				t1 = 1;
				bEmbeddedInPlane = true;
			}
			else
			{
				/*
					"If the distance is greater than 1 then we
					know that a collision cannot ever happen and we can return early from the
					function."

					________O ->______________________
				*/
				continue;
			}
		}
		else
		{
			t0 = ((1.0f - signedDistance) / dotprod);
			t1 = ((1.0f - signedDistance) / dotprod);

			// Swap so t0 < t1
			if (t0 > t1) {
				double temp = t1;
				t1 = t0;
				t0 = temp;
			}

			clamp(t0, 0.0f, 1.0f);
			clamp(t1, 0.0f, 1.0f);

		}

		// OK, at this point we have two time values t0 and t1
		// between which the swept sphere intersects with the
		// triangle plane. If any collision is to occur it must
		// happen within this interval.
		vec3 collisionPoint;
		bool bFoundCollisonWithThisTri = false;
		float t = 1.0;

		// Check that at least one result is within range:
		if (t0 > 1.0f || t1 < 0.0f) {
			// Both t values are outside values [0,1]
			// No collision possible:
			continue;
		}

		// check for collision with interior of triangle
		// ~planeIntersectionPoint = basePoint − planeNormal + t0 ∗ velocity         (3.3)~
		if (!bEmbeddedInPlane)
		{
			vec3 planeIntersectionPoint = eCenterESpace - p.normal + t0 * velocity;
			if (CheckPointInTriangle(planeIntersectionPoint, polys[i], polys[i + 1], polys[i + 2]))
			{
				bFoundCollisonWithThisTri = true;
				collisionPoint = planeIntersectionPoint;
			}
		}


		/*
			"If the sphere does not collide with the inside of the triangle then we’ll have
			to do the “sweep test” against the vertices and edges of the triangle. The
			idea in both cases is to check if there is a t ∈ [t0, t1] where the swept sphere
			collides against either a vertex or edge."
		*/
		if (!bFoundCollisonWithThisTri)
		{
			float velocitySquaredLength = glm::length2(velocity);
			float a, b, c; // Params for equation
			float newT;
			a = velocitySquaredLength;

			// check vertices
			for (int j = 0; j < 3; j++)
			{
				vec3& p = polys[i + j];
				b = 2.0 * (dot(velocity, eCenterESpace - p));
				c = length2(p - eCenterESpace) - 1.0;
				if (getLowestRoot(a, b, c, t, &newT))
				{
					t = newT;
					bFoundCollisonWithThisTri = true;
					collisionPoint = p;
				}
			}

			for (int j = 0; j < 3; j++)
			{
				vec3 p1 = polys[i + j];
				vec3 p2 = j == 2 ? polys[i] : polys[i + j + 1];
				vec3 edge = p2 - p1;
				vec3 baseToVertex = p1 - eCenterESpace;
				float edgeSquaredLength = length2(edge);//edge.squaredLength();
				float edgeDotVelocity = dot(edge, velocity);//edge.dot(velocity);
				float edgeDotBaseToVertex = dot(edge, baseToVertex);//edge.dot(baseToVertex);
				// Calculate parameters for equation
				a = edgeSquaredLength * -velocitySquaredLength +
					edgeDotVelocity * edgeDotVelocity;
				b = edgeSquaredLength * (2 * dot(velocity, baseToVertex)) -
					2.0 * edgeDotVelocity * edgeDotBaseToVertex;
				c = edgeSquaredLength * (1 - length2(baseToVertex)) +
					edgeDotBaseToVertex * edgeDotBaseToVertex;
				// Does the swept sphere collide against infinite edge?
				if (getLowestRoot(a, b, c, t, &newT)) {
					// Check if intersection is within line segment:
					float f = (edgeDotVelocity * newT - edgeDotBaseToVertex) /
						edgeSquaredLength;
					if (f >= 0.0 && f <= 1.0) {
						// intersection took place within segment.
						t = newT;
						bFoundCollisonWithThisTri = true;
						collisionPoint = p1 + f * edge;
					}
				}
			}
		}

		if (bFoundCollisonWithThisTri)
		{
			float distToCollision = length(velocity) * t;
			if (!bFoundCollision || distToCollision < outCollision.nearestDistance)
			{
				outCollision.nearestDistance = distToCollision;
				outCollision.intersectionPt = collisionPoint;
				bFoundCollision = true;
			}
		}
	}
	return bFoundCollision;
}
