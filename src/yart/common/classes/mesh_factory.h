////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the static MeshFactory class, for instancing mesh primitives
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <glm/glm.hpp>


namespace yart
{
    /// @brief Mesh data used for defining 3D mesh objects
    struct Mesh {
        glm::vec3* vertices = nullptr; /// @brief Mesh vertices array pointer. Size of the array should be equal to `verticesCount`
        uint32_t verticesCount; /// @brief Count of vertices in the mesh
        glm::u32vec3* triangleIndices = nullptr; /// @brief Mesh triangle indices array pointer. Size of the array should be equal to `trianglesCount`
        uint32_t trianglesCount; /// @brief Count of triangles in the mesh
        glm::vec2* uvs = nullptr; /// @brief Array of UV coordinates for each vertex in the mesh. Size of the array should be equal to `uvsCount`
        uint32_t uvsCount; /// @brief Size of the `uvs` array
        glm::u32vec3* triangleVerticesUvs = nullptr; /// @brief Map of triangle vertices to their corresponding UVs. Size of the array should be equal to `trianglesCount`
    };


    /// @brief Static factory class for primitive meshes
    class MeshFactory {
    public:
        /// @brief Create a new unit cube mesh, centered around a given origin point
        /// @param origin Center point, around which to build the mesh
        /// @return Mesh primitive
        static Mesh* CubeMesh(const glm::vec3& origin);

    };
} // namespace yart
