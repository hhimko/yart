/// @brief Render an image of given dimensions using ray tracing 
/// @param camera Camera from which to render the scene
/// @param scene Rendered scene
/// @param image Buffer of image pixels in RGB format of size (width * height) 
/// @param width Width of the rendered image
/// @param height Height of the rendered image
void RayTrace(const Camera& camera, const Scene& scene, 
    glm::vec3* pixels, uint32_t width, uint32_t height)
{
    const glm::vec3* ray_directions = camera.GetRayDirections(width, height);

    // Iterate through all pixels/rays in the image
    for (size_t r = 0; r < width * height; ++r) {
        const Ray ray { .origin = camera.position, .direction = ray_directions[r] };
        HitPayload payload;

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

        // Compute the output image color
        const bool miss = (min_distance == std::numeric_limits<float>::infinity());
        pixels[r] = miss ? MissShader(ray) : ClosestHitShader(ray, payload, scene);
    }
}
