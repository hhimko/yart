/// @brief Ray-triangle intersection testing using Moller-Trumbore algorithm
/// @param ray Traced ray
/// @param v0 First vertex of the triangle in world space
/// @param v1 Second vertex of the triangle in world space
/// @param v2 Third vertex of the triangle in world space
/// @param t Output parameter set on valid intersection with hit distance value
/// @param u Output parameter set on valid intersection with barycentric u value
/// @param v Output parameter set on valid intersection with barycentric v value
/// @return Whether the ray intersected with the triangle
bool RayTriangleIntersection(const Ray& ray,
    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, 
    float* t, float* u, float* v)
{
    glm::vec3 E01 = v1 - v0;
    glm::vec3 E02 = v2 - v0;

    glm::vec3 P = glm::cross(ray.direction, E02);
    float det = glm::dot(E01, P);

    // Back-face culling
    if (det < 0.0001f) 
        return false;

    float inv_det = 1.f / det;

    glm::vec3 T = ray.origin - v0;
    *u = glm::dot(T, P) * inv_det;
    if (*u < 0.f || *u > 1.f) 
        return false;

    glm::vec3 Q = glm::cross(T, E01);
    *v = glm::dot(ray.direction, Q) * inv_det;
    if (*v < 0.f || *u + *v > 1.f) 
        return false;

    *t = glm::dot(E02, Q) * inv_det;
    return true;
}
