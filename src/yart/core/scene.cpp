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

    float Scene::IntersectRay(const Ray& ray, Object* hit_obj, float* u, float* v)
    {
        float t;
        for (auto&& obj : m_objects) {
            for (size_t i = 0; i < obj.tris.size(); ++i) {
                const glm::vec3& v0 = obj.verts[obj.tris[i].x];
                const glm::vec3& v1 = obj.verts[obj.tris[i].y];
                const glm::vec3& v2 = obj.verts[obj.tris[i].z];

                if (yart::Ray::IntersectTriangle(ray, v0, v1, v2, &t, u, v)) {
                    const float w = 1 - (*u) - (*v);
                    const glm::u32vec3& uv_indices = obj.triangleUVs[i];
                    const glm::vec2 tex_uv = w * obj.UVs[uv_indices.x] + (*u) * obj.UVs[uv_indices.y] + (*v) * obj.UVs[uv_indices.z];
                    
                    hit_obj = &obj;
                    *u = tex_uv.x;
                    *v = tex_uv.y;
                    
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
        object.tris = { mesh->triangleIndices, mesh->triangleIndices + mesh->trianglesCount };
        object.UVs = { mesh->uvs, mesh->uvs + mesh->uvsCount };
        object.triangleUVs = { mesh->triangleVerticesUvs, mesh->triangleVerticesUvs + mesh->trianglesCount };

        m_objects.push_back(object);
        return &object;
    }
} // namespace yart