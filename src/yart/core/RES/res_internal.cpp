////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal definitions for the RES module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "res_internal.h"


#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "yart/core/utils/glm_utils.h"


/// @brief Color displayed when trying to sample a missing image
#define MISSING_IMAGE_COLOR { 1.0f, 0.0f, 1.0f }


namespace yart
{
    namespace RES
    {
        ResourcesContext& GetResourcesContext()
        {
            static ResourcesContext s_currentContext;
            return s_currentContext;
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

            size_t x_floor = static_cast<size_t>(x);
            size_t y_floor = static_cast<size_t>(y);

            const float* p0 = image->data + (y_floor * image->width + x_floor) * Image::CHANNELS;
            if (interpolation == InterpolationType::NEAREST) {
                if (glm::fract(y) >= 0.5f && y_floor + 1 < image->height) p0 += image->width * Image::CHANNELS;
                if (glm::fract(x) >= 0.5f) p0 += Image::CHANNELS;
                return { p0[0], p0[1], p0[2] };
            }

            const float* p1 = (y_floor + 1 < image->height) ? p0 + image->width * Image::CHANNELS : p0;
            return yart::utils::InterpolateBilinear<3, float, glm::qualifier::defaultp>(
                { p0[0], p0[1], p0[2] }, { p0[3], p0[4], p0[5] }, 
                { p1[0], p1[1], p1[2] }, { p1[3], p1[4], p1[5] }, 
                glm::fract(x), glm::fract(y)
            );
        }

        glm::vec3 SampleImageNorm(const Image* image, float u, float v, InterpolationType interpolation)
        {
            if (image == nullptr)
                return MISSING_IMAGE_COLOR;

            const float x = u * (image->height - 1);
            const float y = v * (image->height - 1);
            return SampleImage(image, x, y, interpolation);
        }

        void DestroyImage(Image* image)
        {
            STBI_FREE(image);
        }

    } // namespace RES
} // namespace yart
