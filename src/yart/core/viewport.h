////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Viewport class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <stdint.h>

#include <imgui.h>

#include "yart/backend/backend.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper around a backend image for a writeable, canvas-like texture 
    /// @note Currently, the Viewport only supports images in the `R32G32B32A32_FLOAT` format
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Viewport {
    public:
        /// @brief Create a new viewport of a given size
        /// @param width Initial width of the viewport in pixels
        /// @param height Initial height of the viewport in pixels
        Viewport(uint32_t width, uint32_t height);

        /// @brief Create a new viewport of a given size and scale
        /// @param width Initial width of the viewport in pixels
        /// @param height Initial height of the viewport in pixels
        /// @param scale Initial scale down factor of the underlying image
        Viewport(uint32_t width, uint32_t height, uint8_t scale);

        /// @brief Viewport class destructor
        ~Viewport();

        /// @brief Resize the viewport
        /// @param width New width of the viewport in pixels
        /// @param height New height of the viewport in pixels
        void Resize(uint32_t width, uint32_t height);

        /// @brief Get the current size of the viewport in pixels
        /// @return The unscaled viewport size used for rendering to the display
        ImVec2 GetViewportSize() const 
        {
            return { static_cast<float>(m_width), static_cast<float>(m_height) };
        }

        /// @brief Get the current scale down factor of the underlying image
        /// @return Viewport image scale down factor
        uint8_t GetImageScale() const 
        {
            return m_imageScale;
        }

        /// @brief Set the scale down factor of the underlying image
        /// @param scale New viewport scale-down factor
        void SetImageScale(uint8_t scale);

        /// @brief Get the current size of the underlying viewport image in pixels
        /// @return The current size of the scaled viewport image
        ImVec2 GetImageSize() const
        {
            return m_image->GetSize();
        }

        /// @brief Get the sampler type currently used by the viewport's image
        /// @return Current image sampler type
        Backend::ImageSampler GetImageSampler() const
        {
            return m_image->GetSampler();
        }

        /// @brief Set the viewport's image sampler type used for interpolation
        /// @param sampler New sampler type
        void SetImageSampler(Backend::ImageSampler sampler) 
        {
            m_image->SetSampler(sampler);
            m_needsRefresh = true;
        }

        /// @brief Get the viewport's image pixel array
        /// @details The size of the array is equal to `width * height * channels` where `width` and `height` can be 
        ///     retrieved by Viewport::GetViewportImageSize(), and `channels` is the number of channels in the image format used
        /// @return Pointer to the viewport's image pixel data
        float* GetImageData() const 
        {
            return m_imageData;
        }

        /// @brief Get the viewport's image Dear ImGui texture ID
        /// @param refresh Optional parameter, specifying whether the viewport should be forced to refresh this frame
        /// @return Dear ImGui's `ImTextureID`, used for rendering GPU textures onto the screen
        ImTextureID GetImTextureID(bool refresh = false)
        {
            if (refresh || m_needsRefresh)
                Refresh();

            return m_image->GetImTextureID();
        }

    private:
        Viewport(const Viewport&) = delete;
        Viewport& operator=(Viewport const&) = delete;

        /// @brief Apply changes made to the image data and update the underlying viewport image
        void Refresh();

    private:
        static constexpr yart::Backend::ImageFormat IMAGE_FORMAT = Backend::ImageFormat::R32G32B32A32_FLOAT;
        static constexpr yart::Backend::ImageSampler DEFAULT_IMAGE_SAMPLER = Backend::ImageSampler::NEAREST;
        yart::Backend::Image* m_image = nullptr;

        uint32_t m_width;  // Width of the viewport in pixels (does not take image scale into account)
        uint32_t m_height; // Height of the viewport in pixels (does not take image scale into account)
        uint8_t m_imageScale = 1; // Should only ever be in the [1, +inf) range
        bool m_needsRefresh = false;
        float* m_imageData = nullptr;

    };
    
} // namespace yart
