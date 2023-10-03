////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Scene object types and helper classes implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "object.h"


namespace yart
{
    Object::Object(const char* name, MeshData& data)
        : m_type(ObjectType::MESH), id(GenerateID()), name(name)
    {
        m_meshData = data;
    }

    Object::Object(const char* name, LightData& data)
        : m_type(ObjectType::LIGHT), id(GenerateID()), name(name)
    {
        m_lightData = data;
    }

    Object::id_t Object::GenerateID()
    {
        static id_t gen = (id_t)0;
        return gen++;
    }

} // namespace yart
