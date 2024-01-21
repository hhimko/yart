/// @brief Check for any intersections with the scene for a given ray 
/// @param ray Traced ray
/// @param scene Rendered scene
/// @param payload Structure used for storing the hit information
/// @return Whether any collisions occurred and the payload has been populated
bool IntersectScene(const Ray& ray, const Scene& scene, HitPayload& payload)
{
    // Determine if the ray hits any geometry along its path
    float min_distance = std::numeric_limits<float>::infinity();
    for (auto&& obj : scene.objects) {
        for (auto&& tri : scene.triangles) {
            float t, u, v;
            bool hit = RayTriangleIntersection(ray, tri.v0, tri.v1, tri.v2, 
                &t, &u, &v);

            // Store the closest hit information into a payload object
            if (hit && t > 0.0f && t < min_distance) {
                min_distance = t;
                payload.hitObject = obj;
                payload.hitDistance = t;
                payload.hitPosition = ray.origin + t * ray.direction;
                const glm::vec3 n = glm::cross(tri.v1 - tri.v0, tri.v2 - tri.v1);
                payload.hitNormal = glm::normalize(n);
            }
        }
    }

    return (min_distance != std::numeric_limits<float>::infinity());
}
