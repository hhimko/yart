#include "vk_utils.h"


int yart::utils::CheckVulkanExtensionsAvailable(std::vector<const char*> extensions)
{
    // Query available extensions from Vulkan
    uint32_t prop_count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, nullptr);

    VkExtensionProperties* props = new VkExtensionProperties[prop_count];
    vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, props);

    // Check all required extensions for availability
    for (size_t i = 0; i < extensions.size(); ++i) {
         bool found = false;
        
        for (size_t j = 0; j < prop_count; j++) {
            if (!strcmp(extensions[i], props[j].extensionName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            delete[] props;
            return static_cast<int>(i);
        }
    }
    
    delete[] props;
    return -1;
}