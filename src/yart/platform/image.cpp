#include "image.h"


#include <iostream>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h> // Renderer backend

#include "yart/utils/yart_utils.h"
#include "utils/vk_utils.h"


// Helper macro to help locate all VkAllocationCallbacks dependencies
#define DEFAULT_VK_ALLOC VK_NULL_HANDLE


namespace yart
{
    Image::Image(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, uint32_t width, uint32_t height)
    {
        m_imageExtent.width = width;
        m_imageExtent.height = height;
        m_imageExtent.depth = 1;

        CreateDescriptorSet(device, physical_device, sampler);
    }

    Image::Image(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, VkCommandPool command_pool, VkQueue queue, uint32_t width, uint32_t height, const void *data)
        : Image(device, physical_device, sampler, width, height)
    {
        BindData(device, command_pool, queue, data);
    }

    Image::~Image()
    {
        if (m_vkImage != VK_NULL_HANDLE)
            std::cerr << "[Image]: object destructor was called without freeing members with Image::Release" << std::endl;
    }

    void Image::BindData(VkDevice device, VkCommandPool command_pool, VkQueue queue, const void *data)
    {
        VkDeviceSize memory_size = GetMemorySize();

        if (!UploadDataToStagingBuffer(device, m_vkStagingBufferMemory, data, memory_size))
            YART_ABORT("Failed to bind image pixel data");

        if (!CopyStagingBufferToImage(device, command_pool, queue, m_vkStagingBuffer, m_vkImage, m_imageExtent))
            YART_ABORT("Failed to bind image pixel data");
    }

    void Image::Resize(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, uint32_t width, uint32_t height)
    {
        Release(device);

        m_imageExtent.width = width;
        m_imageExtent.height = height;

        CreateDescriptorSet(device, physical_device, sampler);
    }

    void Image::Resize(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler, VkCommandPool command_pool, VkQueue queue, uint32_t width, uint32_t height, const void *data)
    {
        Release(device);

        m_imageExtent.width = width;
        m_imageExtent.height = height;

        CreateDescriptorSet(device, physical_device, sampler);
        BindData(device, command_pool, queue, data);
    }

    void Image::Release(VkDevice device)
    {
        // Release stalls CPU execution until the device is idle
        VkResult res = vkDeviceWaitIdle(device);
        CHECK_VK_RESULT_ABORT(res);

        vkDestroyImageView(device, m_vkImageView, DEFAULT_VK_ALLOC);
        m_vkImageView = VK_NULL_HANDLE;

        vkDestroyImage(device, m_vkImage, DEFAULT_VK_ALLOC);
        m_vkImage = VK_NULL_HANDLE;

        vkFreeMemory(device, m_vkMemory, DEFAULT_VK_ALLOC);
        m_vkMemory = VK_NULL_HANDLE;

        vkDestroyBuffer(device, m_vkStagingBuffer, DEFAULT_VK_ALLOC);
        m_vkStagingBuffer = VK_NULL_HANDLE;

        vkFreeMemory(device, m_vkStagingBufferMemory, DEFAULT_VK_ALLOC);
        m_vkStagingBufferMemory = VK_NULL_HANDLE;
    }

    void Image::CreateDescriptorSet(VkDevice device, VkPhysicalDevice physical_device, VkSampler sampler)
    {
        VkDeviceSize memory_size = GetMemorySize();

        m_vkImage = CreateVulkanImage(device, m_imageExtent);
        if (m_vkImage == VK_NULL_HANDLE)
            YART_ABORT("Failed to create Vulkan image");

        m_vkMemory = BindVulkanImageDeviceMemory(device, physical_device, m_vkImage);
        if (m_vkMemory == VK_NULL_HANDLE)
            YART_ABORT("Failed to create Vulkan device memory");

        m_vkImageView = CreateVulkanImageView(device, m_vkImage);
        if (m_vkImageView == VK_NULL_HANDLE)
            YART_ABORT("Failed to create Vulkan image view");

        m_vkStagingBuffer = CreateVulkanStagingBuffer(device, memory_size);
        if (m_vkStagingBuffer == VK_NULL_HANDLE)
            YART_ABORT("Failed to create Vulkan staging buffer");

        m_vkStagingBufferMemory = BindVulkanStagingBufferMemory(device, physical_device, m_vkStagingBuffer, memory_size);
        if (m_vkStagingBufferMemory == VK_NULL_HANDLE)
            YART_ABORT("Failed to create Vulkan staging buffer memory");

        // Descriptor set is the textureID passed into ImGUI commands 
        m_vkDescriptorSet = ImGui_ImplVulkan_AddTexture(sampler, m_vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        YART_ASSERT(m_vkDescriptorSet != VK_NULL_HANDLE);
    }

    VkImage Image::CreateVulkanImage(VkDevice device, VkExtent3D image_extent)
    {
        VkImage image = VK_NULL_HANDLE;

        VkImageCreateInfo image_ci = {};
        image_ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_ci.imageType = VK_IMAGE_TYPE_2D;
        image_ci.format = m_vkFormat;
        image_ci.extent = image_extent;
        image_ci.arrayLayers = 1;
        image_ci.mipLevels = 1;
        image_ci.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        image_ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_ci.samples = VK_SAMPLE_COUNT_1_BIT;
        image_ci.tiling = VK_IMAGE_TILING_OPTIMAL;

        VkResult res = vkCreateImage(device, &image_ci, DEFAULT_VK_ALLOC, &image);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return image;
    }

    VkImageView Image::CreateVulkanImageView(VkDevice device, VkImage image)
    {
        VkImageView image_view = VK_NULL_HANDLE;

        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = m_vkFormat;
        info.image = image;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.layerCount = 1;

        VkResult res = vkCreateImageView(device, &info, nullptr, &image_view);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return image_view;
    }

    VkDeviceMemory Image::BindVulkanImageDeviceMemory(VkDevice device, VkPhysicalDevice physical_device, VkImage image)
    {
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkResult res;

        // Query image memory requirements
        VkMemoryRequirements mem_req;
        vkGetImageMemoryRequirements(device, image, &mem_req);

        // TODO: try implementing VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        static VkMemoryPropertyFlags memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Read more on memory types: https://asawicki.info/news_1740_vulkan_memory_types_on_pc_and_how_to_use_them
        uint32_t memory_type_index = utils::FindVulkanMemoryType(physical_device, memory_properties, mem_req.memoryTypeBits);
        if (memory_type_index == UINT32_MAX) {
            std::cerr << "[Image]: Failed to locate device memory of requested type" << std::endl;
            return VK_NULL_HANDLE;
        }

        // Allocate image required memory on physical device 
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_req.size;
        alloc_info.memoryTypeIndex = memory_type_index;

        res = vkAllocateMemory(device, &alloc_info, DEFAULT_VK_ALLOC, &memory);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);
        res = vkBindImageMemory(device, image, memory, 0);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return memory;
    }

    VkBuffer Image::CreateVulkanStagingBuffer(VkDevice device, VkDeviceSize buffer_size)
    {
        VkBuffer buffer = VK_NULL_HANDLE;

        VkBufferCreateInfo buffer_ci = {};
        buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_ci.size = buffer_size;
        buffer_ci.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        buffer_ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult res = vkCreateBuffer(device, &buffer_ci, nullptr, &buffer);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return buffer;
    }

    VkDeviceMemory Image::BindVulkanStagingBufferMemory(VkDevice device, VkPhysicalDevice physical_device, VkBuffer staging_buffer, [[maybe_unused]] VkDeviceSize memory_size)
    {
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkResult res;

        static VkMemoryPropertyFlags memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT; // Read more on memory types: https://asawicki.info/news_1740_vulkan_memory_types_on_pc_and_how_to_use_them

        // Query buffer memory requirements
        VkMemoryRequirements mem_req;
        vkGetBufferMemoryRequirements(device, staging_buffer, &mem_req);

        YART_ASSERT(memory_size == mem_req.size);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_req.size;
        alloc_info.memoryTypeIndex = utils::FindVulkanMemoryType(physical_device, memory_properties, mem_req.memoryTypeBits);

        res = vkAllocateMemory(device, &alloc_info, nullptr, &memory);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);
        res = vkBindBufferMemory(device, staging_buffer, memory, 0);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return memory;
    }

    bool Image::UploadDataToStagingBuffer(VkDevice device, VkDeviceMemory staging_buffer_memory, const void *data, VkDeviceSize data_size)
    {
        VkResult res;

        void* mapped_data = NULL;
        res = vkMapMemory(device, staging_buffer_memory, 0, data_size, 0, &mapped_data);
        CHECK_VK_RESULT_ABORT(res);

        // TODO: no need for memcpy without resizing. 
        //  This should probably return mapped_data to the caller
        memcpy(mapped_data, data, data_size);

        // Flush mapped memory - guarantee the data is uploaded to device memory 
        VkMappedMemoryRange range[1] = {};
        range[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range[0].memory = staging_buffer_memory;
        range[0].size = data_size;

        res = vkFlushMappedMemoryRanges(device, 1, range);
        CHECK_VK_RESULT_RETURN(res, false);

        vkUnmapMemory(device, staging_buffer_memory);

        return true;
    }

    bool Image::CopyStagingBufferToImage(VkDevice device, VkCommandPool command_pool, VkQueue queue, VkBuffer staging_buffer, VkImage image, VkExtent3D image_extent)
    {
        VkCommandBuffer command_buffer = yart::utils::BeginSingleTimeVulkanCommandBuffer(device, command_pool);

        VkImageMemoryBarrier copy_barrier = {};
        copy_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        copy_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        copy_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        copy_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        copy_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        copy_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_barrier.subresourceRange.levelCount = 1;
        copy_barrier.subresourceRange.layerCount = 1;
        copy_barrier.image = image;

        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &copy_barrier);

        VkBufferImageCopy region = {};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent = image_extent;

        vkCmdCopyBufferToImage(command_buffer, staging_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier use_barrier = {};
        use_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        use_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        use_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        use_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        use_barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        use_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        use_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        use_barrier.subresourceRange.levelCount = 1;
        use_barrier.subresourceRange.layerCount = 1;
        use_barrier.image = image;
        
        vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &use_barrier);

        // Submit and free command buffer
        if (!yart::utils::EndSingleTimeVulkanCommandBuffer(device, command_pool, queue, command_buffer))
            return false;

        return true;
    }

} // namespace yart
