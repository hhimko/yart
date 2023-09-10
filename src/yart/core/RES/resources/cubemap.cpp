////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CubeMap resource type 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cubemap.h"


#include "yart/core/RES/res_internal.h"


namespace yart
{
    namespace RES
    {
        const ResourceType CubeMap::CLASS_ID = ResourceType_CUBEMAP;

        CubeMap::CubeMap(const char *name, resourceID_t id)
            : Resource(name, id)
        {

        }

        glm::vec3 CubeMap::Sample(const glm::vec3 &direction)
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
            } else if (abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z) {
                // Y-major
                max_axis = abs_dir.y;
                const bool is_pos = direction.y > 0;

                u = direction.x;
                v = is_pos ? -direction.z : direction.z;
                index = is_pos ? 2 : 3;
            } else if (abs_dir.z >= abs_dir.x && abs_dir.z >= abs_dir.y) {
                // Z-major
                max_axis = abs_dir.z;
                const bool is_pos = direction.z > 0;

                u = is_pos ? direction.x : -direction.x;
                v = direction.y;
                index = is_pos ? 4 : 5;
            }

            // Convert range from [-1..1] to [0..texture_size]
            // const size_t u_coord = static_cast<size_t>(glm::round((u / max_axis + 1.0f) / 2.0f * (m_skyCubemapTextureSize - 1)));
            // const size_t v_coord = static_cast<size_t>(glm::round((v / max_axis + 1.0f) / 2.0f * (m_skyCubemapTextureSize - 1)));

            // const float* val = m_skyCubemapTextures[index] + (v_coord * m_skyCubemapTextureSize + u_coord) * 3;
            // return { val[0], val[1], val[2] };
            
            return {}; // Not implemented
        }

    } // namespace RES
} // namespace yart
