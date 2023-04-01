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
