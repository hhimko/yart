////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <vector>

#include <glm/glm.hpp>

#include "yart/common/mesh_factory.h"
#include "object.h"
#include "ray.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Named container for scene objects 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    struct SceneCollection {
    public:
        /// @brief SceneCollection struct custom constructor
        /// @param name Name of the collection
        SceneCollection(const char* name)
            : name(name) { }
    
    public:
        const char* name; ///< Name of the collection
        std::vector<Object*> objects; ///< Objects in the collection

    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Scene class, acting as a container for collections and manager for rendered objects 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Scene {
    public:
        /// @brief Construct a new empty scene 
        Scene() = default;

        /// @brief Scene class custom destructor
        ~Scene();

        /// @brief Load the default scene objects
        void LoadDefault();

        /// @brief Get an array of all object collections in the scene
        /// @param count Output parameter, set to the returned array size
        /// @return Array of scene collections
        SceneCollection* GetSceneCollections(size_t* count)
        {
            *count = m_collections.size();
            return m_collections.data();
        }

        /// @brief Get the currently selected scene collection
        /// @return Selected collection, or `nullptr` if none
        SceneCollection* GetSelectedCollection() const 
        {
            return m_selectedCollection;
        }

        /// @brief Test for ray-scene intersections
        /// @param ray Ray to be intersected with the scene 
        /// @param hit_obj Pointer to the nearest hit object, or `nullptr` on miss
        /// @param u Output parameter set on valid intersection with the `u` coordinate in texture space
        /// @param v Output parameter set on valid intersection with the `v` coordinate in texture space
        /// @return Distance to the closest object hit, or a negative value on miss 
        float IntersectRay(const Ray& ray, Object** hit_obj, float* u, float* v);

        /// @brief Add a new mesh type object to the scene 
        /// @param name Name of the object
        /// @param mesh Object's mesh 
        /// @return The newly created object 
        Object* AddMeshObject(const char* name, Mesh* mesh);

    private:
        /// @brief Assign a specified object to a collection
        /// @param object Object to be added to a collection
        /// @param collection Collection to which the object should be added, or `nullptr` to assign to the selected/default collection 
        /// @return Scene collection, to which the object was assigned
        SceneCollection* ObjectAssignCollection(Object* object, SceneCollection* collection = nullptr);

        /// @brief Remove a specified object from its assigned collection
        /// @param object Object to remove
        void CollectionRemoveObject(Object* object);

    private:
        std::vector<SceneCollection> m_collections; ///< List of object collections in the scene
        std::vector<Object> m_objects; ///< List of all objects in the scene, sorted by their ID's in ascending order
        SceneCollection* m_selectedCollection = nullptr; ///< Currently selected scene collection, or `nullptr` if none  

    };
} // namespace yart
