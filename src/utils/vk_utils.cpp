#include "vk_utils.h"

#include <memory>


int yart::utils::CheckVulkanInstanceExtensionsAvailable(const std::vector<const char*>& extensions)
{
    // Query available extensions from Vulkan
    uint32_t prop_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, nullptr);

    auto props = std::make_unique<VkExtensionProperties[]>(prop_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, props.get());

    // Check all required extensions for availability
    for (size_t i = 0; i < extensions.size(); ++i) {
        bool found = false;
        
        for (size_t j = 0; j < prop_count; j++) {
            if (!strcmp(extensions[i], props[j].extensionName)) {
                found = true;
                break;
            }
        }

        if (!found)
            return static_cast<int>(i);
    }
    
    return -1;
}

int yart::utils::CheckVulkanDeviceExtensionsAvailable(VkPhysicalDevice device, const std::vector<const char *> &extensions)
{
    // Query available extensions from Vulkan
    uint32_t prop_count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &prop_count, nullptr);

    auto props = std::make_unique<VkExtensionProperties[]>(prop_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &prop_count, props.get());

    // Check all required extensions for availability
    for (size_t i = 0; i < extensions.size(); ++i) {
        bool found = false;
        for (size_t j = 0; j < prop_count; j++) {
            if (!strcmp(extensions[i], props[j].extensionName)) {
                found = true;
                break;
            }
        }

        if (!found)
            return static_cast<int>(i);
    }
    
    return -1;
}

VkSurfaceFormatKHR yart::utils::RequestVulkanSurfaceFormat(VkPhysicalDevice device, VkSurfaceKHR surface, VkFormat request_format, VkColorSpaceKHR request_color_space)
{
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

    auto formats = std::make_unique<VkSurfaceFormatKHR[]>(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, formats.get());

    for (uint32_t i = 0; i < format_count; ++i) {
        if (formats[i].format == request_format && formats[i].colorSpace == request_color_space)
            return formats[i];
    }

    return formats[0];
}

VkPresentModeKHR yart::utils::RequestVulkanSurfacePresentMode(VkPhysicalDevice device, VkSurfaceKHR surface, VkPresentModeKHR request_present_mode)
{
    uint32_t mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count, nullptr);

    auto modes = std::make_unique<VkPresentModeKHR[]>(mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &mode_count, modes.get());

    for (uint32_t i = 0; i < mode_count; ++i) {
        if (modes[i] == request_present_mode)
            return request_present_mode;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

uint32_t yart::utils::GetMinImageCountFromPresentMode(VkPresentModeKHR mode)
{
    YART_SUPPRESS_PUSH(4061); // enumerator not explicitly handled by a case label
    switch(mode) {
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return 3;
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
        case VK_PRESENT_MODE_FIFO_KHR:
            return 2;
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return 1;
        default:
            break;
    }
    YART_SUPPRESS_POP();

    YART_UNREACHABLE();
    return 0;
}

uint32_t yart::utils::FindVulkanMemoryType(VkPhysicalDevice device, VkMemoryPropertyFlags property_flags, uint32_t type_bits)
{
    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(device, &prop);

    for (uint32_t i = 0; i < prop.memoryTypeCount; ++i) {
        if ((prop.memoryTypes[i].propertyFlags & property_flags) == property_flags && type_bits & (1 << i))
            return i;
    }
    
    return UINT32_MAX;
}

VkCommandBuffer yart::utils::BeginSingleTimeVulkanCommandBuffer(VkDevice device, VkCommandPool command_pool)
{
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    VkResult res;

    VkCommandBufferAllocateInfo cmd_ai = {};
    cmd_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_ai.commandPool = command_pool;
    cmd_ai.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(device, &cmd_ai, &command_buffer);
    CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    res = vkBeginCommandBuffer(command_buffer, &begin_info);
    if (res != VK_SUCCESS) {
        vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
        return VK_NULL_HANDLE;
    }

    return command_buffer;
}

bool yart::utils::EndSingleTimeVulkanCommandBuffer(VkDevice device, VkCommandPool command_pool, VkQueue queue, VkCommandBuffer command_buffer)
{
    VkResult res;

    res = vkEndCommandBuffer(command_buffer);
    CHECK_VK_RESULT_RETURN(res, false);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    res = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE); // vkQueueWaitIdle is used instead of a fence
    CHECK_VK_RESULT_RETURN(res, false);

    // Ensure command buffer has finished executing
    res = vkQueueWaitIdle(queue);
    CHECK_VK_RESULT_RETURN(res, false);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);

    return true;
}
