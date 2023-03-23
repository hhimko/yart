#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "utils/core_utils.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLFW Application Window
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace yart
{
    class Application {
    public:
        Application(int win_w, int win_h);
        Application(const Application&) = delete;
        Application& operator=(Application const&) = delete;
        ~Application();

        int Run();

    private:
        int InitGLFW(int win_w, int win_h);

        int InitVulkan();
        static std::vector<const char*> GetRequiredVulkanExtensions();
        static VkInstance CreateVulkanInstance(std::vector<const char*> extensions); 
        VkDebugUtilsMessengerEXT CreateVulkanDebugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback);

        void Cleanup();

    private:
        utils::LTStack m_ltStack;

        // GLFW types
        GLFWwindow* m_window = nullptr;

        // Vulkan types
        VkInstance m_vkInstance = VK_NULL_HANDLE;
        
    };
} // namespace yart
