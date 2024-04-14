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
            const yart::Ray ray = { camera.position, ray_direction, ray_direction_ddx, ray_direction_ddy };
            TraceRay(camera, ray, payload, 1);

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

    void Renderer::TraceRay(yart::Camera& camera, const Ray& ray, HitPayload& payload, uint8_t bounces)
    {
        // Intersect the ray with the active scene and gizmos view
        glm::vec4 overlay_color = { 0.0f, 0.0f, 0.0f, 0.0f };
        float overlay_distance = m_showOverlays ? SampleOverlaysView(ray, overlay_color) : std::numeric_limits<float>::max();

        if (TraceRaySingle(camera.GetNearClippingPlane(), camera.GetFarClippingPlane(), ray, payload)) {
            // Handle reflections
            glm::vec3 ray_dir = ray.direction;
            HitPayload reflection_payload = payload;
            for (size_t i = 0; i < bounces; i++) {
                float reflection_strength = reflection_payload.hitObject->materialReflection;
                if (reflection_strength <= 0.0f) 
                    break;
                
                ray_dir = glm::reflect(ray_dir, reflection_payload.hitNormal);
                const yart::Ray reflection_ray = { reflection_payload.hitPosition, ray_dir };
                if (!TraceRaySingle(0.0f, camera.GetFarClippingPlane(), reflection_ray, reflection_payload))
                    i = bounces;

                payload.resultColor = payload.resultColor * (1.0f - reflection_strength) + reflection_payload.resultColor * reflection_strength;
            }
        }

        if (payload.hitDistance > 0 && overlay_distance > payload.hitDistance) {
            overlay_color.a = 0.0f; // Fix color ordering
        }

        payload.resultColor = payload.resultColor * (1.0f - overlay_color.a) + glm::vec3(overlay_color) * overlay_color.a;

    }

    bool Renderer::TraceRaySingle(float near, float far, const yart::Ray &ray, HitPayload &payload)
    {
        // Intersect the ray with the active scene
        glm::vec3 out_vec;
        float hit_distance = m_scene->IntersectRay(ray, &payload.hitObject, m_debugShading && m_materialUvs, out_vec);
        payload.hitDistance = hit_distance;

        if (hit_distance < near || hit_distance > far) {
            Miss(ray, payload);
            return false;
        }

        if (m_debugShading) {
            payload.resultColor = out_vec;
            return false;
        } 

        payload.hitPosition = ray.origin + ray.direction * hit_distance;
        payload.hitNormal = out_vec;

        static constexpr size_t num_lights = 3;
        static constexpr glm::vec3 light_positions[num_lights] = { { -2.0f, 4.0f, -3.0f }, { 2.0f, 1.0f, -2.0f }, { -0.5f, 0.5f, -4.0f } };
        static constexpr float light_intensities[num_lights] = { 0.8f, 0.5f, 0.2f };

        float diffuse = 0.0f;
        float specular = 0.0f;
        for (size_t i = 0; i < num_lights; ++i) {
            const float dist = glm::distance(payload.hitPosition, light_positions[i]);
            const glm::vec3 dir = glm::normalize(light_positions[i] - payload.hitPosition);
            
            float shadow = 1.0f;
            if (m_shadows && glm::dot(dir, payload.hitNormal) > 0) {
                const yart::Ray shadow_ray = { payload.hitPosition, dir, dir, dir };

                glm::vec3 _ignored;
                yart::Object* shadow_hit_object;
                const float shadow_hit_distance = m_scene->IntersectRay(shadow_ray, &shadow_hit_object, false, _ignored);

                if (shadow_hit_distance > 0.0f && shadow_hit_distance < dist)
                    shadow = 1.0f + 1.0f / (-4.0f * shadow_hit_distance - 1.0f);
            }

            
            const glm::vec3 h = glm::normalize(dir - ray.direction); // Half vector for specular
            const float intensity = light_intensities[i] * 1.0f / (0.01f * dist * dist + 1.0f); // Inverse-square falloff 

            const float ld = glm::max(0.0f, glm::dot(payload.hitNormal, dir));
            const float ls = glm::pow(glm::max(0.0f, glm::dot(payload.hitNormal, h)), payload.hitObject->materialSpecularFalloff);

            diffuse += shadow * payload.hitObject->materialDiffuse * intensity * ld;
            specular += shadow * payload.hitObject->materialSpecular * intensity * payload.hitObject->materialSpecularFalloff / 256.0f * ls;
        }

        const glm::vec3 ambient = 0.03f * m_world->ambientColor;
        const glm::vec3 mat_col = ambient + payload.hitObject->materialColor * diffuse + specular;

        payload.resultColor = mat_col;
        return true;
    }

    float Renderer::SampleOverlaysView(const yart::Ray &ray, glm::vec4 &color)
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
