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
#include "yart/GUI/gui.h"


/// @brief Smallest valid value for camera's FOV in degrees
/// @details Used in Renderer::OnImGui() to adjust the FOV slider range
#define FOV_MIN 45.0f

/// @brief Largest valid value for camera's FOV in degrees
/// @details Used in Renderer::OnImGui() to adjust the FOV slider range
#define FOV_MAX 180.0f

/// @brief Smallest valid value for camera's near clipping plane distance
/// @details Used in Renderer::OnImGui() to adjust the near clip slider range
#define NEAR_CLIP_MIN 0.001f

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
    bool Renderer::Render(float buffer[], uint32_t width, uint32_t height)
    {
        YART_ASSERT(buffer != nullptr);
        if (width != m_width || height != m_height)
            Resize(width, height);
            

        // -- RAY GENERATION LOOP -- //

        // Multithreaded iteration through all pixels
        std::for_each(std::execution::par, m_verticalPixelIterator.begin(), m_verticalPixelIterator.end(), [&](uint32_t y) {
            std::for_each(std::execution::par, m_horizontalPixelIterator.begin(), m_horizontalPixelIterator.end(), [&](uint32_t x) {

                glm::vec4 ray_direction = m_inverseViewProjectionMatrix * glm::vec4{ x + 0.5f, y + 0.5f, 1.0f, 1.0f };
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
    
    void Renderer::OnImGui()
    {
        // Whether the camera transformation matrix should be recalculated
        bool recalculate = false;


        if (ImGui::SliderFloat("FOV", &m_fieldOfView, FOV_MIN, FOV_MAX)) {
            recalculate = true;
            m_dirty = true;
        }

        if (ImGui::SliderFloat("Near clipping plane", &m_nearClippingPlane, NEAR_CLIP_MIN, NEAR_CLIP_MAX)) {
            recalculate = true;
            m_dirty = true;
        }

        if (ImGui::SliderFloat("Far clipping plane", &m_farClippingPlane, FAR_CLIP_MIN, FAR_CLIP_MAX)) {
            m_dirty = true;
        }


        // Render the view axes context window
        const glm::vec3 x_axis = { glm::sin(m_cameraYaw), glm::sin(m_cameraPitch) * glm::cos(m_cameraYaw), -glm::cos(m_cameraYaw) };
        const glm::vec3 y_axis = { 0, -glm::cos(m_cameraPitch), -glm::sin(m_cameraPitch) };
        const glm::vec3 z_axis = glm::normalize(glm::cross(x_axis, y_axis));
        yart::GUI::RenderViewAxesWindow(x_axis, y_axis, z_axis);

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
            m_dirty = true;
        }

        // Side-to-side movement
        float horizontal_speed = yart::Input::GetHorizontalAxis();
        if (horizontal_speed != 0) {
            const glm::vec3 u = -glm::normalize(glm::cross(m_cameraLookDirection, UP_DIRECTION)); // Camera view horizontal (right) direction vector
            m_cameraPosition += u * horizontal_speed * m_cameraMoveSpeed;
            m_dirty = true;
        }

        // Ascend/descend movement
        float elevation_speed = static_cast<float>(ImGui::IsKeyDown(ImGuiKey_Space));
        elevation_speed -= static_cast<float>(ImGui::IsKeyDown(ImGuiKey_LeftCtrl));
        if (elevation_speed != 0) {
            m_cameraPosition += UP_DIRECTION * elevation_speed * m_cameraMoveSpeed;
            m_dirty = true;
        }


        // -- ROTATION -- //
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
            yart::Input::SetCursorLocked(true); // Lock and hide the cursor
            const glm::ivec2& mouse = yart::Input::GetMouseMoveDelta();

            if (mouse.x != 0 || mouse.y != 0) {
                RotateCameraWithMouseMoveDelta(mouse);
                RecalculateCameraTransformationMatrix();
                m_dirty = true;
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
        payload.resultColor = ray.direction;
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
