////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the static MeshFactory class, for instancing mesh primitives
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "mesh_factory.h"


#include <glm/glm.hpp>

#include "yart/common/utils/glm_utils.h"


namespace yart
{
    Mesh* MeshFactory::CubeMesh(const glm::vec3& origin)
    {
        Mesh* mesh = new Mesh();
        static constexpr float size = 1.0f;
        const float rad = size / 2.0f;

        mesh->verticesCount = 8;
        mesh->vertices = new glm::vec3[mesh->verticesCount] {
            { origin.x - rad, origin.y - rad, origin.z - rad },
            { origin.x - rad, origin.y + rad, origin.z - rad },
            { origin.x - rad, origin.y - rad, origin.z + rad },
            { origin.x - rad, origin.y + rad, origin.z + rad },
            { origin.x + rad, origin.y - rad, origin.z + rad },
            { origin.x + rad, origin.y + rad, origin.z + rad },
            { origin.x + rad, origin.y - rad, origin.z - rad },
            { origin.x + rad, origin.y + rad, origin.z - rad }
        };

        mesh->trianglesCount = 12;
        mesh->triangleIndices = new glm::u32vec3[mesh->trianglesCount] {
            { 0, 1, 7 }, { 7, 6, 0 }, // Front face
            { 2, 3, 1 }, { 1, 0, 2 }, // Left face
            { 4, 5, 3 }, { 3, 2, 4 }, // Back face
            { 6, 7, 5 }, { 5, 4, 6 }, // Right face
            { 1, 3, 5 }, { 5, 7, 1 }, // Top face
            { 2, 0, 6 }, { 6, 4, 2 }  // Bottom face
        };

        mesh->uvsCount = 4;
        mesh->uvs = new glm::vec2[mesh->uvsCount] {
            { 0.0f, 0.0f },
            { 0.0f, 1.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f }
        };

        mesh->triangleVerticesUvs = new glm::u32vec3[mesh->trianglesCount] {
            { 0, 1, 3 }, { 3, 2, 0 }, // Front face
            { 0, 1, 3 }, { 3, 2, 0 }, // Left face
            { 0, 1, 3 }, { 3, 2, 0 }, // Back face
            { 0, 1, 3 }, { 3, 2, 0 }, // Right face
            { 0, 1, 3 }, { 3, 2, 0 }, // Top face
            { 0, 1, 3 }, { 3, 2, 0 }  // Bottom face
        };

        return mesh;
    }

    Mesh* MeshFactory::UvSphereMesh(const glm::vec3& origin, size_t num_segments, size_t num_rings)
    {
        Mesh* mesh = new Mesh();
        static constexpr float radius = 0.5f;

        mesh->verticesCount = num_segments * (num_rings - 1) + 2;
        mesh->vertices = new glm::vec3[mesh->verticesCount];

        mesh->vertices[0] = origin + glm::vec3{ 0.0f, radius, 0.0f }; // Top vertex

        for (size_t i = 0; i < num_rings - 1; ++i) {
            float phi = yart::utils::PI * static_cast<float>(i + 1) / static_cast<float>(num_rings);
            float sin_phi = glm::sin(phi);
            float cos_phi = glm::cos(phi);

            for (size_t j = 0; j < num_segments; ++j) {
                float theta = 2.0f * yart::utils::PI * static_cast<float>(j) / static_cast<float>(num_segments);

                float x = sin_phi * glm::cos(theta);
                float y = cos_phi;
                float z = sin_phi * glm::sin(theta);

                mesh->vertices[i * num_segments + j + 1] = origin + glm::vec3{ x, y, z } * radius;
            }
        }

        mesh->vertices[mesh->verticesCount - 1] = origin + glm::vec3{ 0.0f, -radius, 0.0f }; // Bottom vertex

        
        mesh->trianglesCount = (num_rings - 1) * num_segments * 2;
        mesh->triangleIndices = new glm::u32vec3[mesh->trianglesCount];

        // Topmost & bottommost tris
        for (size_t i = 0; i < num_segments; ++i) {
            const size_t j = (i + 1) % num_segments;
            mesh->triangleIndices[i] = { 0, j + 1, i + 1 };

            const size_t offset = num_segments * (num_rings - 2) + 1;
            mesh->triangleIndices[mesh->trianglesCount - num_segments + i] = { mesh->verticesCount - 1, offset + i, offset + j };
        }

        // Inner quads
        for (size_t i = 0; i < num_rings - 2; ++i) {
            auto i0 = i * num_segments + 1;
            auto i1 = (i + 1) * num_segments + 1;

            for (size_t j = 0; j < num_segments; ++j) {
                const size_t j1 = (j + 1) % num_segments;

                const size_t idx = num_segments + (i * num_segments + j) * 2;
                mesh->triangleIndices[idx + 0] = { i0 + j, i0 + j1, i1 + j1 };
                mesh->triangleIndices[idx + 1] = { i0 + j, i1 + j1, i1 + j  };
            }
        }

        return mesh;
    }
} // namespace yart
