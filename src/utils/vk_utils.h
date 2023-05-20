#pragma once

#include <vulkan/vulkan.h>

#include <stdio.h>
#include <vector>

#include "core_utils.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Vulkan Macros & Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef YART_DEBUG
    // Enable Vulkan validation layers and debug utils
    #define YART_VULKAN_DEBUG_UTILS 
#endif

#ifdef YART_DEBUG
    #define CHECK_VK_RESULT_RETURN(res, ret)                                                        \
        if (res != VK_SUCCESS) {                                                                    \
            fprintf(stderr, "[%s(%d)] Vulkan Error: VkResult = %d\n", __FUNCTION__, __LINE__, res); \
            if (res < 0)                                                                            \
                return ret;                                                                         \
        }
#else
    #define CHECK_VK_RESULT_RETURN(res, ret)    \
        if (res < 0)                            \
            return ret;
#endif

#define CHECK_VK_RESULT_ABORT(res)    \
        if (res < 0)                            \
            YART_ABORT("VkResult != VK_SUCCESS");

// Helper macro for loading in extension function pointers from VkInstance into the local scope
#define LOAD_VK_INSTANCE_FP(instance, name)         \
    auto name = reinterpret_cast<PFN_##name>(       \
        reinterpret_cast<void*>(                    \
            vkGetInstanceProcAddr(instance, #name)  \
        )                                           \
    )



namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //  Helper Vulkan Utility Functions 
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    namespace utils
    {
        /// @brief Check whether required Vulkan extensions are provided by the Vulkan implementation
        /// @return Negative result if all extensions are available, or the index of the first not 
        ///     available extension in `extensions` vector
        int CheckVulkanInstanceExtensionsAvailable(const std::vector<const char*>& extensions);

        /// @brief Check whether required Vulkan extensions are provided by the physical device driver
        /// @return Negative result if all extensions are available, or the index of the first not 
        ///     available extension in `extensions` vector
        int CheckVulkanDeviceExtensionsAvailable(VkPhysicalDevice device, const std::vector<const char*>& extensions);

        /// @brief  Request a certain Vulkan surface format and confirm that it is supported on physical device
        /// @return Requested format or the first available one
        VkSurfaceFormatKHR RequestVulkanSurfaceFormat(VkPhysicalDevice device, VkSurfaceKHR surface, VkFormat request_format, VkColorSpaceKHR request_color_space);

        /// @brief Request a certain Vulkan present mode and confirm that it is supported on physical device
        /// @return Requested present mode if available or VK_PRESENT_MODE_FIFO_KHR (always guaranteed)
        VkPresentModeKHR RequestVulkanSurfacePresentMode(VkPhysicalDevice device, VkSurfaceKHR surface, VkPresentModeKHR request_present_mode);

        uint32_t GetMinImageCountFromPresentMode(VkPresentModeKHR mode);

    } // namespace utils
} // namespace yart
