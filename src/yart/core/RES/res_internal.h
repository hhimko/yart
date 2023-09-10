////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal definitions for the RES module
/// @details This file should only ever be imported and used from within the RES module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include "yart/core/utils/yart_utils.h"
#include "resources/resource.h"


namespace yart
{
    namespace RES
    {
        /// @brief Internal enumerator for uniquely identifying Resource sub-types
        /// @note The underlying type should be compatible with the `ResourceType` typedef from `resources/resource.h`
        enum ResourceType_ : uint8_t {
            ResourceType_CUBEMAP = 0,
            ResourceType_COUNT   = 1
        };
        static_assert(sizeof(ResourceType_) == sizeof(ResourceType));


        /// @brief Internal `RES` module context object
        struct ResourcesContext {
        public:
            /// @brief Array of pointers to lazily-loaded default resources from each `Resource` child class  
            const Resource* defaultResources[ResourceType_COUNT] = {};

        };


        /// @brief Get the current `RES` module context
        /// @return The current context
        ResourcesContext& GetResourcesContext();

    } // namespace RES
} // namespace yart
