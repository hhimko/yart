////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the World class 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "world.h"


#include "yart/utils/glm_utils.h"


namespace yart
{
    glm::vec3 World::SampleSkyColor(const glm::vec3& direction)
    {
        float t = (direction.y + 1.0f) / 2.0f; 
        return yart::utils::LinearGradient(m_skyGradientValues.data(), m_skyGradientLocations.data(), m_skyGradientValues.size(), t);
    }
} // namespace yart
