#pragma once

#include <vulkan/vulkan.h>


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Convenience class for managing 2D Vulkan images. The image is in RGB format
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Image {
    public:
        /// @brief Allocate a new image and optionally upload and bind initial pixel data 
        /// @param device Vulkan device handle on which to allocate the image 
        /// @param width Initial image width in texels
        /// @param height Initial image height in texels
        /// @param data Optional pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*3), where 3 is the number of channels in the image (RGB)
        Image(VkDevice device, uint32_t width, uint32_t height, const void* data = nullptr);
        ~Image();

        /// @brief Upload and bind pixel data to the image  
        /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*3), where 3 is the number of channels in the image (RGB)
        void BindData(const void* data);

        /// @brief Release and resize the image, with optionally uploading and binding pixel data 
        /// @param width New image width in texels
        /// @param height New image height in texels
        /// @param data Optional pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*3), where 3 is the number of channels in the image (RGB)
        void Resize(uint32_t width, uint32_t height, const void* data = nullptr);

        VkDescriptorSet GetDescriptorSet() const { return m_vkDescriptorSet; }

    private:

        void CreateDescriptorSet(uint32_t width, uint32_t height);

        /// @brief Free all allocations managed by this object. This method should only be called when the Vulkan device is idle
        /// @warning All Image instances must be released before VKDevice is destroyed
        void Release();

    private:
        VkDevice m_vkDevice = VK_NULL_HANDLE; // Externally managed
        VkDescriptorSet m_vkDescriptorSet = VK_NULL_HANDLE;
        
    };
} // namespace yart
