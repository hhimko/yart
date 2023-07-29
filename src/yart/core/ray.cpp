////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Ray class and ray intersection tests implementations  
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ray.h"


namespace yart
{
    bool Ray::IntersectTriangle(const Ray &ray, 
        const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, 
        float &t, float &u, float &v
    )
    {
        glm::vec3 E01 = v1 - v0;
        glm::vec3 E02 = v2 - v0;

        glm::vec3 P = glm::cross(ray.direction, E02);
        float det = glm::dot(E01, P);

        // Back culling step
        if (det < EPSILON) 
           return false;

        float inv_det = 1.f / det;

        glm::vec3 T = ray.origin - v0;
        u = glm::dot(T, P) * inv_det;
        if (u < 0.f || u > 1.f) 
            return false;

        glm::vec3 Q = glm::cross(T, E01);
        v = glm::dot(ray.origin, Q) * inv_det;
        if (v < 0.f || u + v > 1.f) 
            return false;

        t = glm::dot(E02, Q) * inv_det;

        return true;
    }
} // namespace yart
