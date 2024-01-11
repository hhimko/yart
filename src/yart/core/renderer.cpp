////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer.h"


#include <algorithm>

#include <imgui.h>

#include "yart/common/threads/parallel_for.h"
#include "yart/application.h"


/// @brief Height of the overlay grid plane
#define GRID_PLANE_HEIGHT 0.0f 

/// @brief Color of the overlay grid plane
#define GRID_PLANE_COLOR 0.01f, 0.01f, 0.01f


namespace yart
{
    bool Renderer::Render(yart::Camera& camera, float buffer[], uint32_t width, uint32_t height)
    {
        YART_ASSERT(buffer != nullptr);
        YART_ASSERT(m_scene != nullptr);

        bool dirty;
        const glm::vec3* ray_directions = camera.GetRayDirections(width, height, &dirty);

        // Multithreaded iteration through all image pixels
        yart::threads::parallel_for<size_t>(0, width * height, [&](size_t i) {
            const auto [y, x] = std::div(i, static_cast<int64_t>(width));

            const glm::vec3 ray_direction     = ray_directions[i];
            const glm::vec3 ray_direction_ddx = ray_directions[(y + 0) * width + x + 1];
            const glm::vec3 ray_direction_ddy = ray_directions[(y + 1) * width + x + 0];

            // Trace a ray from the camera's origin into the scene
            HitPayload payload;
            yart::Ray ray = { camera.position, ray_direction, ray_direction_ddx, ray_direction_ddy };
            TraceRay(camera, ray, payload);

            buffer[i * 4 + 0] = payload.resultColor.r;
            buffer[i * 4 + 1] = payload.resultColor.g;
            buffer[i * 4 + 2] = payload.resultColor.b;
            buffer[i * 4 + 3] = 1.0f;
        });

        return dirty;
    }

    bool Renderer::Render(yart::Camera& camera, const yart::Viewport& viewport)
    {
        float* image_data = viewport.GetImageData();
        const ImVec2 image_size = viewport.GetImageSize();

        return Render(camera, image_data, image_size.x, image_size.y);
    }

    void Renderer::TraceRay(yart::Camera& camera, const Ray& ray, HitPayload& payload)
    {
        yart::Object* hit_object;
        glm::vec4 overlay_color = { 0.0f, 0.0f, 0.0f, 0.0f };
        glm::vec3 out_vec;

        // Intersect the ray with the active scene and gizmos view
        float overlay_distance = m_showOverlays ? SampleOverlaysView(ray, overlay_color) : std::numeric_limits<float>::max();
        float hit_distance = m_scene->IntersectRay(ray, &hit_object, m_materialUvs, out_vec);
        payload.hitDistance = hit_distance;

        if (hit_distance > camera.GetNearClippingPlane() && hit_distance < camera.GetFarClippingPlane()) {
            if (overlay_distance > hit_distance) {
                overlay_color.a = 0.0f; // Fix color ordering
            }

            const glm::vec3 mat_col = out_vec;
            payload.resultColor = mat_col * (1.0f - overlay_color.a) + glm::vec3(overlay_color) * overlay_color.a;
            return; //return ClosestHit(ray, payload);
        }

        Miss(ray, payload);
        payload.resultColor = payload.resultColor * (1.0f - overlay_color.a) + glm::vec3(overlay_color) * overlay_color.a;
    }

    float Renderer::SampleOverlaysView(const yart::Ray& ray, glm::vec4& color)
    {
        // Grid plane
        const float grid_plane_distance = (GRID_PLANE_HEIGHT - ray.origin.y) / ray.direction.y;
        if (grid_plane_distance > 0.0f) {
            const glm::vec3 hit_pos = ray.origin + ray.direction * grid_plane_distance;
            const glm::vec2 uv = { hit_pos.x, hit_pos.z };

            // Analytically filtered grid pattern based on Inigo Quilez's approach
            // See more: https://iquilezles.org/articles/filterableprocedurals/
            const float grid_plane_distance_ddx = (GRID_PLANE_HEIGHT - ray.origin.y) / ray.direction_ddx.y;
            const float grid_plane_distance_ddy = (GRID_PLANE_HEIGHT - ray.origin.y) / ray.direction_ddy.y;
            const glm::vec3 hit_pos_ddx = ray.origin + ray.direction * grid_plane_distance_ddx;
            const glm::vec3 hit_pos_ddy = ray.origin + ray.direction * grid_plane_distance_ddy;
            const glm::vec2 uv_ddx = glm::vec2{ hit_pos_ddx.x, hit_pos_ddx.z } - uv;
            const glm::vec2 uv_ddy = glm::vec2{ hit_pos_ddy.x, hit_pos_ddy.z } - uv;

            // Distance-responsive filter kernel
            const glm::vec2 w = glm::max(glm::abs(uv_ddx), glm::abs(uv_ddy)) + glm::max(grid_plane_distance / 400.0f, 0.0001f);

            // Analytic (box) filtering
            const float N = m_useThickerGrid ? 50.0f : 100.0f;
            const glm::vec2 a = uv + 0.5f * w;                        
            const glm::vec2 b = uv - 0.5f * w;           
            const glm::vec2 i = (glm::floor(a) + glm::min(glm::fract(a) * N, 1.0f) - glm::floor(b) - glm::min(glm::fract(b) * N, 1.0f)) / (N * w);

            color = glm::vec4(GRID_PLANE_COLOR, 1.0f - (1.0f - i.x) * (1.0f - i.y));
        }

        return grid_plane_distance;
    }

    void Renderer::ClosestHit(const Ray& ray, HitPayload& payload)
    {
        YART_UNUSED(ray);
        payload.resultColor = { 0.0f, 0.0f, 0.0f };
    }

    void Renderer::Miss(const Ray& ray, HitPayload& payload)
    {
        payload.resultColor = m_world->SampleSkyColor(ray.direction);
    }
} // namespace yart
