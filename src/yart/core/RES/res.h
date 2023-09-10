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

    } // namespace RES
} // namespace yart
