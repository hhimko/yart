#pragma once


#include <vulkan/vulkan.h>


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Convenience class for managing 2D Vulkan images. The image is in R32G32B32A32 float format
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Image {
    public:
        /// @brief Allocate a new image without uploading and binding pixel data 
        /// @param device Vulkan device handle on which to allocate the image 
        /// @param physical_device Vulkan physical device on which to query memory types
        /// @param sampler Initial Vulkan sampler used to interpolate the image
        /// @param width Initial image width in texels
        /// @param height Initial image height in texels
        Image(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, uint32_t width, uint32_t height);

        /// @brief Allocate a new image and upload and bind initial pixel data 
        /// @param device Vulkan device handle on which to allocate the image 
        /// @param physical_device Vulkan physical device on which to query memory types
        /// @param sampler Initial Vulkan sampler used to interpolate the image
        /// @param command_pool Vulkan command pool from which to allocate internal transfer command buffers
        /// @param queue Vulkan queue used internally to submit transfer command buffers on 
        /// @param width Initial image width in texels
        /// @param height Initial image height in texels
        /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*4), where 4 is the number of channels in the image (RGBA)
        Image(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, VkCommandPool command_pool, VkQueue queue, uint32_t width, uint32_t height, const void* data);

        ~Image();

        /// @brief Upload and bind pixel data to the image  
        /// @param device Vulkan device handle on which the image was allocated 
        /// @param command_pool Vulkan command pool from which to allocate internal transfer command buffers
        /// @param queue Vulkan queue used internally to submit transfer command buffers on 
        /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*4), where 4 is the number of channels in the image (RGBA)
        void BindData(VkDevice device, VkCommandPool command_pool, VkQueue queue, const void* data);

        /// @brief Rebuild the image, without uploading and binding pixel data. This method calls Release, which ultimately will block the CPU until the Vulkan device is idle 
        /// @param device Vulkan device handle on which the image was allocated 
        /// @param physical_device Vulkan physical device on which to query memory types
        /// @param sampler Vulkan sampler used to interpolate the image
        /// @param width New image width in texels
        /// @param height New image height in texels
        void Resize(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, uint32_t width, uint32_t height);

        /// @brief Rebuild the image, with uploading and binding pixel data. This method calls Release, which ultimately will block the CPU until the Vulkan device is idle 
        /// @param device Vulkan device handle on which the image was allocated 
        /// @param physical_device Vulkan physical device on which to query memory types
        /// @param sampler Vulkan sampler used to interpolate the image
        /// @param command_pool Vulkan command pool from which to allocate internal transfer command buffers
        /// @param queue Vulkan queue used internally to submit transfer command buffers on 
        /// @param width New image width in texels
        /// @param height New image height in texels
        /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to (width*height*4), where 4 is the number of channels in the image (RGBA)
        void Resize(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, VkCommandPool command_pool, VkQueue queue, uint32_t width, uint32_t height, const void* data);

        /// @brief Free all allocations managed by this object. This method will block the CPU until the Vulkan device is idle 
        /// @param device Vulkan device handle on which the image was allocated 
        /// @warning All Image instances must be released before VKDevice is destroyed
        void Release(VkDevice device);

        /// @brief Getter to the Vulkan descriptor set handle for the image
        /// @return Vulkan descriptor set handle
        VkDescriptorSet GetDescriptorSet() const { return m_vkDescriptorSet; }

    private:
        VkDeviceSize GetMemorySize() const 
        { 
            static_assert(m_vkFormat == VK_FORMAT_R32G32B32A32_SFLOAT, "m_memorySize evaluation depends on the image format used");
            return m_imageExtent.width * m_imageExtent.height * 16;
        }

        // -- DESCRIPTOR SET CREATION -- //
        void CreateDescriptorSet(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler);
        static VkImage CreateVulkanImage(VkDevice device, VkExtent3D image_extent);
        static VkImageView CreateVulkanImageView(VkDevice device, VkImage image);
        static VkDeviceMemory BindVulkanImageDeviceMemory(VkDevice device, VkPhysicalDevice physical_device, VkImage image);
        static VkBuffer CreateVulkanStagingBuffer(VkDevice device, VkDeviceSize buffer_size);
        static VkDeviceMemory BindVulkanStagingBufferMemory(VkDevice device, VkPhysicalDevice physical_device, VkBuffer staging_buffer, VkDeviceSize memory_size);

        // -- GPU MEMORY BINDING -- //
        static bool UploadDataToStagingBuffer(VkDevice device, VkDeviceMemory staging_buffer_memory, const void* data, VkDeviceSize data_size);
        static bool CopyStagingBufferToImage(VkDevice device, VkCommandPool command_pool, VkQueue queue, VkBuffer staging_buffer, VkImage image, VkExtent3D image_extent);

    private:
        static const VkFormat m_vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

        VkExtent3D m_imageExtent = {};
        VkDescriptorSet m_vkDescriptorSet = VK_NULL_HANDLE;

        VkImage m_vkImage = VK_NULL_HANDLE;
        VkImageView m_vkImageView = VK_NULL_HANDLE;
        VkDeviceMemory m_vkMemory = VK_NULL_HANDLE;
        VkBuffer m_vkStagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_vkStagingBufferMemory = VK_NULL_HANDLE;
        
    };
} // namespace yart