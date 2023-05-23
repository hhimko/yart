#include "renderer.h"

#include "utils/core_utils.h"


namespace yart
{
    void Renderer::Render(float buffer[], uint32_t width, uint32_t height)
    {
        YART_ASSERT(buffer != nullptr);

        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = ((y * width) + x) * 4;
                float val = static_cast<float>(x) / (width - 1) - static_cast<float>(y) / (height - 1);

                buffer[idx + 0] = val;
                buffer[idx + 1] = val;
                buffer[idx + 2] = val;
                buffer[idx + 3] = 1.f;
            }
        }
    }
} // namespace yart
