////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Scene object types and helper classes implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "object.h"


#include "yart/core/scene.h"


namespace yart
{
    Object::Object(const char* name, Object::MeshData& data)
        : m_type(ObjectType::MESH), m_id(GenerateID()), m_name(name)
    {
        m_meshData = data;
    }

    Object::Object(const char* name, Object::LightData& data)
        : m_type(ObjectType::LIGHT), m_id(GenerateID()), m_name(name)
    {
        m_lightData = data;
    }

    Object::id_t Object::GenerateID()
    {
        static id_t gen = (id_t)0;
        return gen++;
    }

} // namespace yart
