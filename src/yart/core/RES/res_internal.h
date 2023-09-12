////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Internal definitions for the RES module
/// @details This file should only ever be imported and used from within the RES module
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <glm/glm.hpp>

#include "yart/core/utils/yart_utils.h"
#include "resources/resource.h"


namespace yart
{
    namespace RES
    {
        /// @brief Internal enumerator for uniquely identifying Resource sub-types
        /// @note The underlying type should be compatible with the `ResourceType` typedef from `resources/resource.h`
        enum ResourceType_ : uint8_t {
            ResourceType_CubeMap = 0,
            ResourceType_COUNT
        };
        static_assert(sizeof(ResourceType_) == sizeof(ResourceType));

        /// @brief Internal structure used for holding image-related data
        struct Image {
        public:
            /// @brief Static number of channels in the image (RGB)
            static constexpr uint8_t CHANNELS = 3; 

            /// @brief Image width in pixels
            uint32_t width;
            /// @brief Image height in pixels
            uint32_t height;
            /// @brief Image pixel data in floats
            /// @details Flexible-size, 0-length array
            float data[]; 

        };

        /// @brief Internal `RES` module context object
        struct ResourcesContext {
        public:
            /// @brief Array of pointers to lazily-loaded default resources from each `Resource` child class  
            const Resource* defaultResources[ResourceType_COUNT] = {};

        };


        /// @brief Get the current `RES` module context
        /// @return The current context
        ResourcesContext& GetResourcesContext();

        /// @brief Load a new image from file
        /// @param filename Path to the image file
        /// @return Newly created image or `nullptr` on fail 
        Image* LoadImageFromFile(const char* filename);

        /// @brief Sample a given image at given pixel coordinates
        /// @param image The image to sample from
        /// @param x Sample X coordinate
        /// @param y Sample Y coordinate
        /// @param interpolation Interpolation strategy to use for sampling
        /// @return Color of the image at the sample point
        glm::vec3 SampleImage(const Image* image, float x, float y, InterpolationType interpolation = InterpolationType::NEAREST);

        /// @brief Sample a given image at normalized UV coordinates
        /// @param image The image to sample from
        /// @param u Normalized U coordinate in the [0..1] range
        /// @param v Normalized V coordinate in the [0..1] range
        /// @param interpolation Interpolation strategy to use for sampling
        /// @return Color of the image at the sample point
        glm::vec3 SampleImageNorm(const Image* image, float u, float v, InterpolationType interpolation = InterpolationType::NEAREST);

        /// @brief Destroy a given image object
        /// @param image Result of RES::LoadImageFromFile()
        void DestroyImage(Image* image);

    } // namespace RES
} // namespace yart
