////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the Viewport class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <stdint.h>

#include <imgui.h>


#if 0
namespace yart
{
    // Forward declaration for friend functions
    class Window;
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper around a GPU texture for a writeable canvas-like structure 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Viewport {
    public:
        /// @brief Create a new viewport of a given size
        /// @param width Width of the viewport image in display-space pixels
        /// @param height Height of the viewport image in display-space pixels
        Viewport(uint32_t width, uint32_t height);

        /// @brief Create a new viewport of a given size
        /// @param width Width of the viewport image in display-space pixels
        /// @param height Height of the viewport image in display-space pixels
        /// @param scale Scale down factor of the underlying image
        Viewport(uint32_t width, uint32_t height, uint8_t scale);

        ~Viewport();

        /// @brief Get the current size of the viewport image in display-space pixels
        /// @return The current size of the unscaled viewport image
        ImVec2 GetImageSizeUnscaled();

        /// @brief Get the current size of the underlying viewport image pixels
        /// @return The current size of the scaled viewport image
        ImVec2 GetImageSizeScaled();

        /// @brief Set the scale down factor of the underlying image
        /// @param scale New viewport scaling factor
        void SetImageScale(int scale);

        /// @brief Resize the viewport and keep the current scaling factor
        /// @param size New viewport size in pixels
        void Resize(ImVec2 size);

        /// @brief Get the viewport's image Dear ImGui's texture ID
        /// @param draw_list Dear ImGui draw list to render to 
        ImTextureID GetImTextureID();

        /// @brief Set an internal flag to make sure the viewport image gets refreshed before the next render command
        /// @details Should be used when the image pixels have changed since the last Viewport::Render(), but the viewport size has not 
        void EnsureRefresh() {
            m_shouldRefresh = true;
        }

        /// @brief Get the viewport's image pixel array
        /// @details The size of the array is equal to `width*height*4` where `width` and `height` can be 
        ///     retrieved from Viewport::GetImageSize(), and `4` is the number of channels in the image (RGBA)
        /// @return Pointer to the viewport's image pixel data
        float* GetImageData() const 
        {
            return m_imageData;
        }        

    private:
        Viewport(const Viewport&) = delete;
        Viewport& operator=(Viewport const&) = delete;

        /// @brief Apply changes made to the image data and update the underlying viewport image
        void Refresh();

    private:
        // yart::Image m_image;
        float* m_imageData = nullptr;

        uint32_t m_imageWidth;  // Original width of the image (does not take image scale into account)
        uint32_t m_imageHeight; // Original height of the image (does not take image scale into account)
        uint8_t m_imageScale = 1; // Should only ever be in the [1, +inf) range
        bool m_shouldRefresh = true;


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Window;

    };
    
} // namespace yart
#endif
