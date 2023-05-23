#pragma once

#include <cstdint>


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief YART offline ray tracing renderer
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Renderer {
    public:
        Renderer() = default;
        ~Renderer() = default;

        /// @brief Render scene to a given buffer
        /// @param buffer Pointer to a pixel array to be rendered onto. 
        ///     The size of the array should be equal to width*height*3, where 3 denotes the number of channels in the output image (RGB)
        /// @param width Width in pixels of the output image
        /// @param height Height in pixels of the output image
        void Render(double buffer[], uint32_t width, uint32_t height);

    private:
    private:

    };
} // namespace yart
