#include "Sphere.h"
#include <cmath>
#include <glm/gtx/norm.hpp>
#include "IArchive.h"

Sphere RitterBoundingSphere(const EVec<glm::vec3>& vertices)
{
    glm::vec3 vmin, vmax;

    vmin = vmax = vertices[0];                          //assign first vertex as minimum and maximum
    int num_vertices = vertices.size();

    for (int i = 1; i < num_vertices; i++)                    //find min and max x, y, and z values
    {
        if (vertices[i].x < vmin.x)
            vmin.x = vertices[i].x;
        if (vertices[i].y < vmin.y)
            vmin.y = vertices[i].y;
        if (vertices[i].z < vmin.z)
            vmin.z = vertices[i].z;

        if (vertices[i].x > vmax.x)
            vmax.x = vertices[i].x;
        if (vertices[i].y > vmax.y)
            vmax.y = vertices[i].y;
        if (vertices[i].z > vmax.z)
            vmax.z = vertices[i].z;
    }
    float xdiff = vmax.x - vmin.x;
    float ydiff = vmax.y - vmin.y;
    float zdiff = vmax.z - vmin.y;

    float diameter = std::max(xdiff, std::max(ydiff, zdiff));     //take max as diameter
    glm::vec3 centre = (vmax + vmin) * (0.5f);
    float radius = diameter / 2;

    float sq_radius = radius * radius;

    for (int i = 0; i < num_vertices; i++)
    {
        glm::vec3 point = vertices[i];

        glm::vec3 direction = point - centre;
        float sq_distance = glm::length2(direction);

        if (sq_distance > sq_radius)
        {
            float distance = sqrt(sq_distance);

            float difference = distance - radius;

            float new_diameter = 2 * radius + difference;
            sq_radius = radius * radius;

            difference /= 2;

            centre += difference * direction;
            radius = 0.5f * new_diameter;
        }
    }
    return Sphere{ centre, radius };
}

void Sphere::Serialize(IArchive& a)
{
    if (a.IsStoring())
    {
        a.PushObj("Sphere");
            a.PushObj("Center");
                a << center;
            a.PopObj();
            a.PushObj("Radius");
                a << radius;
            a.PopObj();
        a.PopObj();
    }
    else
    {
        a.PushObj("Sphere");
            a.PushObj("Sphere");
            a.PushObj("Center");
                a >> center;
            a.PopObj();
            a.PushObj("Radius");
                a >> radius;
            a.PopObj();
        a.PopObj();
        a.PopObj();
    }
    
}
