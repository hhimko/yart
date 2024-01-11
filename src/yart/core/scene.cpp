////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scene.h"


#include <limits>

#include "yart/common/utils/yart_utils.h"


namespace yart
{
    Scene::Scene()
    {
        m_objects.reserve(100);
    }

    Scene::~Scene()
    {

    }

    void Scene::LoadDefault()
    {
        Mesh* cube_mesh = MeshFactory::CubeMesh({ 0, 0, 0 });
        AddMeshObject("Default Cube", cube_mesh);

        MeshFactory::DestroyMesh(cube_mesh);
    }

    void Scene::ToggleSelection(SceneCollection* collection)
    {
        m_selectedObject = nullptr;
        if (collection == nullptr) {
            m_selectedCollection = nullptr;
            return; 
        }

        m_selectedCollection = m_selectedCollection == collection ? nullptr : collection;
    }

    void Scene::ToggleSelection(Object* object)
    {
        m_selectedCollection = nullptr;
        if (object == nullptr) {
            m_selectedObject = nullptr;
            return;
        }

        m_selectedObject = m_selectedObject == object ? nullptr : object;
    }

    float Scene::IntersectRay(const Ray& ray, Object** hit_obj, bool uv, glm::vec3& out)
    {
        static constexpr float FLOAT_MAX = std::numeric_limits<float>::max();
        float min_dist = FLOAT_MAX;

        for (auto&& obj : m_objects) {
            for (size_t i = 0; i < obj.tris.size(); ++i) {
                const glm::vec3 v0 = obj.verts[obj.tris[i].x];
                const glm::vec3 v1 = obj.verts[obj.tris[i].y];
                const glm::vec3 v2 = obj.verts[obj.tris[i].z];

                float t, u, v;
                if (yart::Ray::IntersectTriangle(ray, v0, v1, v2, &t, &u, &v) && t < min_dist) {
                    *hit_obj = &obj;
                    min_dist = t;

                    if (uv) {
                        // const float w = 1 - (*u) - (*v);
                        // const glm::u32vec3& uv_indices = obj.triangleUVs[i];
                        // const glm::vec2 tex_uv = w * obj.UVs[uv_indices.x] + (*u) * obj.UVs[uv_indices.y] + (*v) * obj.UVs[uv_indices.z];
                        out.x = u;
                        out.y = v;
                        out.z = 0.0f;
                    } else {
                        // Calculate the surface's normal vector
                        out = glm::normalize(glm::cross(v1 - v0, v2 - v1));
                    }
                }
            }
        }

        return min_dist > FLOAT_MAX - 0.001f ? -1.0f : min_dist;
    }

    Object* Scene::AddMeshObject(const char* name, Mesh* mesh)
    {
        if (m_objects.size() == 100) 
            YART_ABORT("For now, scenes accept for up to 100 objects");

        Object::MeshData mesh_data = { };
        Object object(name, mesh_data);
        
        object.verts = { mesh->vertices, mesh->vertices + mesh->verticesCount };
        object.tris = { mesh->triangleIndices, mesh->triangleIndices + mesh->trianglesCount };
        // object.UVs = { mesh->uvs, mesh->uvs + mesh->uvsCount };
        // object.triangleUVs = { mesh->triangleVerticesUvs, mesh->triangleVerticesUvs + mesh->trianglesCount };

        Object* p_object = &m_objects.emplace_back(object);
        ObjectAssignCollection(p_object);

        return p_object;
    }

    SceneCollection* Scene::ObjectAssignCollection(Object* object, SceneCollection* collection)
    {
        // Remove object from its collection, if it's already assigned
        if (object->m_collection != nullptr)
            CollectionRemoveObject(object);

        if (collection == nullptr)
            collection = m_selectedCollection;

        if (collection == nullptr) {
            if (m_collections.size() == 0)
                m_collections.emplace_back("Collection 1");

            collection = &m_collections[0];
        }

        YART_ASSERT(collection != nullptr);
        collection->objects.push_back(object);
        object->m_collection = collection;

        return collection;
    }

    void Scene::CollectionRemoveObject(Object* object)
    {
        SceneCollection* collection = object->m_collection;
        if (collection == nullptr) return;

        size_t object_index = -1;
        for (size_t i = 0; i < collection->objects.size(); ++i) {
            if (collection->objects[i] == object) {
                object_index = i; 
                break;
            }
        }

        YART_ASSERT(object_index >= 0);
        collection->objects.erase(collection->objects.begin() + object_index);
        object->m_collection = nullptr;
    }

} // namespace yart
