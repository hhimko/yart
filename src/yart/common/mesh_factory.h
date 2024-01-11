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
        /// @brief Mesh vertices array pointer. Size of the array should be equal to `verticesCount`
        glm::vec3* vertices = nullptr; 
        /// @brief Count of vertices in the mesh
        uint32_t verticesCount; 
        /// @brief Mesh triangle indices array pointer. Size of the array should be equal to `trianglesCount`
        glm::u32vec3* triangleIndices = nullptr; 
        /// @brief Count of triangles in the mesh
        uint32_t trianglesCount; 
        // /// @brief Array of UV coordinates for each vertex in the mesh. Size of the array should be equal to `uvsCount`
        // glm::vec2* uvs = nullptr;
        // /// @brief Size of the `uvs` array 
        // uint32_t uvsCount; 
        // /// @brief Map of triangle vertices to their corresponding UVs. Size of the array should be equal to `trianglesCount`
        // glm::u32vec3* triangleVerticesUvs = nullptr; 
    };


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Static factory class for generating primitive meshes
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class MeshFactory {
    public:
        /// @brief Generate a new unit cube mesh, centered around a given origin point
        /// @param origin Center point, around which to build the mesh
        /// @return Mesh object for the primitive
        static Mesh* CubeMesh(const glm::vec3& origin);

        /// @brief Generate a new UV sphere mesh, centered around a given origin point
        /// @param origin Center point, around which to build the mesh
        /// @param num_segments Number of sphere segments (vertical slices) to subdivide the mesh with.
        ///     The segment count should not be less than three
        /// @param num_rings Number of sphere rings (horizontal stacks) to subdivide the mesh with.
        ///     The ring count should not be less than three
        /// @return Mesh object for the primitive
        static Mesh* UvSphereMesh(const glm::vec3& origin, size_t num_segments = 32U, size_t num_rings = 16U);

        /// @brief Destroy a mesh object
        /// @param mesh Mesh object to be freed
        static void DestroyMesh(Mesh* mesh);

    };
} // namespace yart
