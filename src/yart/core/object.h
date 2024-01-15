////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Scene object types and helper classes definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>


namespace yart
{
    class Scene; // yart::Scene class forward declaration
    class SceneCollection; // yart::SceneCollection class forward declaration


    /// @brief Scene object types enum
    enum class ObjectType : uint8_t {
        MESH = 0, ///< Mesh object type
        LIGHT,    ///< Light object type
        SDF       ///< Signed Distance Field object type
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Generic scene object class holding data for different types of objects with 3D transform
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Object {
    public:
        /// @brief Object's uniquely identifying ID type
        using id_t = size_t;


        /// @brief Get the object ID
        /// @return Uniquely identifying object ID
        id_t GetId() const 
        {
            return m_id;
        }

        /// @brief Get the object display name
        /// @return Object name
        const char* GetName() const
        {
            return m_name.c_str();
        }

        /// @brief Signal to the object that its transformation has changed
        /// @details Calling this method ensures that the transformation matrix gets recalculated
        void TransformationChanged() 
        {
            m_shouldRecalculateTransformationMatrix = true;
        }

        glm::mat4 GetTransformationMatrix(); 

    private:
        /// @brief Structure containing data required to render a mesh object
        struct MeshData {

        };

        /// @brief Structure containing data required to render a light object
        struct LightData {

        };

        /// @brief Structure containing data required to render a SDF object
        struct SdfData {
            float radius; ///< Sphere radius
        };

        /// @brief Construct a new mesh type object 
        /// @param name Display name of the object
        /// @param data Mesh object type data
        Object(const char* name, MeshData& data);

        /// @brief Construct a new light type object 
        /// @param name Display name of the object
        /// @param data Light object type data
        Object(const char* name, LightData& data);

        /// @brief Construct a new light type object 
        /// @param name Display name of the object
        /// @param data Light object type data
        Object(const char* name, SdfData& data);

        /// @brief Generate a new unique ID
        /// @return Unique ID
        static id_t GenerateID();

    public:
        glm::vec3 scale    = { 1.0f, 1.0f, 1.0f };
        glm::vec3 position = { 0.0f, 0.0f, 0.0f };
        // glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };

    private:
        const id_t m_id; ///< Uniquely identifying ID of the object

        SceneCollection* m_collection = nullptr; ///< Scene object collection, to which the object belongs to
        const ObjectType m_type; ///< Underlying type of the object
        const std::string m_name; ///< Display name of the object

        union {
            /// @brief Objects mesh data
            /// @details Valid only when the `m_type` member variable is equal to ObjectType::MESH
            MeshData m_meshData;
            /// @brief Objects light data
            /// @details Valid only when the `m_type` member variable is equal to ObjectType::LIGHT
            LightData m_lightData;
            /// @brief Objects SDF data
            /// @details Valid only when the `m_type` member variable is equal to ObjectType::SDF
            SdfData m_sdfData;
        };

        glm::mat4 m_transformationMatrix { 0 }; ///< Cached object transformation matrix
        bool m_shouldRecalculateTransformationMatrix = true; 

        // Temporary mesh variables 
        std::vector<glm::vec3> verts;
        std::vector<glm::u32vec3> tris;
        // std::vector<glm::vec2> UVs;
        // std::vector<glm::u32vec3> triangleUVs;


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Scene;

    };
} // namespace yart
