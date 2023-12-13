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

    private:
        /// @brief Structure containing data required to render a mesh object
        struct MeshData {
        public:

        };

        /// @brief Structure containing data required to render a light object
        struct LightData {
        public:

        };


        /// @brief Construct a new mesh type object 
        /// @param name Display name of the object
        /// @param data Mesh object type data
        Object(const char* name, MeshData& data);

        /// @brief Construct a new light type object 
        /// @param name Display name of the object
        /// @param data Light object type data
        Object(const char* name, LightData& data);

        /// @brief Generate a new unique ID
        /// @return Unique ID
        static id_t GenerateID();

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
        };

        // Temporary mesh variables 
        std::vector<glm::vec3> verts;
        std::vector<glm::u32vec3> tris;
        std::vector<glm::vec2> UVs;
        std::vector<glm::u32vec3> triangleUVs;


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Scene;
        friend class Scene;

    };
} // namespace yart
