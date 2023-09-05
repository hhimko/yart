////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <cstdint>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "yart/GUI/views/renderer_view.h"
#include "world.h"
#include "ray.h"


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
        /// @return Whether the current frame has changed visually from the previous rendered frame (used for conditional viewport refreshing) 
        bool Render(float buffer[], uint32_t width, uint32_t height);

        yart::World& GetWorld() const 
        {
            return *m_world;
        }

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Structure holding data returned as a result of tracing a ray into the scene 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        struct HitPayload {
            /// @brief Distance from the ray origin to a registered hit surface, or a negative value on ray miss
            float hitDistance;

            union {
                glm::vec3 resultColor;
                glm::vec3 hitPosition;
            };
        };


        /// @brief Set the render output image size 
        /// @param width Width of the render output in pixels 
        /// @param height Height of the render output in pixels 
        void Resize(uint32_t width, uint32_t height);

        /// @brief Shoot a ray into the scene and store the results in a HitPayload structure
        /// @param ray Traced ray
        /// @param payload HitPayload structure, where the ray tracing results will be stored
        void TraceRay(const yart::Ray& ray, HitPayload& payload);

        /// @brief Simulated `ClosestHit()` shader, executed on the closest scene geometry hit by the ray
        /// @param ray Traced ray
        /// @param payload HitPayload structure, where the ray tracing results will be stored
        void ClosestHit(const yart::Ray& ray, HitPayload& payload);

        /// @brief Simulated `Miss()` shader, executed when no scene geometry is hit by the ray
        /// @param ray Traced ray
        /// @param payload HitPayload structure, where the ray tracing results will be stored
        void Miss(const yart::Ray& ray, HitPayload& payload);

        /// @brief Recalculate the inverse view projection matrix, used for transforming screen space coordinates into world space 
        /// @note This method should be called if any of the following camera properties have changed:
        ///        - aspect ratio,  
        ///        - field of view,
        ///        - near clipping plane
        void RecalculateCameraTransformationMatrix();

    private:
        std::unique_ptr<yart::World> m_world = std::make_unique<World>();
        uint32_t m_width = 0; // Width of the render output in pixels 
        uint32_t m_height = 0; // Height of the render output in pixels 
        bool m_dirty = true; // Signals whether the current frame has changed visually from the previous rendered frame (used for conditional viewport refreshing) 

        // Iterators used by Renderer::Render for multithreading
        std::vector<uint32_t> m_verticalPixelIterator;
        std::vector<uint32_t> m_horizontalPixelIterator;

        // -- CAMERA DATA -- // 
        static constexpr glm::vec3 UP_DIRECTION = { .0f, 1.0f, .0f }; // World up vector used for camera positioning

        glm::vec3 m_cameraPosition = { .0f, .0f, -5.0f }; // World space position 
        glm::vec3 m_cameraLookDirection = { .0f, .0f, 1.0f }; // Normalized look-at vector, calculated from camera's yaw and pitch rotations
        float m_cameraYaw = 90.0f * yart::utils::DEG_TO_RAD; // Horizontal camera rotation in radians (around the y axis)
        float m_cameraPitch = 0.0f; // Vertical camera rotation in radians (around the x axis)
        
        float m_fieldOfView = 90.0f; // Horizontal camera FOV in degrees
        float m_nearClippingPlane = 0.1f;
        float m_farClippingPlane = 1000.0f;

        // Cached view projection matrix inverse for transforming screen space coordinates into world space 
        glm::mat4 m_inverseViewProjectionMatrix; 


        // -- FRIEND DECLARATIONS -- //
        friend class yart::GUI::RendererView;

    };
} // namespace yart
