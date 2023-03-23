#pragma once

#include <vulkan/vulkan.h>

#include <stdio.h>
#include <vector>


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Vulkan Macros & Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef YART_DEBUG
    #define YART_VULKAN_DEBUG_UTILS // Enable Vulkan validation layers and debug utils
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

// Helper macro for loading in extension function pointers from VkInstance into the local scope
#define LOAD_VK_INSTANCE_FP(instance, name)         \
    auto name = reinterpret_cast<PFN_##name>(       \
        reinterpret_cast<void*>(                    \
            vkGetInstanceProcAddr(instance, #name)  \
        )                                           \
    )


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Helper Vulkan Utility Functions 
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace yart
{
    namespace utils
    {
        /**
         * @brief Check whether required Vulkan extensions are provided by the Vulkan implementation
         * @return Negative result if all extensions are available, or the index of the first not 
         *  available extension in `extensions` vector
         */
        int CheckVulkanExtensionsAvailable(std::vector<const char*> extensions);

    } // namespace utils
} // namespace yart
