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
#include "yart/platform/input.h"
#include "yart/application.h"


/// @brief Smallest valid value for camera's FOV in degrees
/// @details Used in Renderer::OnImGui() to adjust the FOV slider range
#define FOV_MIN 45.0f

/// @brief Largest valid value for camera's FOV in degrees
/// @details Used in Renderer::OnImGui() to adjust the FOV slider range
#define FOV_MAX 180.0f

/// @brief Smallest valid value for camera's near clipping plane distance
/// @details Used in Renderer::OnImGui() to adjust the near clip slider range
#define NEAR_CLIP_MIN 0.0f

/// @brief Largest valid value for camera's near clipping plane distance
/// @details Used in Renderer::OnImGui() to adjust the near clip slider range
#define NEAR_CLIP_MAX 10.0f

/// @brief Smallest valid value for camera's far clipping plane distance
/// @details Used in Renderer::OnImGui() to adjust the far clip slider range
#define FAR_CLIP_MIN 100.0f

/// @brief Largest valid value for camera's far clipping plane distance
/// @details Used in Renderer::OnImGui() to adjust the far clip slider range
#define FAR_CLIP_MAX 1000.0f


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
                size_t idx = (y * width + x) * 4;

                glm::vec4 ray_direction = m_inverseViewProjectionMatrix * glm::vec4{ x + 0.5f, y + 0.5f, 1.0f, 1.0f };
                ray_direction = glm::normalize(ray_direction);

                buffer[idx + 0] = ray_direction.r;
                buffer[idx + 1] = ray_direction.g;
                buffer[idx + 2] = ray_direction.b;
                buffer[idx + 3] = 1.0f;
            });
        });
    }
    
    void Renderer::OnImGui()
    {
        // Whether the camera transformation matrix should be recalculated
        bool recalculate = false;


        if (ImGui::SliderFloat("FOV", &m_fieldOfView, FOV_MIN, FOV_MAX)) {
            recalculate = true;
        }

        if (ImGui::SliderFloat("Near clipping plane", &m_nearClippingPlane, NEAR_CLIP_MIN, NEAR_CLIP_MAX)) {
            recalculate = true;
        }

        ImGui::SliderFloat("Far clipping plane", &m_farClippingPlane, FAR_CLIP_MIN, FAR_CLIP_MAX);


        if (recalculate)
            RecalculateCameraTransformationMatrix();
    }

    bool Renderer::UpdateCamera()
    {
        // -- TRANSLATION -- //
        // Forward/backward movement
        float vertical_speed = yart::Input::GetVerticalAxis();
        if (vertical_speed != 0) {
            m_cameraPosition += m_cameraLookDirection * vertical_speed * m_cameraMoveSpeed;
        }

        // Side-to-side movement
        float horizontal_speed = yart::Input::GetHorizontalAxis();
        if (horizontal_speed != 0) {
            const glm::vec3 u = -glm::normalize(glm::cross(m_cameraLookDirection, UP_DIRECTION)); // Camera view horizontal (right) direction vector
            m_cameraPosition += u * horizontal_speed * m_cameraMoveSpeed;
        }

        // Ascend/descend movement
        float elevation_speed = static_cast<float>(ImGui::IsKeyDown(ImGuiKey_Space));
        elevation_speed -= static_cast<float>(ImGui::IsKeyDown(ImGuiKey_LeftCtrl));
        if (elevation_speed != 0) {
            m_cameraPosition += UP_DIRECTION * elevation_speed * m_cameraMoveSpeed;
        }


        // -- ROTATION -- //
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            yart::Input::SetCursorLocked(true); // Lock and hide the cursor
            const glm::ivec2& mouse = yart::Input::GetMouseMoveDelta();

            if (mouse.x != 0 || mouse.y != 0) {
                RotateCameraWithMouseMoveDelta(mouse);
                RecalculateCameraTransformationMatrix();
            }
        } else {
            yart::Input::SetCursorLocked(false); // Unlock the cursor 
        }


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
        float fov = m_fieldOfView * DEG_TO_RAD;
        const glm::mat4 projection_matrix_inverse = yart::utils::CreateInverseProjectionMatrix(fov, w, h, m_nearClippingPlane);

        m_inverseViewProjectionMatrix = view_matrix_inverse * projection_matrix_inverse;
    }

    void Renderer::RotateCameraWithMouseMoveDelta(const glm::ivec2& mouse_drag)
    {
        m_cameraYaw += static_cast<float>(mouse_drag.x) * 0.01f;
        m_cameraPitch += static_cast<float>(mouse_drag.y) * 0.01f;
        m_cameraPitch = glm::clamp(m_cameraPitch, CAMERA_PITCH_MIN, CAMERA_PITCH_MAX);

        m_cameraLookDirection = yart::utils::SphericalToCartesianUnitVector(m_cameraYaw, m_cameraPitch);
    }
} // namespace yart
