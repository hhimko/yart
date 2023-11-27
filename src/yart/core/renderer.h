////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Renderer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <cstdint>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

#include "yart/interface/views/renderer_view.h"
#include "yart/core/viewport.h"
#include "scene.h"
#include "world.h"
#include "ray.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART offline ray tracing renderer
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Renderer {
    public:
        Renderer();
        ~Renderer() = default;

        /// @brief Render the active scene to a given buffer
        /// @param buffer Pointer to a pixel array to be rendered onto. 
        ///     The size of the array should be equal to width*height*4, where 4 denotes the number of channels in the output image (RGBA)
        /// @param width Width in pixels of the output image
        /// @param height Height in pixels of the output image
        /// @return Whether the current frame has changed visually from the previous rendered frame (used for conditional viewport refreshing) 
        bool Render(float buffer[], uint32_t width, uint32_t height);

        /// @brief Render the active scene directly to a given viewport
        /// @param viewport Viewport to render to
        /// @return Whether the current frame has changed visually from the previous rendered frame (used for conditional viewport refreshing) 
        bool Render(const yart::Viewport* viewport);

        /// @brief Set a scene to be used for rendering by this renderer
        /// @param scene New scene instance
        void SetScene(std::shared_ptr<yart::Scene> scene) 
        {
            m_scene = scene;
        }

        /// @brief Get the renderer's world instance 
        /// @return Renderer's world instance pointer
        yart::World* GetWorld() const 
        {
            return m_world.get();
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

        /// @brief Sample the overlays/gizmos layer from a given ray
        /// @param ray Traced ray
        /// @param color Output parameter set to the color at the hit point, or a transparent color on miss
        /// @return Distance from the ray origin to the closest hit, or a negative value on miss
        float SampleOverlaysView(const yart::Ray& ray, glm::vec4& color);

        /// @brief Simulated `ClosestHit()` shader, executed on the closest scene geometry hit by the ray
        /// @param ray Traced ray
        /// @param payload HitPayload structure, where the ray tracing results will be stored
        void ClosestHit(const yart::Ray& ray, HitPayload& payload);

        /// @brief Simulated `Miss()` shader, executed when no scene geometry is hit by the ray
        /// @param ray Traced ray
        /// @param payload HitPayload structure, where the ray tracing results will be stored
        void Miss(const yart::Ray& ray, HitPayload& payload);

        /// @brief Recalculate the camera ray direction cache
        /// @note This method should be called if any of the following camera properties have changed:
        ///        - look direction
        ///        - aspect ratio,  
        ///        - field of view,
        ///        - near clipping plane
        void RecalculateRayDirections();

    private:
        std::unique_ptr<yart::World> m_world = std::make_unique<World>();
        std::shared_ptr<yart::Scene> m_scene;
        uint32_t m_width = 0; // Width of the render output in pixels 
        uint32_t m_height = 0; // Height of the render output in pixels 

        bool m_showOverlays = true; // Whether the overlays layer should be rendered
        bool m_useThickerGrid = false; // Whether the overlay grid should use a thicker outline

        // -- CAMERA DATA -- // 
        static constexpr glm::vec3 UP_DIRECTION = { .0f, 1.0f, .0f }; // World up vector used for camera positioning

        glm::vec3 m_cameraPosition = { 2.0f, 2.0f, -4.0f }; // World space position 
        glm::vec3 m_cameraLookDirection = { .0f, .0f, 1.0f }; // Normalized look-at vector, calculated from camera's yaw and pitch rotations
        float m_cameraYaw = 120.0f * yart::utils::DEG_TO_RAD; // Horizontal camera rotation in radians (around the y axis)
        float m_cameraPitch = -25.0f * yart::utils::DEG_TO_RAD; // Vertical camera rotation in radians (around the x axis)
        
        float m_fieldOfView = 60.0f; // Horizontal camera FOV in degrees
        float m_nearClippingPlane = 0.1f;
        float m_farClippingPlane = 1000.0f;

        // Cached camera ray directions
        std::vector<glm::vec3> m_rayDirections;


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Interface::RendererView;

    };
} // namespace yart
