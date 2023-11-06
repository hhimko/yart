////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal definitions for the RES module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "res_internal.h"

#ifndef DOXYGEN_EXCLUDE // Exclude from documentation
    #define STBI_FAILURE_USERMSG
    #define STB_IMAGE_IMPLEMENTATION
#endif // #ifndef DOXYGEN_EXCLUDE
#include <stb_image.h>

#include "yart/common/utils/glm_utils.h"


/// @brief Color displayed when trying to sample a missing image
#define MISSING_IMAGE_COLOR { 1.0f, 0.0f, 1.0f }


namespace yart
{
    namespace RES
    {
        ResContext* GetResContext()
        {
            static ResContext s_currentContext;
            return &s_currentContext;
        }

        Image* LoadImageFromFile(const char* filename)
        {
            int w, h, comp;
            if (!stbi_info(filename, &w, &h, &comp) || w <= 0 || h <= 0) {
                // stbi_failure_reason()
                return nullptr;
            }

            static constexpr int desired_comp = Image::CHANNELS; // Desired number of channels in the image (RGB)
            const size_t data_size = w * h * desired_comp;

            Image* image = static_cast<Image*>(STBI_MALLOC(sizeof(Image) + data_size * sizeof(float)));
            if (image == nullptr)
                return nullptr;

            float* data = stbi_loadf(filename, &w, &h, &comp, desired_comp);
            YART_ASSERT(comp == desired_comp);
            if (data == nullptr) {
                // stbi_failure_reason()
                DestroyImage(image);
                return nullptr;
            }

            memcpy(image->data, data, data_size * sizeof(float));
            stbi_image_free(data);
            

            image->width = static_cast<uint32_t>(w);
            image->height = static_cast<uint32_t>(h);

            return image;
        }

        glm::vec3 SampleImage(const Image* image, float x, float y, InterpolationType interpolation)
        {
            if (image == nullptr)
                return MISSING_IMAGE_COLOR;

            uint32_t x_floor = static_cast<uint32_t>(x);
            uint32_t y_floor = static_cast<uint32_t>(y);

            switch (interpolation) {
            case InterpolationType::NEAREST: {
                const float* p = image->data + (y_floor * image->width + x_floor) * Image::CHANNELS;

                if (glm::fract(y) >= 0.5f && y_floor + 1 < image->height) p += image->width * Image::CHANNELS;
                if (glm::fract(x) >= 0.5f) p += Image::CHANNELS;
                return { p[0], p[1], p[2] };
            }
            case InterpolationType::BILINEAR: {
                const float* p0 = image->data + (y_floor * image->width + x_floor) * Image::CHANNELS;
                const float* p1 = (y_floor + 1 < image->height) ? p0 + image->width * Image::CHANNELS : p0;

                const glm::vec3 values[4] = {
                    { p0[0], p0[1], p0[2] }, { p0[3], p0[4], p0[5] }, 
                    { p1[0], p1[1], p1[2] }, { p1[3], p1[4], p1[5] }
                };

                return yart::utils::InterpolateBilinear(values, glm::fract(x), glm::fract(y));
            }
            case InterpolationType::BICUBIC: {
                float values[4 * 4 * image->CHANNELS];
                GetImageKernelWindow(image, values, x_floor, y_floor, 4);

                static_assert(sizeof(glm::vec3) == 3 * sizeof(float));
                return yart::utils::InterpolateBicubic((glm::vec3*)values, glm::fract(x), glm::fract(y));
            }
            }

            YART_UNREACHABLE();
            return {};
        }

        glm::vec3 SampleImageNorm(const Image* image, float u, float v, InterpolationType interpolation)
        {
            if (image == nullptr)
                return MISSING_IMAGE_COLOR;

            const float x = u * (image->height - 1);
            const float y = v * (image->height - 1);
            return SampleImage(image, x, y, interpolation);
        }

        void GetImageKernelWindow(const Image* image, float* buffer, int x, int y, int kernel_size)
        {
            const int f = (kernel_size - 1) / 2;
            const int c = f + (kernel_size - 1) % 2;

            YART_ASSERT(kernel_size < 4);
            uint32_t x_LUT[4];
            uint32_t y_LUT[4];

            for (int i = 0, dx = -f; dx <= c; ++dx, ++i)
                x_LUT[i] = static_cast<uint32_t>(glm::clamp(x + dx, 0, (int)image->width - 1));

            for (int i = 0, dy = -f; dy <= c; ++dy, ++i)
                y_LUT[i] = static_cast<uint32_t>(glm::clamp(y + dy, 0, (int)image->height - 1));

            for (int yi = 0; yi < kernel_size; ++yi) {
                for (int xi = 0; xi < kernel_size; ++xi) {
                    float* bp = &buffer[(yi * kernel_size + xi) * image->CHANNELS];
                    const float* vp = &image->data[(y_LUT[yi] * image->width + x_LUT[xi]) * image->CHANNELS];
                    bp[0] = vp[0];
                    bp[1] = vp[1];
                    bp[2] = vp[2];
                }
            }
        }

        void DestroyImage(Image* image)
        {
            STBI_FREE(image);
        }

    } // namespace RES
} // namespace yart
