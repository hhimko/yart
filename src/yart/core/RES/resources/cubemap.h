////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the CubeMap resource type 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "glm/glm.hpp"

#include "yart/core/RES/res.h"
#include "resource.h"


namespace yart
{
    namespace RES
    {
        /// @brief Cubemap resource type used for rendering skyboxes 
        class CubeMap : public Resource {
        public:
            /// @brief Construct a new CubeMap object
            /// @param name Display name
            /// @param id Resource ID
            CubeMap(const char* name, resourceID_t id);


            /// @brief Sample the cubemap at a given direction
            /// @param direction Unit direction vector
            /// @return Color at the sampled point
            glm::vec3 Sample(const glm::vec3& direction);

        private:
            /// @brief Load and return the default cubemap
            /// @details The caller is expected to free the returned default instance 
            /// @note LoadDefault() should always ensure to return a valid pointer
            /// @return Pointer to the default resource
            static CubeMap* LoadDefault() {
                return new CubeMap("Default CubeMap", 0);
            }

        private:
            /// @brief `Resource` subclass identifier number
            static const ResourceType CLASS_ID;


            // -- FRIEND DECLARATIONS -- //
            friend CubeMap* yart::RES::GetDefault<CubeMap>();

        };

    } // namespace RES
} // namespace yart
