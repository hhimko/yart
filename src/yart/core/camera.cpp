#include "camera.h"


#include <algorithm>

#include "yart/common/threads/parallel_for.h"


namespace yart
{
    Camera::Camera()
    {
        // Calculate the initial camera look direction vector, based on default pitch and yaw values
        m_lookDirection = yart::utils::SphericalToCartesianUnitVector(m_rotationYaw, m_rotationPitch);
        m_shouldRecalculateCache = true;
    }

    const glm::vec3* Camera::GetRayDirections(uint32_t width, uint32_t height, bool* resized)
    {
        bool should_resize = (width != m_rayDirectionsCacheWidth || height != m_rayDirectionsCacheHeight);
        if (should_resize || m_shouldRecalculateCache)
            RecalculateRayDirectionsCache(width, height);

        if (resized != nullptr) 
            *resized = should_resize;

        return m_rayDirectionsCache.data();
    }

    void Camera::RotateByMouseDelta(float x, float y)
    {
        static constexpr float scale = 0.01f; 

        m_rotationYaw -= x * scale;
        m_rotationPitch -= y * scale;
        m_rotationPitch = glm::clamp(m_rotationPitch, Camera::PITCH_MIN, Camera::PITCH_MAX);

        m_lookDirection = yart::utils::SphericalToCartesianUnitVector(m_rotationYaw, m_rotationPitch);
        m_shouldRecalculateCache = true;
    }

    void Camera::RecalculateRayDirectionsCache(uint32_t width, uint32_t height)
    {
        m_rayDirectionsCacheWidth = width;
        m_rayDirectionsCacheHeight = height;

        // Calculate the view matrix inverse (camera space to world space)
        const glm::mat4 view_matrix = yart::utils::CreateViewMatrix(m_lookDirection, UP_DIRECTION);
        const glm::mat4 view_matrix_inverse = glm::inverse(view_matrix);

        // Calculate the projection matrix inverse (screen space to camera space)
        float w = static_cast<float>(width);
        float h = static_cast<float>(height);
        float fov = m_fieldOfView * yart::utils::DEG_TO_RAD;
        const glm::mat4 projection_matrix_inverse = yart::utils::CreateInverseProjectionMatrix(fov, w, h, m_nearClippingPlane);
        const glm::mat4 inverse_view_projection_matrix = view_matrix_inverse * projection_matrix_inverse;

        // Precalculate ray directions for each pixel
        size_t size = (width + 1) * (height + 1);
        m_rayDirectionsCache.resize(size);

        yart::threads::parallel_for<size_t>(0, size, [&](size_t i) {
            const auto [y, x] = std::div(i, static_cast<int64_t>(width));
            m_rayDirectionsCache[i] = glm::normalize(inverse_view_projection_matrix * glm::vec4{ x + 0.5f, y + 0.5f, 1.0f, 1.0f });
        });

        m_shouldRecalculateCache = false;
    }
} // namespace yart
