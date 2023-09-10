////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal definitions for the RES module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "res_internal.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace yart
{
    namespace RES
    {
        ResourcesContext& GetResourcesContext()
        {
            static ResourcesContext s_currentContext;
            return s_currentContext;
        }

    } // namespace RES
} // namespace yart
