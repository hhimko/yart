#include "renderer.h"

#include <algorithm>
#include <execution>
#include <iostream>

#include "utils/core_utils.h"
#include "utils/glm_utils.h"


namespace yart
{
    void Renderer::Render(float buffer[], uint32_t width, uint32_t height)
    {
        YART_ASSERT(buffer != nullptr);
        if (width != m_width || height != m_height)
            Resize(width, height);


        // Multithreaded iteration through all pixels
        std::for_each(std::execution::par, m_verticalPixelIterator.begin(), m_verticalPixelIterator.end(), [&](uint32_t y) {
            std::for_each(std::execution::par, m_horizontalPixelIterator.begin(), m_horizontalPixelIterator.end(), [&](uint32_t x) {
                size_t idx = ((y * width) + x) * 4;

                buffer[idx + 0] = static_cast<float>(x) / (width - 1);
                buffer[idx + 1] = static_cast<float>(y) / (height - 1);
                buffer[idx + 2] = 0.f;
                buffer[idx + 3] = 1.f;
            });
        });
    }

    bool Renderer::UpdateCamera()
    {
        const glm::vec3 u = -glm::normalize(glm::cross(m_cameraLookDirection, UP_DIRECTION)); // Camera view horizontal (right) direction vector
        const glm::vec3 v = glm::cross(-u, m_cameraLookDirection); // Camera view vertical (up) direction vector

        /// TODO: Renderer::UpdateCamera is unimplemented
        return false;
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

    void Renderer::RecalculateCameraTransformationMatrix()
    {
        // Calculate the view matrix inverse (camera space to world space)
        const glm::mat4 view_matrix = yart::utils::CreateViewMatrix(m_cameraLookDirection, UP_DIRECTION);
        const glm::mat4 view_matrix_inverse = glm::inverse(view_matrix);

        // Calculate the projection matrix inverse (screen space to camera space)
        float w = static_cast<float>(m_width);
        float h = static_cast<float>(m_height);
        const glm::mat4 projection_matrix_inverse = yart::utils::CreateInverseProjectionMatrix(m_fieldOfView, w, h, m_nearClippingPlane);

        m_inverseViewProjectionMatrix = view_matrix_inverse * projection_matrix_inverse;
    }

} // namespace yart
