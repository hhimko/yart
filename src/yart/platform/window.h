#pragma once


#include <functional>
#include <memory>
#include <vector>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h> // Platform backend
#include <backends/imgui_impl_vulkan.h> // Renderer backend

#include "viewport.h"
#include "yart/utils/yart_utils.h"


namespace yart
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper singleton class for windowing backends
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Window {
    public:
        /// @brief Callback function for rendering custom ImGui windows  
        typedef std::function<void()> imgui_callback_t;

        Window(const Window&) = delete;
        Window& operator=(Window const&) = delete;
        ~Window();

        /// @brief Get the static singleton instance, lazily initialized on first call
        /// @return Static Window instance
        static Window& Get();

        /// @brief Initialize and open the window
        /// @param title Initial window title
        /// @param win_w Initial width of the window in pixels
        /// @param win_h Initial height of the window in pixels
        /// @return Whether the window has been successfully created 
        bool Init(const char* title, int win_w, int win_h);

        /// @brief Submit frame for render and present the next frame-in-flight to the window
        void Render();

        /// @brief Register a custom ImGui window to be rendered every frame 
        /// @param window_name Name of the window
        /// @param callback Callback function to window contents definition 
        void RegisterImGuiWindow(const char* window_name, imgui_callback_t callback);

        /// @brief Get the main window viewport instance
        /// @return Pointer to a Viewport instance
        std::shared_ptr<yart::Viewport> GetViewport() const 
        {
            return m_viewport;
        }

    private:
        Window() = default;

        /// @brief Container for per frame-in-flight related data
        /// @note FrameInFlight members are all set in Window::CreateSwapchainFramesInFlight
        struct FrameInFlight {
            VkFramebuffer vkFrameBuffer;
            VkCommandPool vkCommandPool;
            VkCommandBuffer vkCommandBuffer;

            VkSemaphore vkImageAcquiredSemaphore;
            VkSemaphore vkRenderCompleteSemaphore;
            VkFence vkFence;

            FrameInFlight() = default;
            FrameInFlight(const FrameInFlight&) = delete;
            FrameInFlight& operator=(FrameInFlight const&) = delete;
        };


        /// @brief Initialize GLFW and create a new window 
        /// @param win_title Initial window title
        /// @param win_w Initial width of the window in pixels
        /// @param win_h Initial height of the window in pixels
        /// @return Whether GLFW has been successfully initialized
        bool InitGLFW(const char* win_title, int win_w, int win_h);

        /// @brief Initialize Vulkan with supported extensions and create required Vk objects
        /// @return Whether Vulkan with all required objects have been successfully initialized
        bool InitVulkan();
        static std::vector<const char*> GetRequiredVulkanExtensions();
        static VkInstance CreateVulkanInstance(const std::vector<const char*>& extensions);
        static VkDebugUtilsMessengerEXT CreateVulkanDebugMessenger(VkInstance instance, PFN_vkDebugUtilsMessengerCallbackEXT callback);
        static VkPhysicalDevice SelectVulkanPhysicalDevice(VkInstance instance, VkPhysicalDeviceProperties& properties);
        static bool GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags, VkSurfaceKHR surface = VK_NULL_HANDLE);
        static VkDevice CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family, const std::vector<const char*>& extensions);
        static VkDescriptorPool CreateVulkanDescriptorPool(VkDevice device);

        /// @brief Query present capabilities and create the initial swapchain
        /// @return Whether the swapchain with all required objects have been successfully initialized
        bool InitializeSwapchain();
        static VkSwapchainKHR CreateVulkanSwapchain(VkDevice device, VkSurfaceKHR surface, VkSurfaceFormatKHR surface_format, VkPresentModeKHR present_mode, VkExtent2D extent, uint32_t min_image_count, VkSwapchainKHR old_swapchain);

        bool CreateSwapchainFramesInFlight(const VkExtent2D& current_extent, bool recreate);
        static VkRenderPass CreateVulkanRenderPass(VkDevice device, VkSurfaceFormatKHR surface_format);
        static VkImageView CreateVulkanImageView(VkDevice device, VkFormat format, VkImage image);
        static VkFramebuffer CreateVulkanFramebuffer(VkDevice device, VkRenderPass render_pass, const VkExtent2D& extent, VkImageView image_view);

        /// @brief Initialize Dear ImGUI for GLFW/Vulkan
        /// @return Whether Dear ImGUI has been successfully initialized
        bool InitImGUI();

        /// @brief Record Dear ImGUI render commands for this class
        void OnImGUI();

        /// @brief Create viewport images along with a shared sampler
        /// @return Whether viewports have been successfully created
        bool CreateViewports();

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
        /// @note It is safe to call this function without prior initialization
        void Cleanup();

    private:
        /// @brief Struct containing data for displaying custom ImGUI windows
        struct ImGuiWindow {
            const char* name;
            imgui_callback_t callback;
        };

        utils::LTStack m_LTStack;
        std::vector<ImGuiWindow> m_registeredImGuiWindows;
        std::shared_ptr<yart::Viewport> m_viewport = nullptr; // Main window viewport
        
        // -- SWAPCHAIN DATA -- // 
        utils::LTStack m_swapchainLTStack;
        bool m_shouldRebuildSwapchain = false; // Signals whether the swapchain should be rebuild/resized next frame 
        std::vector<FrameInFlight> m_framesInFlight;

        VkSurfaceKHR m_vkSurface = VK_NULL_HANDLE;
        VkSurfaceFormatKHR m_surfaceFormat = {};
        VkPresentModeKHR m_surfacePresentMode;
        VkExtent2D m_surfaceExtent = {};

        uint32_t m_currentFrameInFlight = 0;
        uint32_t m_currentSemaphoreIndex = 0;
        uint32_t m_minImageCount = 0;
        uint32_t m_maxImageCount = 0;
        uint32_t m_imageCount = 0;

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
        VkRenderPass m_vkRenderPass = VK_NULL_HANDLE;
        VkSampler m_viewportImageSampler = VK_NULL_HANDLE;
        

        // -- FRIEND FUNCTION DECLARATIONS -- //
        friend yart::Viewport::Viewport(uint32_t width, uint32_t height);
        friend void yart::Viewport::Resize(uint32_t width, uint32_t height);
        friend void yart::Viewport::Refresh();

    };
} // namespace yart
