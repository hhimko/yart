////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <glm/glm.hpp>

#include "yart/common/classes/mesh_factory.h"
#include "object.h"
#include "ray.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Scene class, acting as a container and manager for rendered objects 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Scene {
    public:
        /// @brief Construct a new empty scene 
        Scene();

        /// @brief Scene class custom destructor
        ~Scene();

        /// @brief Load the default scene objects
        void LoadDefault();

        /// @brief Test for ray-scene intersections
        /// @param ray Ray to be intersected with the scene 
        /// @param hit_obj Pointer to the nearest hit object, or `nullptr` on miss
        /// @param u Output parameter set on valid intersection with the `u` coordinate in texture space
        /// @param v Output parameter set on valid intersection with the `v` coordinate in texture space
        /// @return Distance to the closest object hit, or a negative value on miss 
        float IntersectRay(const Ray& ray, Object* hit_obj, float* u, float* v);

        /// @brief Add a new mesh type object to the scene 
        /// @param name Name of the object
        /// @param mesh Object's mesh 
        /// @return The newly created object 
        Object* AddMeshObject(const char* name, Mesh* mesh);

    private:
        /// @brief Jagged array of vertices from all objects in the scene 
        glm::f32vec3* m_vertHeap = nullptr;
        static constexpr size_t DEFAULT_VHEAP_CAPACITY = 1024;
        size_t m_vertHeapCapacity = DEFAULT_VHEAP_CAPACITY;

        /// @brief Jagged array of triangle indices from all objects in the scene 
        /// @details Triangles are indexed starting from the object's first vertex 
        glm::u32vec3* m_triHeap = nullptr;
        static constexpr size_t DEFAULT_THEAP_CAPACITY = 1024;
        size_t m_triHeapCapacity = DEFAULT_THEAP_CAPACITY;

        /// @brief List of all objects in the scene, sorted by their ID's in ascending order
        std::vector<Object> m_objects;

    };
} // namespace yart
