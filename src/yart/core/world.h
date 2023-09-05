////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the World class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <glm/glm.hpp>

#include "yart/GUI/views/world_view.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Rendered scene environment definition class
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class World {
    public:
        World() = default;

        /// @brief Get the sky color at a given direction 
        /// @param direction Unit direction vector
        /// @return Color at the sampled point
        glm::vec3 SampleSkyColor(const glm::vec3& direction);

    private:
        /// @brief Types of renderable environment skies
        enum class SkyType {
            SOLID_COLOR,
            GRADIENT,
            SKYBOX
        };


        SkyType m_skyType = SkyType::SOLID_COLOR;
        glm::vec3 m_skySolidColor = {};
        std::vector<glm::vec3> m_skyGradientValues = { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
        std::vector<float> m_skyGradientLocations = { 0.0f, 1.0f };

        // -- FRIEND DECLARATIONS -- //
        friend class yart::GUI::WorldView;

    };  
} // namespace yart
