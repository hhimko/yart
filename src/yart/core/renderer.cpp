////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer.h"


#include <algorithm>
#include <execution>
#include <iostream>
#include <limits>

#include <imgui.h>

#include "yart/core/utils/yart_utils.h"
#include "yart/application.h"


/// @brief Height of the overlay grid plane
#define GRID_PLANE_HEIGHT 0.0f 

/// @brief Color of the overlay grid plane
#define GRID_PLANE_COLOR 0.01f, 0.01f, 0.01f


namespace yart
{
    Renderer::Renderer()
    {
        m_cameraLookDirection = yart::utils::SphericalToCartesianUnitVector(m_cameraYaw, m_cameraPitch);
        RecalculateCameraTransformationMatrix();
    }

    bool Renderer::Render(float buffer[], uint32_t width, uint32_t height)
    {
        YART_ASSERT(buffer != nullptr);
        YART_ASSERT(m_scene != nullptr);

        bool dirty = false;
        if (width != m_width || height != m_height) {
            Resize(width, height);
            dirty = true;
        }

        // Multithreaded iteration through all image pixels
        std::for_each(std::execution::par, m_verticalPixelIterator.begin(), m_verticalPixelIterator.end(), [&](uint32_t y) {
            std::for_each(std::execution::par, m_horizontalPixelIterator.begin(), m_horizontalPixelIterator.end(), [&](uint32_t x) {

                const glm::vec3 ray_direction     = glm::normalize(m_inverseViewProjectionMatrix * glm::vec4{ x + 0.5f, y + 0.5f, 1.0f, 1.0f });
                const glm::vec3 ray_direction_ddx = glm::normalize(m_inverseViewProjectionMatrix * glm::vec4{ x + 1.5f, y + 0.5f, 1.0f, 1.0f });
                const glm::vec3 ray_direction_ddy = glm::normalize(m_inverseViewProjectionMatrix * glm::vec4{ x + 0.5f, y + 1.5f, 1.0f, 1.0f });

                // Trace a ray from the camera's origin into the scene
                HitPayload payload;
                TraceRay({ m_cameraPosition, ray_direction, ray_direction_ddx, ray_direction_ddy }, payload);

                size_t idx = (y * width + x) * 4;
                buffer[idx + 0] = payload.resultColor.r;
                buffer[idx + 1] = payload.resultColor.g;
                buffer[idx + 2] = payload.resultColor.b;
                buffer[idx + 3] = 1.0f;
            });
        });

        return dirty;
    }

    bool Renderer::Render(const yart::Viewport* viewport)
    {
        float* image_data = viewport->GetImageData();
        ImVec2 image_size = viewport->GetImageSize();
        return Render(image_data, image_size.x, image_size.y);
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        // Resize and fill pixel iterators
        m_verticalPixelIterator.resize(static_cast<size_t>(height));
        for (uint32_t i = 0; i < height; ++i)
            m_verticalPixelIterator[i] = i;        
        
        m_horizontalPixelIterator.resize(static_cast<size_t>(width));
        for (uint32_t i = 0; i < width; ++i)
            m_horizontalPixelIterator[i] = i;    
        
        m_width = width;
        m_height = height;

        // Change in aspect ratio requires the camera matrix to be recalculated 
        RecalculateCameraTransformationMatrix();
    }

    void Renderer::TraceRay(const Ray& ray, HitPayload& payload)
    {
        yart::Object* hit_object;
        glm::vec4 overlay_color = { 0.0f, 0.0f, 0.0f, 0.0f };
        float u, v;

        // Intersect the ray with the active scene and gizmos view
        float overlay_distance = m_showOverlays ? SampleOverlaysView(ray, overlay_color) : std::numeric_limits<float>::max();
        float hit_distance = m_scene->IntersectRay(ray, hit_object, &u, &v);
        payload.hitDistance = hit_distance;

        if (hit_distance > m_nearClippingPlane && hit_distance < m_farClippingPlane) {
            if (overlay_distance > hit_distance) {
                overlay_color.a = 0.0f; // Fix color ordering
            }

            const glm::vec3 mat_col = { u, v, 0.0f };
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
            const glm::vec2 w = glm::max(glm::abs(uv_ddx), glm::abs(uv_ddy)) + glm::max(grid_plane_distance / 4000.0f, 0.0001f);

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

    void Renderer::RecalculateCameraTransformationMatrix()
    {
        // Calculate the view matrix inverse (camera space to world space)
        const glm::mat4 view_matrix = yart::utils::CreateViewMatrix(m_cameraLookDirection, UP_DIRECTION);
        const glm::mat4 view_matrix_inverse = glm::inverse(view_matrix);

        // Calculate the projection matrix inverse (screen space to camera space)
        float w = static_cast<float>(m_width);
        float h = static_cast<float>(m_height);
        float fov = m_fieldOfView * yart::utils::DEG_TO_RAD;
        const glm::mat4 projection_matrix_inverse = yart::utils::CreateInverseProjectionMatrix(fov, w, h, m_nearClippingPlane);

        m_inverseViewProjectionMatrix = view_matrix_inverse * projection_matrix_inverse;
    }
    
} // namespace yart
