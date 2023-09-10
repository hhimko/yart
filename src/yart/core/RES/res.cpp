////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief RES module global interface for creating and retrieving application resources
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "res.h"


#include <type_traits>

#include "yart/core/utils/yart_utils.h"
#include "resources/cubemap.h"
#include "res_internal.h"


namespace yart
{
    namespace RES
    {
        template <class TResource>
        TResource* GetDefault<TResource>()
        {            
            static_assert(std::is_base_of<Resource, TResource>::value, "TResource not derived from RES::Resource");
            ResourcesContext& ctx = GetResourcesContext();

            // Lazily load the default resource
            size_t class_id = static_cast<size_t>(TResource::CLASS_ID);
            if (ctx.defaultResources[class_id] == nullptr)
                ctx.defaultResources[class_id] = static_cast<Resource*>(TResource::LoadDefault()); // TODO: Default resources are not free'd 

            TResource* default_resource = (TResource*)ctx.defaultResources[class_id];
            YART_ASSERT(default_resource != nullptr);

            return default_resource;
        }

        // Explicit template instantiations
        template CubeMap* GetDefault<CubeMap>();

    } // namespace RES
} // namespace yart
