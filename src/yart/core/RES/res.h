////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief RES module global interface for creating and retrieving application resources
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "resources/resource.h"


namespace yart
{
    namespace RES
    {
        /// @brief Retrieve a particular resource type's default instance
        /// @tparam TResource `Resource`-derived type
        /// @return Pointer to the default resource. Ensured to always return a valid pointer
        template<class TResource>
        TResource* GetDefault();

        /// @brief Retrieve a particular resource type by ID
        /// @tparam TResource `Resource`-derived type
        /// @param id ID to search for. Set to the default ID if not found
        /// @return Pointer to the resource. The default is returned if Id was not found
        template<class TResource>
        TResource* GetResourceByID(resourceID_t& id);

    } // namespace RES
} // namespace yart
