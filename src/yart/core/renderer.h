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
#include "yart/core/camera.h"
#include "yart/core/scene.h"
#include "yart/core/world.h"
#include "yart/core/ray.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART offline ray tracing renderer
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Renderer {
    public:
        /// @brief Render the active scene to a given buffer
        /// @param camera YART camera instance, from which perspective to render
        /// @param buffer Pointer to a pixel array to be rendered onto. 
        ///     The size of the array should be equal to width*height*4, where 4 denotes the number of channels in the output image (RGBA)
        /// @param width Width in pixels of the output image
        /// @param height Height in pixels of the output image
        /// @return Whether the current frame has changed visually from the previous rendered frame (used for conditional viewport refreshing) 
        bool Render(yart::Camera& camera, float buffer[], uint32_t width, uint32_t height);

        /// @brief Render the active scene directly to a given viewport
        /// @param camera YART camera instance, from which perspective to render
        /// @param viewport Viewport to render to
        /// @return Whether the current frame has changed visually from the previous rendered frame (used for conditional viewport refreshing) 
        bool Render(yart::Camera& camera, const yart::Viewport& viewport);

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
            float hitDistance; ///< Distance from the ray origin to a registered hit surface, or a negative value on ray miss
            yart::Object* hitObject; ///< Object hit by the ray
            glm::vec3 hitNormal; ///< Normal vector of the hit surface
            glm::vec3 hitPosition; ///< Hit position vector in world-space
            glm::vec3 resultColor; ///< Color of the hit surface
        };

        /// @brief Trace a ray with a specified number of max bounces and store the results in a HitPayload structure
        /// @param camera YART camera instance, from which to trace the rays
        /// @param ray Traced ray
        /// @param payload HitPayload structure, where the ray tracing results will be stored
        /// @param bounces Max number of bounces
        void TraceRay(yart::Camera& camera, const yart::Ray& ray, HitPayload& payload, uint8_t bounces);

        /// @brief Shoot a single ray into the scene and store the results in a HitPayload structure
        /// @param near Near clipping plane distance
        /// @param far Far clipping plane distance
        /// @param ray 
        /// @param payload HitPayload structure, where the ray tracing results will be stored
        /// @return Whether the ray has hit an object on it's path. Used for terminating reflection bounces
        bool TraceRaySingle(float near, float far, const yart::Ray& ray, HitPayload& payload);

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

    private:
        std::unique_ptr<yart::World> m_world = std::make_unique<World>();
        std::shared_ptr<yart::Scene> m_scene;

        bool m_showOverlays = true; // Whether the overlays layer should be rendered
        bool m_useThickerGrid = false; // Whether the overlay grid should use a thicker outline
        bool m_debugShading = false; // Whether to render the surface uvs or normals as the object's material 
        bool m_materialUvs = false; // Whether to render the surface uvs as the object's material when `m_debugShading` is true
        bool m_shadows = true; // Whether to cast and render surface shadows


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Interface::RendererView;

    };
} // namespace yart
