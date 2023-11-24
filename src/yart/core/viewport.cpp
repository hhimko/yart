////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the Viewport class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "viewport.h"


#include <glm/glm.hpp>

#include "yart/common/utils/yart_utils.h"


namespace yart
{
    Viewport::Viewport(uint32_t width, uint32_t height)
        : m_width(width), m_height(height)
    {
        m_image = yart::Backend::CreateImage(width, height, IMAGE_FORMAT, DEFAULT_IMAGE_SAMPLER);
        YART_ASSERT(m_image != nullptr);

        m_imageData = new float[width * height * m_image->GetFormatChannelsCount()];
    }

    Viewport::Viewport(uint32_t width, uint32_t height, uint8_t scale)
        : m_width(width), m_height(height), m_imageScale(scale)
    {
        uint32_t scaled_width = glm::max(m_width / m_imageScale, 1U);
        uint32_t scaled_height = glm::max(m_height / m_imageScale, 1U);

        m_image = yart::Backend::CreateImage(scaled_width, scaled_height, IMAGE_FORMAT, DEFAULT_IMAGE_SAMPLER);
        YART_ASSERT(m_image != nullptr);

        m_imageData = new float[scaled_width * scaled_height * m_image->GetFormatChannelsCount()];
    }

    Viewport::~Viewport()
    {
        delete[] m_imageData;
        Backend::DestroyImage(m_image);
    }
    
    void Viewport::Resize(uint32_t width, uint32_t height)
    {
        m_width = width; 
        m_height = height;

        uint32_t scaled_width = glm::max(width / m_imageScale, 1U);
        uint32_t scaled_height = glm::max(height / m_imageScale, 1U);
        ImVec2 image_size = GetImageSize();
        if (image_size.x == scaled_width && image_size.y == scaled_height)
            return; // When the underlying image is down scaled, scaling the viewport does not necessarily mean the image has to be recreated

        delete[] m_imageData;
        m_imageData = new float[scaled_width * scaled_height * m_image->GetFormatChannelsCount()];

        m_image->Resize(scaled_width, scaled_height);
        m_needsRefresh = true;
    }

    void Viewport::SetImageScale(uint8_t scale)
    {
        YART_ASSERT(scale > 0);
        if (scale == m_imageScale)
            return;

        m_imageScale = scale;
    }

    void Viewport::Refresh()
    {
        m_image->BindData(m_imageData);
        m_needsRefresh = false;
    }

} // namespace yart
