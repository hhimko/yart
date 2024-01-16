////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scene.h"


#include <limits>

#include "yart/common/utils/yart_utils.h"


namespace yart
{
    Scene::~Scene()
    {
        m_objects.clear();
    }

    void Scene::LoadDefault()
    {
        // Mesh* cube_mesh = MeshFactory::CubeMesh({ 0, 0, 0 });
        // AddMeshObject("Default Cube", cube_mesh);

        // MeshFactory::DestroyMesh(cube_mesh);

        const float xoff = 0.3f;
        const float zoff = -0.5f;

        Object* object;
        object = AddSdfObject("Sphere", 0.5f);
        object->position = { -0.8f + xoff, 0.5f, -0.2f + zoff };
        object->materialColor = { 0.1f, 0.8f, 0.1f };

        object = AddSdfObject("Sphere", 0.3f);
        object->position = { 0.0f + xoff, 0.3f, -0.35f + zoff };
        object->materialColor = { 0.1f, 0.1f, 0.8f };

        object = AddSdfObject("Sphere", 1.0f);
        object->position = { 0.1f + xoff, 1.0f, 0.8f + zoff };
        object->materialColor = { 1.0f, 0.1f, 0.1f };

        Mesh* plane_mesh = MeshFactory::PlaneMesh({ 0, 0, 0 }, 1000.0f);
        object = AddMeshObject("Ground Plane", plane_mesh);
        object->materialColor = { 0.3f, 0.3f, 0.3f };

        MeshFactory::DestroyMesh(plane_mesh);
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
        static constexpr float infinity = std::numeric_limits<float>::infinity();
        float min_dist = infinity;

        for (auto&& obj : m_objects) {
            switch (obj.m_type) {
            case ObjectType::MESH: {
                glm::mat4 transformation = obj.GetTransformationMatrix();

                for (size_t i = 0; i < obj.tris.size(); ++i) {
                    const glm::vec3 v0 = transformation * glm::vec4(obj.verts[obj.tris[i].x], 1.0f);
                    const glm::vec3 v1 = transformation * glm::vec4(obj.verts[obj.tris[i].y], 1.0f);
                    const glm::vec3 v2 = transformation * glm::vec4(obj.verts[obj.tris[i].z], 1.0f);

                    float t, u, v;
                    if (yart::Ray::IntersectTriangle(ray, v0, v1, v2, &t, &u, &v) && t > 0.0f && t < min_dist) {
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
                break;
            }
            case ObjectType::SDF: {
                const glm::vec3 pos = obj.position;
                const float radius = obj.m_sdfData.radius * obj.scale.x;
                glm::vec3 dir = ray.origin - pos; 
                const float dir_len = glm::length(dir);

                const float a = 1.0f;
                const float half_b = glm::dot(dir, ray.direction);
                const float c = dir_len * dir_len - radius * radius;
                const float discriminant = half_b * half_b - a * c;

                if (discriminant < 0) {
                    break;
                }

                const float dist = -half_b - glm::sqrt(discriminant);
                if (dist > 0.0f && dist < min_dist) {
                    *hit_obj = &obj;
                    min_dist = dist;

                    const glm::vec3 hit_pos = ray.origin + dist * ray.direction;
                    out = glm::normalize(hit_pos - pos);
                }

                break;
            }
            }
        }

        return min_dist == infinity ? -1.0f : min_dist;
    }

    Object* Scene::AddMeshObject(const char* name, Mesh* mesh)
    {
        if (m_objects.size() == 100) 
            YART_ABORT("For now, scenes accept for up to 100 objects");

        // Get unique name
        static int id = 1;
        std::string name_str(name);
        name_str += " ";
        name_str += std::to_string(id++);

        Object::MeshData mesh_data = { };
        Object object(name_str, mesh_data);
        
        object.verts = { mesh->vertices, mesh->vertices + mesh->verticesCount };
        object.tris = { mesh->triangleIndices, mesh->triangleIndices + mesh->trianglesCount };
        // object.UVs = { mesh->uvs, mesh->uvs + mesh->uvsCount };
        // object.triangleUVs = { mesh->triangleVerticesUvs, mesh->triangleVerticesUvs + mesh->trianglesCount };

        Object* p_object = &m_objects.emplace_back(object);
        ObjectAssignCollection(p_object);

        return p_object;
    }

    Object* Scene::AddSdfObject(const char* name, float radius)
    {
        if (m_objects.size() == 100) 
            YART_ABORT("For now, scenes accept for up to 100 objects");

        // Get unique name
        static int id = 1;
        std::string name_str(name);
        name_str += " ";
        name_str += std::to_string(id++);

        Object::SdfData sdf_data = { };
        sdf_data.radius = radius;
        Object object(name_str, sdf_data);
        
        Object* p_object = &m_objects.emplace_back(object);
        ObjectAssignCollection(p_object);

        return p_object;
    }

    void Scene::RemoveObject(Object* object)
    {
        for (auto& it = m_objects.begin(); it != m_objects.end(); ++it) {
            Object* o = &(*it);
            if (o == object) {
                if (o == m_selectedObject)
                    m_selectedObject = nullptr;

                CollectionRemoveObject(object);
                m_objects.erase(it);
                break;
            }
        }
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
