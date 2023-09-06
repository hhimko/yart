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
            return SampleSkyboxTexture(direction);
        }
        case SkyType::COUNT:
            break;
        }

        YART_UNREACHABLE();
        return glm::vec3{};
    }

    glm::vec3 World::SampleSkyboxTexture(const glm::vec3& direction)
    {
        glm::vec3 abs_dir = glm::abs(direction);
        float max_axis = 0.0f, u = 0.0f, v = 0.0f;
        size_t index = 0;
        
        if (abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z) {
            // X-major
            max_axis = abs_dir.x;
            const bool is_pos = direction.x > 0;

            u = is_pos ? -direction.z : direction.z;
            v = direction.y;
            index = is_pos ? 0 : 1;
        } 
        if (abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z) {
            // Y-major
            max_axis = abs_dir.y;
            const bool is_pos = direction.y > 0;

            u = direction.x;
            v = is_pos ? -direction.z : direction.z;
            index = is_pos ? 2 : 3;
        } 
        if (abs_dir.z >= abs_dir.x && abs_dir.z >= abs_dir.y) {
            // Z-major
            max_axis = abs_dir.z;
            const bool is_pos = direction.z > 0;

            u = is_pos ? direction.x : -direction.x;
            v = direction.y;
            index = is_pos ? 4 : 5;
        }

        // Convert range from [-1..1] to [0..texture_size]
        const size_t u_coord = static_cast<size_t>(glm::ceil(u / max_axis + 1.0f) / 2.0f * (m_skySkyboxTextureSize - 1));
        const size_t v_coord = static_cast<size_t>(glm::ceil(v / max_axis + 1.0f) / 2.0f * (m_skySkyboxTextureSize - 1));

        const float* val = m_skySkyboxTextures[index] + v_coord * m_skySkyboxTextureSize + u_coord;
        return { val[0], val[1], val[2] };
    }
} // namespace yart
