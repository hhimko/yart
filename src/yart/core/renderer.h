////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <cstdint>
#include <vector>

#include <glm/glm.hpp>


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART offline ray tracing renderer
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Renderer {
    public:
        Renderer() = default;
        ~Renderer() = default;

        /// @brief Render scene to a given buffer
        /// @param buffer Pointer to a pixel array to be rendered onto. 
        ///     The size of the array should be equal to width*height*4, where 4 denotes the number of channels in the output image (RGBA)
        /// @param width Width in pixels of the output image
        /// @param height Height in pixels of the output image
        void Render(float buffer[], uint32_t width, uint32_t height);

        /// @brief Issue ImGui draw commands for the renderer
        void OnImGui();


        // -- CAMERA PUBLIC METHODS -- //

        /// @brief Update the camera position based on user input
        /// @return Whether the camera has been moved
        bool UpdateCamera();

    private:
        /// @brief Set the render output image size 
        /// @param width Width of the render output in pixels 
        /// @param height Height of the render output in pixels 
        void Resize(uint32_t width, uint32_t height);


        // -- CAMERA PRIVATE METHODS -- // 

        /// @brief Recalculate the inverse view projection matrix, used for transforming screen space coordinates into world space 
        /// @note This method should be called if any of the following camera properties have changed:
        ///        - aspect ratio,  
        ///        - field of view,
        ///        - near clipping plane
        void RecalculateCameraTransformationMatrix();

        /// @brief Rotate the camera around its origin based on a screen position cursor movement delta 
        /// @param mouse_drag Amount of pixels moved by the cursor since last frame
        void RotateCameraWithMouseMoveDelta(const glm::ivec2& mouse_drag);

    private:
        static constexpr float PI = 3.14159274f; 
        static constexpr float DEG_TO_RAD = PI / 180.0f; // Degrees to radians conversion constant

        uint32_t m_width = 0; // Width of the render output in pixels 
        uint32_t m_height = 0; // Height of the render output in pixels 

        // Iterators used by Renderer::Render for multithreading
        std::vector<uint32_t> m_verticalPixelIterator;
        std::vector<uint32_t> m_horizontalPixelIterator;

        // -- CAMERA DATA -- // 
        static constexpr glm::vec3 UP_DIRECTION = { .0f, 1.0f, .0f }; // World up vector used for camera positioning
        static constexpr float CAMERA_PITCH_MIN = -90.0f * DEG_TO_RAD + 0.0001f;
        static constexpr float CAMERA_PITCH_MAX =  90.0f * DEG_TO_RAD - 0.0001f;

        glm::vec3 m_cameraPosition = { .0f, .0f, -5.0f }; // World space position 
        glm::vec3 m_cameraLookDirection = { .0f, .0f, 1.0f }; // Normalized look-at vector, calculated from camera's yaw and pitch rotations
        float m_cameraYaw = 90.0f * DEG_TO_RAD; // Horizontal camera rotation in radians (around the y axis)
        float m_cameraPitch = .0f; // Vertical camera rotation in radians (around the x axis)
        
        float m_fieldOfView = 90.0f; // Horizontal camera FOV in degrees
        float m_nearClippingPlane = 0.1f;
        float m_farClippingPlane = 1000.0f;

        float m_cameraMoveSpeed = 0.01f;

        // Cached view projection matrix inverse for transforming screen space coordinates into world space 
        glm::mat4 m_inverseViewProjectionMatrix; 

    };
} // namespace yart
