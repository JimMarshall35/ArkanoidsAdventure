#include "pch.h"
#include "MiscFunctions.h"
#include <utility>
#include <vector>
#include <stack>
#include <stdlib.h>


bool AreSame(float a, float b, float tolerance /*= 10e-5*/)
{
	float diff = fabs(a - b);
	return diff < tolerance;
}

bool PointIsSame(const glm::vec2& pt1, const glm::vec2& pt2, float tolerance /*= 10e-5*/)
{
	return AreSame(pt1.x, pt2.x, tolerance) && AreSame(pt1.y, pt2.y, tolerance);
}

bool RectTest(const CRect& rect, const glm::vec2& pos)
{
	if (pos.x >= rect.left && pos.x <= rect.right)
	{
		if (pos.y >= rect.top && pos.y <= rect.bottom)
		{
			return true;
		}
	}
	return false;
}


// Given three collinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool OnSegment(const glm::vec2& p, const glm::vec2& q, const glm::vec2& r)
{
    using namespace std;
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;

    return false;
}

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are collinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
int Orientation(const glm::vec2& p, const glm::vec2& q, const glm::vec2& r)
{
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
    // for details of below formula. 
    int val = (q.y - p.y) * (r.x - q.x) -
        (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // collinear 

    return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool DoIntersect(const glm::vec2& p1, const glm::vec2& q1, const glm::vec2& p2, const glm::vec2& q2)
{
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = Orientation(p1, q1, p2);
    int o2 = Orientation(p1, q1, q2);
    int o3 = Orientation(p2, q2, p1);
    int o4 = Orientation(p2, q2, q1);

    // General case 
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases 
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1 
    if (o1 == 0 && OnSegment(p1, p2, q1)) return true;

    // p1, q1 and q2 are collinear and q2 lies on segment p1q1 
    if (o2 == 0 && OnSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are collinear and p1 lies on segment p2q2 
    if (o3 == 0 && OnSegment(p2, p1, q2)) return true;

    // p2, q2 and q1 are collinear and q1 lies on segment p2q2 
    if (o4 == 0 && OnSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases 
}

glm::vec3 CalcNormal(const glm::vec3& t1, const glm::vec3& t2, const glm::vec3& t3)
{
    glm::vec3 A = t2 - t1;
    glm::vec3 B = t3 - t1;
    return glm::normalize(glm::cross(A, B));
}

