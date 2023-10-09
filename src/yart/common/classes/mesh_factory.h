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
        glm::vec3* vertices = nullptr;
        uint32_t verticesCount;
        glm::u32vec3* triangleIndices = nullptr;
        uint32_t triangleIndicesCount;
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
