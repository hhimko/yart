////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Ray class and ray intersection tests definitions  
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <glm/glm.hpp>


namespace yart
{
    /// @brief Structure defining a ray in a three dimensional space 
    struct Ray {
    public:
        /// @brief Origin of the ray in world space
        glm::vec3 origin; 

        /// @brief Direction (unit) vector of the ray
        glm::vec3 direction;

    public:
        /// @brief Ray-triangle intersection check, implemented using the Möller-Trumbore algorithm
        /// @param ray Traced ray
        /// @param v0 First vertex of the triangle in world space
        /// @param v1 Second vertex of the triangle in world space
        /// @param v2 Third vertex of the triangle in world space
        /// @param t Output parameter set on valid intersection with distance from the ray's origin to the intersection point value
        /// @param u Output parameter set on valid intersection with barycentric u parameter value
        /// @param v Output parameter set on valid intersection with barycentric v parameter value
        /// @return Whether there was a valid intersection and the output parameters were populated
        static bool IntersectTriangle(const Ray& ray, 
            const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float* t, float* u, float* v
        );

    };
} // namespace yart
