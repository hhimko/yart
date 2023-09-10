////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Resource base class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "resource.h"


#include "yart/core/RES/res_internal.h"


namespace yart
{
    namespace RES
    {
        const ResourceType Resource::CLASS_ID = ResourceType_COUNT;

        Resource::Resource(const char* name, resourceID_t id)
            : m_id(id), m_name(name) 
        {

        }

        Resource::Resource(const std::string& name, resourceID_t id)
            : m_id(id), m_name(name) 
        {

        }

    } // namespace RES
} // namespace yart
