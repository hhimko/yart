////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "renderer.h"


#include <algorithm>
#include <execution>
#include <iostream>

#include <imgui.h>

#include "yart/core/utils/yart_utils.h"
#include "yart/application.h"


namespace yart
{
    bool Renderer::Render(float buffer[], uint32_t width, uint32_t height)
    {
        YART_ASSERT(buffer != nullptr);

        bool dirty = false;
        if (width != m_width || height != m_height) {
            Resize(width, height);
            dirty = true;
        }

        // Multithreaded iteration through all image pixels
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

        return dirty;
    }

    bool Renderer::Render(const yart::Viewport *viewport)
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
        float t, u, v;
        if (yart::Ray::IntersectTriangle(ray, {-1, 0, 0}, {0, 1, 0}, {1, 0, 0}, t, u, v) && t > m_nearClippingPlane && t < m_farClippingPlane) {
            payload.resultColor = glm::vec3{u,v,1-u-v};
            return; // ClosestHit(ray, payload);
        }

        return Miss(ray, payload);
    }

    void Renderer::ClosestHit(const Ray& ray, HitPayload& payload)
    {
        YART_UNUSED(ray);
        payload.resultColor = glm::vec3{0,0,0};
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
