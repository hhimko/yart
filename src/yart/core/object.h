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
    // yart::Scene class forward declaration
    class Scene;


    /// @brief Scene object types enum
    enum class ObjectType : uint8_t {
        MESH = 0, /// @brief Mesh object type
        LIGHT,    /// @brief Light object type
    };


    class Object {
    public:
        /// @brief Object's uniquely identifying ID type
        using id_t = size_t;

        /// @brief Uniquely identifying ID of the object
        const id_t id;
        /// @brief Display name of the object
        std::string name;

    private:
        /// @brief Structure containing data required to render a mesh object
        struct MeshData {
        public:
            int x;
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
        /// @brief Underlying type of the object
        const ObjectType m_type;

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


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Scene;
        friend class Scene;

    };
} // namespace yart
