////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the platform specific Viewport class implemented for Vulkan
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "viewport.h"


#include "window.h"


namespace yart
{
    Viewport::Viewport(uint32_t width, uint32_t height)
        : m_image(yart::Window::Get().m_vkDevice, yart::Window::Get().m_vkPhysicalDevice, yart::Window::Get().m_viewportImageSampler, width, height)
    { 
        m_imageWidth = width; 
        m_imageHeight = height;

        m_imageData = new float[width * height * 4];
        m_shouldRefresh = true;
    }

    Viewport::~Viewport()
    {
        delete[] m_imageData;
    }

    void Viewport::GetImageSize(uint32_t* width, uint32_t* height)
    {
        if (width != nullptr)
            *width = m_imageWidth / m_imageScale;

        if (height != nullptr)
            *height = m_imageHeight / m_imageScale;
    }

    void Viewport::Render(ImDrawList *draw_list)
    {
        if (m_shouldRefresh)
            Refresh();

        ImTextureID viewport_image = (ImTextureID)m_image.GetDescriptorSet();

        const ImVec2 p_min = m_position;
        const ImVec2 p_max = { static_cast<float>(m_imageWidth) + m_position.x, static_cast<float>(m_imageHeight) + m_position.y };

        draw_list->AddImage(viewport_image, p_min, p_max);
    }

    void Viewport::Resize(ImVec2 size, int scale)
    {
        yart::Window& window = yart::Window::Get();

        uint32_t width = static_cast<uint32_t>(size.x);
        uint32_t height = static_cast<uint32_t>(size.y);
        if (!m_shouldResize && width == m_imageWidth && height == m_imageHeight && scale == m_imageScale)
            return;

        m_imageWidth = width; 
        m_imageHeight = height;
        m_imageScale = scale;

        uint32_t scaled_width = width / m_imageScale;
        uint32_t scaled_height = height / m_imageScale;

        delete[] m_imageData;
        m_imageData = new float[scaled_width * scaled_height * 4];

        m_image.Resize(window.m_vkDevice, window.m_vkPhysicalDevice, window.m_viewportImageSampler, scaled_width, scaled_height);
        m_shouldResize = false;
        m_shouldRefresh = true;
    }

    void Viewport::Resize(ImVec2 size)
    {
        return Resize(size, m_imageScale);
    }

    void Viewport::Refresh()
    {
        yart::Window& window = yart::Window::Get();

        VkCommandPool command_pool = window.m_framesInFlight[window.m_currentFrameInFlight].vkCommandPool;
        m_image.BindData(window.m_vkDevice, command_pool, window.m_vkQueue, m_imageData);
        m_shouldRefresh = false;
    }

    void Viewport::OnImGui()
    {
        ImGui::SeparatorText("Viewport");

        // Range slider for controlling the viewport scale 
        int scale = m_imageScale;
        bool scale_changed = ImGui::SliderInt("Scale", &scale, 1, 10);
        if (scale_changed)
            SetScale(scale);

        ImGui::Text("Width: %d", m_imageWidth / m_imageScale);
        ImGui::Text("Height: %d", m_imageHeight / m_imageScale);
    }

} // namespace yart
