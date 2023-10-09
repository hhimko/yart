////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scene.h"


namespace yart
{
    Scene::Scene()
    {
        if (m_vertHeapCapacity <= 0) m_vertHeapCapacity = DEFAULT_VHEAP_CAPACITY;
        m_vertHeap = new glm::vec3[m_vertHeapCapacity];

        if (m_triHeapCapacity <= 0) m_triHeapCapacity = DEFAULT_THEAP_CAPACITY;
        m_triHeap = new glm::u32vec3[m_triHeapCapacity];
    }

    Scene::~Scene()
    {
        // Free the allocated memory blocks
        delete[] m_vertHeap;
        delete[] m_triHeap;
    }

    void Scene::LoadDefault()
    {
        Mesh* mesh = MeshFactory::CubeMesh({ 0, 0, 0 });
        AddMeshObject("Default object", mesh);

        delete[] mesh->vertices;
        delete[] mesh->triangleIndices;
        delete mesh;
    }

    float Scene::IntersectRay(const Ray& ray, Object* hit_obj)
    {
        float t, u, v;
        for (auto&& obj : m_objects) {
            for (auto&& triangle_indices : obj.tris) {
                const glm::vec3& v0 = obj.verts[triangle_indices.x];
                const glm::vec3& v1 = obj.verts[triangle_indices.y];
                const glm::vec3& v2 = obj.verts[triangle_indices.z];

                if (yart::Ray::IntersectTriangle(ray, v0, v1, v2, t, u, v)) {
                    hit_obj = &obj;
                    return t;
                }
            }
        }

        return -1.0f;
    }

    Object* Scene::AddMeshObject(const char* name, Mesh* mesh)
    {
        Object::MeshData mesh_data = { };

        Object object(name, mesh_data);
        object.verts = { mesh->vertices, mesh->vertices + mesh->verticesCount };
        object.tris = { mesh->triangleIndices, mesh->triangleIndices + mesh->triangleIndicesCount };

        m_objects.push_back(object);
        return &object;
    }
} // namespace yart