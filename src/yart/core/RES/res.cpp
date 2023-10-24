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
        TResource* GetDefault()
        {            
            static_assert(std::is_base_of<Resource, TResource>::value, "TResource not derived from RES::Resource");
            ResContext* ctx = GetResContext();

            // Lazily load the default resource
            size_t class_id = static_cast<size_t>(TResource::CLASS_ID);
            if (ctx->defaultResources[class_id] == nullptr)
                ctx->defaultResources[class_id] = static_cast<Resource*>(TResource::LoadDefault()); // TODO: Default resources are not free'd 

            TResource* default_resource = (TResource*)ctx->defaultResources[class_id];
            YART_ASSERT(default_resource != nullptr);

            return default_resource;
        }

        // Explicit template instantiations
        #ifndef DOXYGEN_EXCLUDE // Exclude from documentation
            template CubeMap* GetDefault<CubeMap>();
        #endif // #ifndef DOXYGEN_EXCLUDE

        template <class TResource>
        TResource* GetResourceByID(resourceID_t& id)
        {
            static_assert(std::is_base_of<Resource, TResource>::value, "TResource not derived from RES::Resource");
            if (id == DEFAULT_RESOURCE_ID)
                return GetDefault<TResource>();


            TResource* resource = nullptr;

            // Fallback to returning the default resource if the ID was not found
            if (resource == nullptr) {
                id = DEFAULT_RESOURCE_ID; // Set the ID accordingly, to avoid searching for the invalid ID in the future
                return GetDefault<TResource>();
            }

            return resource;
        }

        // Explicit template instantiations
        #ifndef DOXYGEN_EXCLUDE // Exclude from documentation
            template CubeMap* GetResourceByID<CubeMap>(resourceID_t& id);
        #endif // #ifndef DOXYGEN_EXCLUDE

    } // namespace RES
} // namespace yart
