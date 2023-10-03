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
} // namespace yart