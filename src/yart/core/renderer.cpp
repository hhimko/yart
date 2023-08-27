////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer.h"


#include <algorithm>
#include <execution>
#include <iostream>

#include <imgui.h>

#include "yart/utils/yart_utils.h"
#include "yart/utils/glm_utils.h"
#include "yart/application.h"


namespace yart
{
    bool Renderer::Render(float buffer[], uint32_t width, uint32_t height)
    {
        YART_ASSERT(buffer != nullptr);
        if (width != m_width || height != m_height)
            Resize(width, height);
            

        // -- RAY GENERATION LOOP -- //

        // Multithreaded iteration through all pixels
        std::for_each(std::execution::par, m_verticalPixelIterator.begin(), m_verticalPixelIterator.end(), [&](uint32_t y) {
            std::for_each(std::execution::par, m_horizontalPixelIterator.begin(), m_horizontalPixelIterator.end(), [&](uint32_t x) {

                glm::vec3 ray_direction = m_inverseViewProjectionMatrix * glm::vec4{ x + 0.5f, y + 0.5f, 1.0f, 1.0f };
                ray_direction = glm::normalize(ray_direction);

                // Trace a ray from the camera's origin into the scene
                HitPayload payload;
                TraceRay({m_cameraPosition, ray_direction}, payload);

                size_t idx = (y * width + x) * 4;
                buffer[idx + 0] = payload.resultColor.r;
                buffer[idx + 1] = payload.resultColor.g;
                buffer[idx + 2] = payload.resultColor.b;
                buffer[idx + 3] = 1.0f;
            });
        });


        bool was_dirty = m_dirty;
        m_dirty = false;
        return was_dirty;
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

        m_dirty = true;

        // Change in aspect ratio requires the camera matrix to be recalculated 
        RecalculateCameraTransformationMatrix();
    }

    void Renderer::TraceRay(const Ray& ray, HitPayload& payload)
    {
        float t, u, v;
        if (yart::Ray::IntersectTriangle(ray, {-1, 0, 0}, {0, 1, 0}, {1, 0, 0}, t, u, v) && t > m_nearClippingPlane && t < m_farClippingPlane) {
            payload.resultColor = glm::vec3{u,v,1-u-v};
            return; // ClosestHit(ray, payload);
        }

        return Miss(ray, payload);
    }

    void Renderer::ClosestHit(const Ray &ray, HitPayload &payload)
    {
        payload.resultColor = glm::vec3{0,0,0};
    }

    void Renderer::Miss(const Ray &ray, HitPayload &payload)
    {
        static const glm::vec3 grad[3] = { {0.1, 0.1, 0.2}, {0.05, 0.1, 0.6}, {0.1,0.15,0.9} };
        static const uint8_t grad_size = 2;

        float g = (ray.direction.y + 1.0f) / 2.0f; 

        const glm::vec3& col_1 = grad[static_cast<size_t>(glm::floor(g * (grad_size - 1)))];
        const glm::vec3& col_2 = grad[static_cast<size_t>(glm::ceil(g * (grad_size - 1)))];
        float i = glm::fract(g * (grad_size - 1));
        payload.resultColor = col_1 * (1.0f - i) + col_2 * i;
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
