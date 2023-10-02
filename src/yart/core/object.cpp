////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Scene object types and helper classes implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "object.h"


namespace yart
{
    Object::Object(MeshData& data)
        : m_type(ObjectType::MESH), id(GenerateID())
    {
        m_meshData = data;
    }

    Object::Object(LightData& data)
        : m_type(ObjectType::LIGHT), id(GenerateID())
    {
        m_lightData = data;
    }

    Object::id_t Object::GenerateID()
    {
        static id_t gen = (id_t)0;
        return gen++;
    }

} // namespace yart
