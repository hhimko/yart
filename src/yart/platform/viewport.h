////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of the platform specific Viewport class implemented for Vulkan
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <stdint.h>

#include <imgui.h>

#include "image.h"


namespace yart
{
    // Forward declaration for friend functions
    class Window;
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Window viewport for rendering images onto
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Viewport {
    public:
        /// @brief Create a new viewport from a given window
        /// @param width Width of the viewport image in pixels
        /// @param height Height of the viewport image in pixels
        Viewport(uint32_t width, uint32_t height);
        Viewport(const Viewport&) = delete;
        Viewport& operator=(Viewport const&) = delete;
        ~Viewport();

        /// @brief Get the current size of the viewport image in pixels
        /// @param width Pointer to a variable to be set with the width of the viewport image or NULL
        /// @param height Pointer to a variable to be set with the height of the viewport image or NULL
        void GetImageSize(uint32_t* width, uint32_t* height);

        /// @brief Resize the viewport
        /// @param size New viewport size in pixels
        /// @param scale New viewport scaling factor
        void Resize(ImVec2 size, int scale);

        /// @brief Resize the viewport and keep the current scaling factor
        /// @param size New viewport size in pixels
        void Resize(ImVec2 size);

        /// @brief Render the viewport image onto a Dear ImGui draw list
        /// @param draw_list Dear ImGui draw list to render to 
        void Render(ImDrawList* draw_list);

        /// @brief Set the viewport render position
        /// @param pos New position in pixel coordinates
        void SetPosition(ImVec2 pos) {
            m_position = pos;
        }

        /// @brief Set the viewport's image scale factor
        /// @param scale New image scale factor
        void SetScale(int scale) {
            m_imageScale = scale;
            m_shouldResize = true;
        }

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
        /// @brief Apply changes made to the image data and update the underlying viewport image
        void Refresh();

        void OnImGUI();

    private:
        yart::Image m_image;
        float* m_imageData = nullptr;

        ImVec2 m_position = { 0.0f, 0.0f }; // Render position in screen pixel coordinates
        uint32_t m_imageWidth;  // Original width of the image (does not take image scale into account)
        uint32_t m_imageHeight; // Original height of the image (does not take image scale into account)
        int m_imageScale = 1; // Should only ever be in the [1, +inf) range
        bool m_shouldResize = false;
        bool m_shouldRefresh = true;


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Window;

    };
    
} // namespace yart
