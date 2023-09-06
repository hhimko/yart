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
        enum class SkyType : uint8_t {
            SOLID_COLOR = 0,
            GRADIENT    = 1,
            SKYBOX      = 2,
            COUNT       = 3
        };


        SkyType m_skyType = SkyType::GRADIENT;

        static constexpr glm::vec3 DEFAULT_SKY_COLOR = { 0.131f, 0.241f, 0.500f };
        glm::vec3 m_skySolidColor = DEFAULT_SKY_COLOR;

        static constexpr glm::vec3 DEFAULT_SKY_GRADIENT_VALUE1 = { 0.158f, 0.180f, 0.195f };
        static constexpr glm::vec3 DEFAULT_SKY_GRADIENT_VALUE2 = { 0.158f, 0.180f, 0.195f };
        std::vector<glm::vec3> m_skyGradientValues = { DEFAULT_SKY_GRADIENT_VALUE1, DEFAULT_SKY_GRADIENT_VALUE2 };

        static constexpr float DEFAULT_SKY_GRADIENT_LOCATION1 = 0.0f;
        static constexpr float DEFAULT_SKY_GRADIENT_LOCATION2 = 1.0f;
        std::vector<float> m_skyGradientLocations = { DEFAULT_SKY_GRADIENT_LOCATION1, DEFAULT_SKY_GRADIENT_LOCATION2 };


        // -- FRIEND DECLARATIONS -- //
        friend class yart::GUI::WorldView;

    };  
} // namespace yart
