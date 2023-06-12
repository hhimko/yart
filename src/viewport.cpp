#include "viewport.h"

#include "window.h"


namespace yart
{
    Viewport::Viewport(yart::Window* window, uint32_t width, uint32_t height)
        : m_image(window->m_vkDevice, window->m_vkPhysicalDevice, window->m_viewportImageSampler, width, height)
    { 
        m_imageWidth = width; 
        m_imageHeight = height;

        m_imageData = new float[width * height * 4];
    }

    Viewport::~Viewport()
    {
        delete[] m_imageData;
    }

    void Viewport::GetImageSize(uint32_t *width, uint32_t *height)
    {
        if (width != nullptr)
            *width = m_imageWidth;

        if (height != nullptr)
            *height = m_imageHeight;
    }

    void Viewport::Refresh(yart::Window* window)
    {
        VkCommandPool command_pool = window->m_swapchainData.framesInFlight[window->m_swapchainData.current_frame_in_flight].vkCommandPool;
        m_image.BindData(window->m_vkDevice, command_pool, window->m_vkQueue, m_imageData);
    }

    void Viewport::Resize(yart::Window* window, uint32_t width, uint32_t height)
    {
        m_imageWidth = width; 
        m_imageHeight = height;

        delete[] m_imageData;
        m_imageData = new float[width * height * 4];

        m_image.Resize(window->m_vkDevice, window->m_vkPhysicalDevice, window->m_viewportImageSampler, width, height);
    }
} // namespace yart
