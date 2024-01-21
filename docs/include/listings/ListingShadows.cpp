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

        const bool hit = IntersectScene(ray, scene, payload);
        if (!hit) {
            // Compute the output image color
            pixels[r] = MissShader(ray);
            continue;
        }

        float shadow = 1.0f; // 1.0f - fully lit, 0.0f - fully in shadow
        for (auto&& light : scene.lights) {
            const float light_dist = glm::distance(payload.hitPosition, light.position);
            const glm::vec3 dir = glm::normalize(light.position - payload.hitPosition);
            const Ray shadow_ray { .origin = payload.hitPosition, .direction = dir };
            HitPayload shadow_payload;

            const bool shadow_hit = IntersectScene(shadow_ray, scene, shadow_payload);
            if (shadow_hit && shadow_payload.hitDistance < light_dist)
                shadow = glm::max(0.0f, shadow - light.intensity);
        }
        
        // Compute the output image color
        pixels[r] = shadow * ClosestHitShader(ray, payload, scene);
    }
}
