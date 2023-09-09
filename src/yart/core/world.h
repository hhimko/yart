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
        /// @brief Sample the cubemap texture at a given direction
        /// @param direction Unit direction vector
        /// @return Color at the sampled point
        glm::vec3 SampleCubemapColor(const glm::vec3& direction);

    private:
        /// @brief Types of renderable environment skies
        enum class SkyType : uint8_t {
            SOLID_COLOR = 0,
            GRADIENT    = 1,
            CUBEMAP     = 2,
            COUNT       = 3
        };


        SkyType m_skyType = SkyType::GRADIENT;

        static constexpr glm::vec3 DEFAULT_SKY_COLOR = { 0.131f, 0.241f, 0.500f };
        glm::vec3 m_skySolidColor = DEFAULT_SKY_COLOR;

        static constexpr glm::vec3 DEFAULT_SKY_GRADIENT_VALUE1 = { 0.316f, 0.544f, 0.625f };
        static constexpr glm::vec3 DEFAULT_SKY_GRADIENT_VALUE2 = { 0.131f, 0.241f, 0.500f };
        std::vector<glm::vec3> m_skyGradientValues = { DEFAULT_SKY_GRADIENT_VALUE1, DEFAULT_SKY_GRADIENT_VALUE2 };

        static constexpr float DEFAULT_SKY_GRADIENT_LOCATION1 = 0.25f;
        static constexpr float DEFAULT_SKY_GRADIENT_LOCATION2 = 1.00f;
        std::vector<float> m_skyGradientLocations = { DEFAULT_SKY_GRADIENT_LOCATION1, DEFAULT_SKY_GRADIENT_LOCATION2 };

        static constexpr float DEFAULT_SKY_CUBEMAP_TEXTURE[12] = { 
            1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f 
        };
        size_t m_skyCubemapTextureSize = 2;
        const float* m_skyCubemapTextures[6] = { 
            DEFAULT_SKY_CUBEMAP_TEXTURE, // Positive X axis
            DEFAULT_SKY_CUBEMAP_TEXTURE, // Negative X axis
            DEFAULT_SKY_CUBEMAP_TEXTURE, // Positive Y axis
            DEFAULT_SKY_CUBEMAP_TEXTURE, // Negative Y axis
            DEFAULT_SKY_CUBEMAP_TEXTURE, // Positive Z axis
            DEFAULT_SKY_CUBEMAP_TEXTURE  // Negative Z axis
        };


        // -- FRIEND DECLARATIONS -- //
        friend class yart::GUI::WorldView;

    };  
} // namespace yart
