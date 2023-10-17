////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the static MeshFactory class, for instancing mesh primitives
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mesh_factory.h"


namespace yart
{
    Mesh* MeshFactory::CubeMesh(const glm::vec3& origin)
    {
        Mesh* mesh = new Mesh();

        mesh->verticesCount = 8;
        mesh->vertices = new glm::vec3[mesh->verticesCount] {
            { origin.x - 0.5f, origin.y - 0.5f, origin.z - 0.5f },
            { origin.x - 0.5f, origin.y + 0.5f, origin.z - 0.5f },
            { origin.x - 0.5f, origin.y - 0.5f, origin.z + 0.5f },
            { origin.x - 0.5f, origin.y + 0.5f, origin.z + 0.5f },
            { origin.x + 0.5f, origin.y - 0.5f, origin.z + 0.5f },
            { origin.x + 0.5f, origin.y + 0.5f, origin.z + 0.5f },
            { origin.x + 0.5f, origin.y - 0.5f, origin.z - 0.5f },
            { origin.x + 0.5f, origin.y + 0.5f, origin.z - 0.5f }
        };

        mesh->trianglesCount = 12;
        mesh->triangleIndices = new glm::u32vec3[mesh->trianglesCount] {
            { 0, 1, 7 }, // Front face
            { 7, 6, 0 },
            { 2, 3, 1 }, // Left face
            { 1, 0, 2 },
            { 4, 5, 3 }, // Back face
            { 3, 2, 4 },
            { 6, 7, 5 }, // Right face
            { 5, 4, 6 },
            { 1, 3, 5 }, // Top face
            { 5, 7, 1 },
            { 2, 0, 6 }, // Bottom face
            { 6, 4, 2 }
        };

        mesh->uvsCount = 4;
        mesh->uvs = new glm::vec2[mesh->uvsCount] {
            { 0.0f, 0.0f },
            { 0.0f, 1.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f }
        };

        mesh->triangleVerticesUvs = new glm::u32vec3[mesh->trianglesCount] {
            { 0, 1, 3 }, // Front face
            { 3, 2, 0 },
            { 0, 1, 3 }, // Left face
            { 3, 2, 0 },
            { 0, 1, 3 }, // Back face
            { 3, 2, 0 },
            { 0, 1, 3 }, // Right face
            { 3, 2, 0 },
            { 0, 1, 3 }, // Top face
            { 3, 2, 0 },
            { 0, 1, 3 }, // Bottom face
            { 3, 2, 0 }
        };

        return mesh;
    }
} // namespace yart
