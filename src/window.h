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


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper class for windowing backends
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Window {
    public:
        Window() = default;
        Window(const Window&) = delete;
        Window& operator=(Window const&) = delete;
        ~Window();

        /// @brief Initialize and open the window
        /// @param title Initial window title
        /// @param win_w Initial width of the window in pixels
        /// @param win_h Initial height of the window in pixels
        /// @return Boolean value indicating whether the window has been successfully created 
        bool Create(const char* title, int win_w, int win_h);

        /// @brief Submit frame for render and present the next frame-in-flight to the window
        void Render();

        /// @brief Perform window shutdown and cleanup
        void Close();

    private:
        typedef PFN_vkDebugUtilsMessengerCallbackEXT vk_debug_callback_t;

        /// @brief Container for swapchain related data
        struct SwapchainData {
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VkSurfaceFormatKHR surface_format = {};
            VkPresentModeKHR present_mode;
            VkExtent2D current_extent = {};

            std::unique_ptr<VkFramebuffer[]> vk_frame_buffers;
            std::unique_ptr<VkCommandPool[]> vk_command_pools;
            std::unique_ptr<VkCommandBuffer[]> vk_command_buffers;

            std::unique_ptr<VkSemaphore[]> vk_image_acquired_semaphore;
            std::unique_ptr<VkSemaphore[]> vk_render_complete_semaphore;
            std::unique_ptr<VkFence[]> vk_fences;

            uint32_t current_frame_in_flight = 0;
            uint32_t current_semaphore_index = 0;
            uint32_t min_image_count = 0;
            uint32_t max_image_count = 0;
            uint32_t image_count = 0;

            utils::LTStack ltStack;

            SwapchainData(const SwapchainData&) = delete;
            SwapchainData& operator=(SwapchainData const&) = delete;
            ~SwapchainData() { ltStack.Release(); };
        };

        /// @brief Initialize GLFW and create a new window 
        /// @param win_title Initial window title
        /// @param win_w Initial width of the window in pixels
        /// @param win_h Initial height of the window in pixels
        /// @return Boolean value indicating whether GLFW has been successfully initialized
        bool InitGLFW(const char* win_title, int win_w, int win_h);

        /// @brief Initialize Vulkan with supported extensions and create required Vk objects
        /// @return Boolean value indicating whether Vulkan with all required objects have been successfully initialized
        bool InitVulkan();
        static std::vector<const char*> GetRequiredVulkanExtensions();
        static VkInstance CreateVulkanInstance(const std::vector<const char*>& extensions);
        static VkDebugUtilsMessengerEXT CreateVulkanDebugMessenger(VkInstance instance, vk_debug_callback_t callback);
        static VkPhysicalDevice SelectVulkanPhysicalDevice(VkInstance instance, VkPhysicalDeviceProperties& properties);
        static bool GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags, VkSurfaceKHR surface = VK_NULL_HANDLE);
        static VkDevice CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family, const std::vector<const char*>& extensions);
        static VkDescriptorPool CreateVulkanDescriptorPool(VkDevice device);

        /// @brief Query present capabilities and create the initial swapchain
        /// @param surface Platform window Vulkan surface
        /// @return Boolean value indicating whether the swapchain with all required objects have been successfully initialized
        bool InitializeSwapchain(VkSurfaceKHR surface);
        static VkSwapchainKHR CreateVulkanSwapchain(VkDevice device, const SwapchainData& data, VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);

        bool CreateSwapchainFramesInFlight(const VkExtent2D& current_extent);
        static VkRenderPass CreateVulkanRenderPass(VkDevice device, const SwapchainData& data);
        static VkImageView CreateVulkanImageView(VkDevice device, VkFormat format, VkImage image);
        static VkFramebuffer CreateVulkanFramebuffer(VkDevice device, VkRenderPass render_pass, const VkExtent2D& extent, VkImageView image_view);

        /// @brief Initialize Dear ImGUI for GLFW/Vulkan
        /// @return Boolean value indicating whether Dear ImGUI has been successfully initialized
        bool InitImGUI();

        /// @brief Recreate the swapchain with extent of given size 
        /// @param width Swapchain image extent width 
        /// @param height Swapchain image extent height
        void WindowResize(uint32_t width, uint32_t height);

        /// @brief Submit draw commands for render
        /// @param draw_data Dear ImGui draw data
        /// @return Whether the swapchain is invalidated and should be rebuild
        bool FrameRender(ImDrawData* draw_data);

        /// @brief Present the next available frame-in-flight to the GLFW window
        /// @return Whether the swapchain is invalidated and should be rebuild
        bool FramePresent();

        /// @brief Wait for the GPU and destroy all allocated members. 
        /// @note It is safe to call this function even without initializing first
        void Cleanup();

    private:
        utils::LTStack m_ltStack;

        // -- GLFW TYPES -- //
        GLFWwindow* m_window = nullptr;

        // -- VULKAN TYPES -- //
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
