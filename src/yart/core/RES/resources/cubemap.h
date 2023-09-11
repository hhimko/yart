////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the CubeMap resource type 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <cstdint>

#include "glm/glm.hpp"

#include "yart/core/RES/res.h"
#include "resource.h"


namespace yart
{
    namespace RES
    {
        /// @brief Internal Image struct forward declaration
        struct Image;


        /// @brief Cubemap resource type used for rendering skyboxes 
        class CubeMap : public Resource {
        public:
            /// @brief Sample the cubemap at a given direction
            /// @param direction Unit direction vector
            /// @return Color at the sampled point
            glm::vec3 Sample(const glm::vec3& direction);

        private:
            /// @brief Construct a new CubeMap object from 6 images for each cube face
            /// @param name Display name
            /// @param id Resource ID
            /// @param pos_z Positive Z-axis cube face image
            CubeMap(const char* name, resourceID_t id, Image* pos_z);

            /// @brief Load and return the default cubemap
            /// @details The caller is expected to free the returned default instance 
            /// @note LoadDefault() should always ensure to return a valid pointer
            /// @return Pointer to the default resource
            static CubeMap* LoadDefault();

        private:
            /// @brief Internal enumerator for cube sides
            enum CubeMapCubeSide_ : uint8_t {
                CubeMapCubeSide_PosX,
                CubeMapCubeSide_PosY,
                CubeMapCubeSide_PosZ,
                CubeMapCubeSide_NegX,
                CubeMapCubeSide_NegY,
                CubeMapCubeSide_NegZ,
                CubeMapCubeSide_COUNT
            };


            /// @brief `Resource` subclass identifier number
            static const ResourceType CLASS_ID;

            /// @brief Cube face images
            Image* m_images[CubeMapCubeSide_COUNT];


            // -- FRIEND DECLARATIONS -- //
            friend CubeMap* yart::RES::GetDefault<CubeMap>();

        };

    } // namespace RES
} // namespace yart
