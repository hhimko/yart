#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h> // Platform backend
#include <backends/imgui_impl_vulkan.h> // Renderer backend

#include <memory>
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

        /* Starts the main loop of the application if not already running */
        int Run();
        void Shutdown();

    private:
        typedef PFN_vkDebugUtilsMessengerCallbackEXT vk_debug_callback_t;

        /* Container for swapchain related data */
        struct SwapchainData {
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VkSurfaceFormatKHR surface_format = {};
            VkPresentModeKHR present_mode;

            std::unique_ptr<VkCommandPool[]> vk_command_pools;
            std::unique_ptr<VkCommandBuffer[]> vk_command_buffers;

            std::unique_ptr<VkSemaphore[]> vk_image_acquired_semaphore;
            std::unique_ptr<VkSemaphore[]> vk_render_complete_semaphore;
            std::unique_ptr<VkFence[]> vk_fences;

            uint32_t current_frame_in_flight; 
            uint32_t min_image_count; 
            uint32_t max_image_count; 
            uint32_t image_count; 

            utils::LTStack ltStack;

            SwapchainData(const SwapchainData&) = delete;
            SwapchainData& operator=(SwapchainData const&) = delete;
            ~SwapchainData() { ltStack.Release(); };
        };

        int InitGLFW(int win_w, int win_h);

        int InitVulkan();
        static std::vector<const char*> GetRequiredVulkanExtensions();
        static VkInstance CreateVulkanInstance(std::vector<const char*>& extensions);
        static VkDebugUtilsMessengerEXT CreateVulkanDebugMessenger(VkInstance instance, vk_debug_callback_t callback);
        static VkPhysicalDevice SelectVulkanPhysicalDevice(VkInstance instance, VkPhysicalDeviceProperties& properties);
        static bool GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags, VkSurfaceKHR surface = VK_NULL_HANDLE);
        static VkDevice CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family, const std::vector<const char*>& extensions);
        static VkDescriptorPool CreateVulkanDescriptorPool(VkDevice device);

        /* Sets SwapchainData members and creates the initial Swapchain */
        bool InitializeSwapchain(VkSurfaceKHR surface);
        static VkSwapchainKHR CreateVulkanSwapchain(VkDevice device, const SwapchainData& data, VkExtent2D& current_extent, VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);

        bool CreateSwapchainFramesInFlight(VkExtent2D& current_extent);
        static VkRenderPass CreateVulkanRenderPass(VkDevice device, const SwapchainData& data);
        static VkImageView CreateVulkanImageView(VkDevice device, VkFormat format, VkImage image);
        static VkFramebuffer CreateVulkanFramebuffer(VkDevice device, VkRenderPass render_pass, VkExtent2D& extent, VkImageView image_view);

        bool InitImGUI();

        /* Rebuilds the swapchain */
        void WindowResize(uint32_t width, uint32_t height);

        /* Returns whether the swapchain should be resized/rebuilt */
        bool FrameRender(ImDrawData* draw_data);
        /* Returns whether the swapchain should be resized/rebuilt */
        bool FramePresent();

        void Cleanup();

    private:
        bool m_initialized = false; // Whether the backends were successfully initialized 
        bool m_running = false;

        utils::LTStack m_ltStack;

        // GLFW types
        GLFWwindow* m_window = nullptr;

        // Vulkan types
        VkInstance m_vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_vkDevice = VK_NULL_HANDLE;
        VkQueue m_vkQueue = VK_NULL_HANDLE;
        uint32_t m_queueFamily = 0;
        VkDescriptorPool m_vkDescriptorPool = VK_NULL_HANDLE;
        VkSwapchainKHR m_vkSwapchain = VK_NULL_HANDLE;
        SwapchainData m_swapchainData = {};
        VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
        
    };
} // namespace yart
