////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scene.h"


#include "yart/common/utils/yart_utils.h"


namespace yart
{
    Scene::~Scene()
    {

    }

    void Scene::LoadDefault()
    {
        Mesh* cube_mesh = MeshFactory::CubeMesh({ 0, 0, 0 });
        AddMeshObject("Default Cube", cube_mesh);

        delete[] cube_mesh->vertices;
        delete[] cube_mesh->triangleIndices;
        delete[] cube_mesh->uvs;
        delete[] cube_mesh->triangleVerticesUvs;
        delete cube_mesh;
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

    float Scene::IntersectRay(const Ray &ray, Object **hit_obj, float *u, float *v)
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
                    
                    *hit_obj = &obj;
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
