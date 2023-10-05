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
        glm::u32vec3* triangleIndices = nullptr;
    };


    class MeshFactory {
        /// @brief Create a new unit cube mesh, centered around point (0,0,0)
        /// @return Mesh primitive
        static Mesh* CubeMesh();

    };
} // namespace yart
