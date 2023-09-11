////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal definitions for the RES module
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "res_internal.h"


#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


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

        Image* LoadImageFromFile(const char* filename, uint8_t padding)
        {
            int w, h, comp;
            if (!stbi_info(filename, &w, &h, &comp) || w <= 0 || h <= 0) {
                // stbi_failure_reason()
                return nullptr;
            }

            static constexpr int desired_comp = Image::CHANNELS; // Desired number of channels in the image (RGB)
            const size_t data_size_with_padding = w * h * desired_comp + (padding * padding * 4 + padding * 2 * (w + h)) * desired_comp;

            Image* image = static_cast<Image*>(STBI_MALLOC(sizeof(Image) + data_size_with_padding * sizeof(float)));
                if (image == nullptr)
                    return nullptr;

            if (padding == 0) {
                float* data = stbi_loadf(filename, &w, &h, &comp, desired_comp);
                YART_ASSERT(comp == desired_comp);
                if (data == nullptr) {
                    // stbi_failure_reason()
                    DestroyImage(image);
                    return nullptr;
                }

                memcpy(image->data, data, data_size_with_padding * sizeof(float));
                stbi_image_free(data);
            } else {
                float* data = stbi_loadf(filename, &w, &h, &comp, desired_comp);
                YART_ASSERT(comp == desired_comp);
                if (data == nullptr) {
                    // stbi_failure_reason()
                    DestroyImage(image);
                    return nullptr;
                }

                const size_t data_size = w * h * desired_comp; // Size of the data returned by STBI 
                memcpy(image->data, data, data_size * sizeof(float));
                
                stbi_image_free(data);
            }

            image->width = static_cast<uint32_t>(w);
            image->height = static_cast<uint32_t>(h);
            image->padding = padding;

            return image;
        }

        glm::vec3 SampleImage(const Image* image, float x, float y)
        {
            if (image == nullptr)
                return MISSING_IMAGE_COLOR;

            size_t xc = static_cast<size_t>(x);
            size_t yc = static_cast<size_t>(y);

            const float* val = image->data + (yc * image->width + xc) * Image::CHANNELS;
            return { val[0], val[1], val[2] };
        }

        glm::vec3 SampleImageNorm(const Image* image, float u, float v)
        {
            if (image == nullptr)
                return MISSING_IMAGE_COLOR;

            const uint32_t x = static_cast<uint32_t>(glm::round(u * (image->height - 1)));
            const uint32_t y = static_cast<uint32_t>(glm::round(v * (image->height - 1)));
            return SampleImage(image, x, y);
        }

        void DestroyImage(Image* image)
        {
            STBI_FREE(image);
        }

    } // namespace RES
} // namespace yart
