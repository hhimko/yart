////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Definition of helper utility functions for Vulkan
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <stdio.h>
#include <vector>

#include <vulkan/vulkan.h>

#include "yart/common/utils/yart_utils.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Vulkan Macros & Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Default value for Vulkan's `VkAllocationCallbacks` parameters (`VK_NULL_HANDLE`) 
/// @details The purpose of this macro is to help locate all VkAllocationCallbacks dependencies throughout the file
#define DEFAULT_VK_ALLOC VK_NULL_HANDLE

#ifdef YART_DEBUG
    /// @brief Check Vulkan's `VkResult` return value and log to `stderr` on error
    /// @param res Value from Vulkan's `VkResult` enum
    /// @param ret Error value to be returned when `ret` is other than `VK_SUCCESS`
    /// @note This macro exits quietly on release builds and prints to `stderr` on debug builds
    #define CHECK_VK_RESULT_RETURN(res, ret)                                                        \
        if (res != VK_SUCCESS) {                                                                    \
            YART_LOG_ERR("[%s(%d)] Vulkan Error: VkResult = %d\n", __FUNCTION__, __LINE__, res); \
            if (res < 0)                                                                            \
                return ret;                                                                         \
        }
#else
    /// @brief Check Vulkan's `VkResult` return value
    /// @param res Value from Vulkan's `VkResult` enum
    /// @param ret Error value to be returned when `ret` is other than `VK_SUCCESS`
    /// @note This macro exits quietly on release builds and prints to `stderr` on debug builds
    #define CHECK_VK_RESULT_RETURN(res, ret)    \
        if (res < 0)                            \
            return ret;
#endif // #ifdef YART_DEBUG

/// @brief Check the value of Vulkan's `VkResult` return value and abort the program on error
/// @param res Value from Vulkan's `VkResult` enum
#define CHECK_VK_RESULT_ABORT(res)            \
    if (res < 0)                              \
        YART_ABORT("VkResult != VK_SUCCESS");

/// @brief Check whether a Vulkan handle has been initialized successfully and print to `stderr` on error
/// @param handle Handle to a Vulkan object
/// @param err Error message to print on failed assertion
#define ASSERT_VK_HANDLE_INIT(handle, err)  \
    if (handle == VK_NULL_HANDLE) {         \
        YART_LOG_ERR(err);                  \
        return 0;                           \
    }

/// @brief Helper macro for loading in Vulkan extension function pointers from `VkInstance` into the local scope
/// @param instance A valid `VkInstance` handle
/// @param name Name of the extension to load
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

        /// @brief Return the required swapchain image count from a given Vulkan present mode 
        /// @param mode Requested Vulkan present mode
        /// @return Number of required minimum swapchain images
        uint32_t GetMinImageCountFromPresentMode(VkPresentModeKHR mode);

        /// @brief Query available GPU memory types and return the index to a memory type with given properties
        /// @param device Physical device on which to query available memory types
        /// @param property_flags Required memory type properties 
        /// @param type_bits Bit field of suitable memory types 
        /// @return Index to the most suitable memory type or UINT32_MAX if requested memory type was not found
        /// @note For more valuable information on Vulkan memory types check out the great 
        ///     <a href="https://asawicki.info/news_1740_vulkan_memory_types_on_pc_and_how_to_use_them">blog entry</a> by Adam Sawicki
        uint32_t FindVulkanMemoryType(VkPhysicalDevice device, VkMemoryPropertyFlags property_flags, uint32_t type_bits);

        /// @brief Allocate and begin a single time submit command buffer from a given command pool
        /// @details Recording to the command buffer should be concluded later, using EndSingleTimeVulkanCommandBuffer()
        /// @param device Handle to a Vulkan device, on which to allocate the command buffer
        /// @param command_pool Handle to a Vulkan command pool from which to allocate the command buffer
        /// @return Handle to the newly allocated command buffer or `VK_NULL_HANDLE` on fail
        VkCommandBuffer BeginSingleTimeVulkanCommandBuffer(VkDevice device, VkCommandPool command_pool);

        /// @brief Finish recording commands to, and free a single time command buffer, created using BeginSingleTimeVulkanCommandBuffer()
        /// @param device Handle to the Vulkan device, on which the command buffer was created
        /// @param command_pool Handle to the Vulkan command pool from which the command buffer was allocated  
        /// @param queue Handle to a Vulkan queue on which to submit the command pool
        /// @param command_buffer Handle to the Vulkan command buffer
        /// @return Whether the command buffer was successfully submitted
        /// @warning This method blocks the CPU until the queue has finished executing
        bool EndSingleTimeVulkanCommandBuffer(VkDevice device, VkCommandPool command_pool, VkQueue queue, VkCommandBuffer command_buffer);

    } // namespace utils
} // namespace yart
