#pragma once

#include <stdint.h>

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
        Viewport(yart::Window* window, uint32_t width, uint32_t height);
        Viewport(const Viewport&) = delete;
        Viewport& operator=(Viewport const&) = delete;
        ~Viewport();

        /// @brief Get the current size of the viewport image in pixels
        /// @param width Pointer to a variable to be set with the width of the viewport image or NULL
        /// @param height Pointer to a variable to be set with the height of the viewport image or NULL
        void GetImageSize(uint32_t* width, uint32_t* height);

        /// @brief Apply changes made to the image data and update the underlying viewport image
        void Refresh(yart::Window* window);

        float* GetImageData() const 
        {
            return m_imageData;
        }        

    private:
        void Resize(yart::Window* window, uint32_t width, uint32_t height);

    private:
        yart::Image m_image;
        float* m_imageData = nullptr;

        uint32_t m_imageWidth; 
        uint32_t m_imageHeight;


        // -- FRIEND DECLARATIONS -- //
        friend class yart::Window;

    };
} // namespace yart
