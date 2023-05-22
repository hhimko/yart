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
        /// @param physical_device Vulkan physical device on which to query memory types
        /// @param sampler Initial Vulkan sampler used to interpolate the image
        /// @param width Initial image width in texels
        /// @param height Initial image height in texels
        /// @param data Optional pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*3), where 3 is the number of channels in the image (RGB)
        Image(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, uint32_t width, uint32_t height, const void* data = nullptr);
        ~Image();

        /// @brief Upload and bind pixel data to the image  
        /// @param device Vulkan device handle on which the image was allocated 
        /// @param physical_device Vulkan physical device on which to query memory types
        /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*3), where 3 is the number of channels in the image (RGB)
        void BindData(VkDevice device, VkPhysicalDevice physical_device, const void* data);

        /// @brief Rebuild the image, with optionally uploading and binding pixel data. This method calls Release, which ultimately will block the CPU until the Vulkan device is idle 
        /// @param device Vulkan device handle on which the image was allocated 
        /// @param physical_device Vulkan physical device on which to query memory types
        /// @param sampler Vulkan sampler used to interpolate the image
        /// @param width New image width in texels
        /// @param height New image height in texels
        /// @param data Optional pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*3), where 3 is the number of channels in the image (RGB)
        void Resize(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, uint32_t width, uint32_t height, const void* data = nullptr);

        /// @brief Free all allocations managed by this object. This method will block the CPU until the Vulkan device is idle 
        /// @param device Vulkan device handle on which the image was allocated 
        /// @warning All Image instances must be released before VKDevice is destroyed
        void Release(VkDevice device);

        VkDescriptorSet GetDescriptorSet() const { return m_vkDescriptorSet; }

    private:
        VkDeviceSize GetMemorySize() const 
        { 
            static_assert(m_vkFormat == VK_FORMAT_R32G32B32_SFLOAT, "m_memorySize evaluation depends on the image format used");
            return m_imageExtent.width * m_imageExtent.height * 3;
        }

        // -- DESCRIPTOR SET CREATION -- //
        void CreateDescriptorSet(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, VkExtent3D image_extent);
        static VkImage CreateVulkanImage(VkDevice device, VkExtent3D image_extent);
        static VkImageView CreateVulkanImageView(VkDevice device, VkImage image);
        static VkDeviceMemory BindVulkanImageDeviceMemory(VkDevice device, VkPhysicalDevice physical_device, VkImage image, VkDeviceSize memory_size);
        static VkBuffer CreateVulkanStagingBuffer(VkDevice device, VkDeviceSize buffer_size);
        static VkDeviceMemory BindVulkanStagingBufferMemory(VkDevice device, VkPhysicalDevice physical_device, VkBuffer staging_buffer, VkDeviceSize memory_size);

        // -- GPU MEMORY BINDING -- //
        static bool UploadDataToStagingBuffer(VkDevice device, VkDeviceMemory staging_buffer_memory, const void* data, VkDeviceSize data_size);

    private:
        static const VkFormat m_vkFormat = VK_FORMAT_R32G32B32_SFLOAT;

        VkExtent3D m_imageExtent = {};
        VkDescriptorSet m_vkDescriptorSet = VK_NULL_HANDLE;

        VkImage m_vkImage = VK_NULL_HANDLE;
        VkImageView m_vkImageView = VK_NULL_HANDLE;
        VkDeviceMemory m_vkMemory = VK_NULL_HANDLE;
        VkBuffer m_vkStagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_vkStagingBufferMemory = VK_NULL_HANDLE;
        
    };
} // namespace yart
