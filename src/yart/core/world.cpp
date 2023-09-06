////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the World class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "world.h"


#include "yart/utils/yart_utils.h"
#include "yart/utils/glm_utils.h"


namespace yart
{
    glm::vec3 World::SampleSkyColor(const glm::vec3& direction)
    {
        switch (m_skyType) {
        case SkyType::SOLID_COLOR: {
            return m_skySolidColor;
        }
        case SkyType::GRADIENT: {
            float t = (direction.y + 1.0f) / 2.0f; 
            return yart::utils::LinearGradient(m_skyGradientValues.data(), m_skyGradientLocations.data(), m_skyGradientValues.size(), t);
        }
        case SkyType::SKYBOX: {
            return {1,1,1};
        }
        case SkyType::COUNT:
            break;
        }

        YART_UNREACHABLE();
        return glm::vec3{};
    }
} // namespace yart
