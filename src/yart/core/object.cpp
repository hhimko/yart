////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Scene object types and helper classes implementations
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "object.h"

#include "yart/core/scene.h"


namespace yart
{
    glm::mat4 Object::GetTransformationMatrix()
    {
        if (m_shouldRecalculateTransformationMatrix) {
            m_transformationMatrix[0][0] = scale.x;
            m_transformationMatrix[1][1] = scale.y;
            m_transformationMatrix[2][2] = scale.z;

            m_transformationMatrix[3][0] = position.x;
            m_transformationMatrix[3][1] = position.y;
            m_transformationMatrix[3][2] = position.z;

            m_transformationMatrix[3][3] = 1.0f;
            
            m_shouldRecalculateTransformationMatrix = false;
        }

        return m_transformationMatrix;
    }

    Object::Object(std::string& name, Object::MeshData& data)
        : m_type(ObjectType::MESH), m_id(GenerateID()), m_name(name)
    {
        m_meshData = data;
    }

    Object::Object(std::string& name, Object::LightData& data)
        : m_type(ObjectType::LIGHT), m_id(GenerateID()), m_name(name)
    {
        m_lightData = data;
    }

    Object::Object(std::string& name, SdfData& data)
        : m_type(ObjectType::SDF), m_id(GenerateID()), m_name(name)
    {
        m_sdfData = data;
    }

    Object::id_t Object::GenerateID()
    {
        static id_t gen = (id_t)0;
        return gen++;
    }

} // namespace yart
