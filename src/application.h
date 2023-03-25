#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h> // Platform backend
#include <backends/imgui_impl_vulkan.h> // Renderer backend

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

        static Application& Get();

        int Run();
        void Close();

    private:
        typedef PFN_vkDebugUtilsMessengerCallbackEXT vk_debug_callback_t;
        int InitGLFW(int win_w, int win_h);

        int InitVulkan();
        static std::vector<const char*> GetRequiredVulkanExtensions();
        static VkInstance CreateVulkanInstance(std::vector<const char*> extensions);
        static VkDebugUtilsMessengerEXT CreateVulkanDebugMessenger(VkInstance instance, vk_debug_callback_t callback);
        static VkPhysicalDevice SelectVulkanPhysicalDevice(VkInstance instance, VkPhysicalDeviceProperties& properties);
        static bool GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags);
        static VkDevice CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family);

        void Cleanup();

    private:
        bool m_running = false;

        utils::LTStack m_ltStack;

        // GLFW types
        GLFWwindow* m_window = nullptr;

        // Vulkan types
        VkInstance m_vkInstance = VK_NULL_HANDLE;
        
    };
} // namespace yart
