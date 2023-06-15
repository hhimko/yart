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
        Refresh(window);
    }

    Viewport::~Viewport()
    {
        delete[] m_imageData;
    }

    void Viewport::GetImageSize(uint32_t *width, uint32_t *height)
    {
        if (width != nullptr)
            *width = m_imageWidth / m_imageScale;

        if (height != nullptr)
            *height = m_imageHeight / m_imageScale;
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

        uint32_t scaled_width = width / m_imageScale;
        uint32_t scaled_height = height / m_imageScale;

        delete[] m_imageData;
        m_imageData = new float[scaled_width * scaled_height * 4];

        m_image.Resize(window->m_vkDevice, window->m_vkPhysicalDevice, window->m_viewportImageSampler, scaled_width, scaled_height);
        Refresh(window);
    }

    void Viewport::OnImGUI(yart::Window* window)
    {
        ImGui::SeparatorText("Viewport");

        // Range slider for controlling the viewport scale 
        bool scale_changed = ImGui::SliderInt("Scale", &m_imageScale, 1, 10);
        if (scale_changed)
            Resize(window, m_imageWidth, m_imageHeight);

        ImGui::Text("Width: %d", m_imageWidth / m_imageScale);
        ImGui::Text("Height: %d", m_imageHeight / m_imageScale);
    }
} // namespace yart
