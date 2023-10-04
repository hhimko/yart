////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <glm/glm.hpp>

#include "object.h"
#include "ray.h"


namespace yart
{
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
        /// @return Distance to the closest object hit, or a negative value on miss 
        float IntersectRay(const Ray& ray, Object* hit_obj);

        /// @brief Add a new mesh type object to the scene 
        /// @param name Name of the object
        /// @param verts Object's mesh vertices
        /// @param tris Object's mesh triangle indices 
        /// @return The newly created object 
        Object* AddMeshObject(const char* name, std::vector<glm::vec3> verts, std::vector<glm::u32vec3> tris);

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
