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
        const ResourceType CubeMap::CLASS_ID = ResourceType_CubeMap;

        glm::vec3 CubeMap::Sample(const glm::vec3& direction)
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
                index = is_pos ? CubeMapCubeSide_PosX : CubeMapCubeSide_NegX;
            } else if (abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z) {
                // Y-major
                max_axis = abs_dir.y;
                const bool is_pos = direction.y > 0;

                u = direction.x;
                v = is_pos ? -direction.z : direction.z;
                index = is_pos ? CubeMapCubeSide_PosY : CubeMapCubeSide_NegY;
            } else if (abs_dir.z >= abs_dir.x && abs_dir.z >= abs_dir.y) {
                // Z-major
                max_axis = abs_dir.z;
                const bool is_pos = direction.z > 0;

                u = is_pos ? direction.x : -direction.x;
                v = direction.y;
                index = is_pos ? CubeMapCubeSide_PosZ : CubeMapCubeSide_NegZ;
            }

            // Convert range from [-1..1] to [0..1]
            u = (u / max_axis + 1.0f) / 2.0f;
            v = 1.0f - (v / max_axis + 1.0f) / 2.0f;

            return SampleImageNorm(m_images[index], u, v, m_interpolation);
        }

        CubeMap::CubeMap(const char *name, resourceID_t id, Image* pos_x, Image* pos_y, Image* pos_z, Image* neg_x, Image* neg_y, Image* neg_z)
            : Resource(name, id)
        {
            m_images[CubeMapCubeSide_PosX] = pos_x;
            m_images[CubeMapCubeSide_PosY] = pos_y;
            m_images[CubeMapCubeSide_PosZ] = pos_z;
            m_images[CubeMapCubeSide_NegX] = neg_x;
            m_images[CubeMapCubeSide_NegY] = neg_y;
            m_images[CubeMapCubeSide_NegZ] = neg_z;
        }

        CubeMap* CubeMap::LoadDefault()
        {
            Image* px = LoadImageFromFile("../res/defaults/DefaultCubeMap_PX.png");
            Image* py = LoadImageFromFile("../res/defaults/DefaultCubeMap_PY.png");
            Image* pz = LoadImageFromFile("../res/defaults/DefaultCubeMap_PZ.png");
            Image* nx = LoadImageFromFile("../res/defaults/DefaultCubeMap_NX.png");
            Image* ny = LoadImageFromFile("../res/defaults/DefaultCubeMap_NY.png");
            Image* nz = LoadImageFromFile("../res/defaults/DefaultCubeMap_NZ.png");

            return new CubeMap("Default CubeMap", DEFAULT_RESOURCE_ID, px, py, pz, nx, ny, nz);
        }

    } // namespace RES
} // namespace yart
