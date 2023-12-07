////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Camera class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include "yart/common/utils/glm_utils.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART camera class for defining a viewing context for rendering 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Camera {
    public:
        /// @brief Camera class custom constructor
        Camera();

        /// @brief Get an array of ray directions for a specified screen size
        /// @param width Width of the screen in pixels
        /// @param height Height of the screen in pixels
        /// @param resized Optional output parameter. Given boolean variable is set to whether the
        ///     internal ray directions cache has been resized (width or height changed since last call)
        /// @return A flattened array of size `width * height` of ray directions
        const glm::vec3* GetRayDirections(uint32_t width, uint32_t height, bool* resized = nullptr);

        /// @brief Get the current camera (pitch, yaw) rotation
        /// @param pitch Output parameter, populated with the pitch rotation amount in radians. Safe to pass in `nullptr`
        /// @param yaw Output parameter, populated with the yaw rotation amount in radians. Safe to pass in `nullptr`
        void GetRotation(float* pitch, float* yaw) const;

        /// @brief Set the camera (yaw, pitch) rotation
        /// @param pitch New pitch rotation amount in radians. Should be between Camera::PITCH_MIN and Camera::PITCH_MAX
        /// @param yaw New yaw rotation amount in radians
        void SetRotation(float pitch, float yaw);

        /// @brief Rotate the camera by screen-space mouse movement 
        /// @param x Horizontal mouse position delta
        /// @param y Vertical mouse position delta
        void RotateByMouseDelta(float x, float y);

        /// @brief Get the current looking direction vector
        /// @return Unit vector
        glm::vec3 GetLookDirection() const
        {
            return m_lookDirection;
        }

        /// @brief Get the near clipping plane distance, used for clipping objects out of the camera's frustum
        /// @return Current near clipping plane distance
        float GetNearClippingPlane() const 
        {
            return m_nearClippingPlane;
        }

        /// @brief Set the near clipping plane distance, used for clipping objects out of the camera's frustum
        /// @param value New near clipping plane distance
        void SetNearClippingPlane(float value) 
        {
            m_nearClippingPlane = value;
            m_shouldRecalculateCache = true;
        }

        /// @brief Get the far clipping plane distance, used for clipping objects out of the camera's frustum
        /// @return Current far clipping plane distance
        float GetFarClippingPlane() const 
        {
            return m_farClippingPlane;
        }

        /// @brief Set the far clipping plane distance, used for clipping objects out of the camera's frustum
        /// @param value New far clipping plane distance
        void SetFarClippingPlane(float value) 
        {
            m_farClippingPlane = value;
        }

        /// @brief Get the camera's field of view
        /// @return Current field of view
        float GetFOV() const 
        {
            return m_fieldOfView;
        }

        /// @brief Set the camera's field of view
        /// @param value New field of view distance
        void SetFOV(float value) 
        {
            m_fieldOfView = value;
            m_shouldRecalculateCache = true;
        }

    private:
        /// @brief Recalculate the camera ray directions cache for a specified screen size
        /// @param width Width of the screen in pixels
        /// @param height Height of the screen in pixels
        /// @note This method should be called if any of the following camera properties have changed:
        ///        - look direction,
        ///        - aspect ratio,  
        ///        - field of view,
        ///        - near clipping plane
        void RecalculateRayDirectionsCache(uint32_t width, uint32_t height);

    public:
        static constexpr glm::vec3 UP_DIRECTION = { .0f, 1.0f, .0f }; ///< World up vector used for camera rotation

        static constexpr float FOV_MIN = 45.0f; ///< Smallest valid value for camera's FOV in degrees
        static constexpr float FOV_MAX = 180.0f; ///< Largest valid value for camera's FOV in degrees
        static constexpr float NEAR_CLIP_MIN = 0.01f; ///< Smallest valid value for camera's near clipping plane distance
        static constexpr float NEAR_CLIP_MAX = 10.0f; ///< Largest valid value for camera's near clipping plane distance
        static constexpr float FAR_CLIP_MIN = 10.0f; ///< Smallest valid value for camera's far clipping plane distance
        static constexpr float FAR_CLIP_MAX = 1000.0f; ///< Largest valid value for camera's far clipping plane distance
        static constexpr float PITCH_MIN = -90.0f * yart::utils::DEG_TO_RAD + yart::utils::EPSILON; ///< Smallest valid value for the pitch rotation
        static constexpr float PITCH_MAX =  90.0f * yart::utils::DEG_TO_RAD - yart::utils::EPSILON; ///< Largest valid value for the pitch rotation

        glm::vec3 position = { 2.0f, 2.0f, -4.0f }; ///< World space position 

    private:
        glm::vec3 m_lookDirection = { .0f, .0f, 1.0f }; ///< Normalized look-at vector, calculated from yaw and pitch rotations
        float m_rotationYaw = 120.0f * yart::utils::DEG_TO_RAD; ///< Horizontal rotation in radians (around the y axis)
        float m_rotationPitch = -25.0f * yart::utils::DEG_TO_RAD; ///< Vertical rotation in radians (around the x axis)

        float m_nearClippingPlane = 0.25f; ///< Near clipping plane distance
        float m_farClippingPlane = 1000.0f; ///< Far clipping plane distance
        float m_fieldOfView = 60.0f; ///< Horizontal camera FOV in degrees
        
        std::vector<glm::vec3> m_rayDirectionsCache; ///< Cached ray directions for a given output size
        uint32_t m_rayDirectionsCacheWidth;
        uint32_t m_rayDirectionsCacheHeight;
        bool m_shouldRecalculateCache; ///< Whether the cache has been invalidated and should be rebuild 

    };
} // namespace yart
