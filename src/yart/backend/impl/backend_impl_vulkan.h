////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Backend module implementation for Vulkan and GLFW  
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h> // Platform backend
#include <backends/imgui_impl_vulkan.h> // Renderer backend
#include <vulkan/vulkan.h>

#include "yart/backend/utils/backend_utils.h"
#include "yart/backend/backend.h"


#ifdef YART_DEBUG
    // Enable Vulkan validation layers and debug utils
    #define YART_VULKAN_DEBUG_UTILS 
#endif


namespace yart
{
    namespace Backend
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Image class implementation for Vulkan, for managing and uploading 2D textures to the GPU  
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        class VulkanImage : Image {
        public:
            /// @brief Upload and bind pixel data to the image  
            /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to `(width * height * channels)`,
            ///     where `channels` is the number of channels in the image, based on the image format used
            void BindData(const void* data) override;

            /// @brief Rebuild the image for a new given size
            /// @param width New image width in texels
            /// @param height New image height in texels
            /// @warning Rebuilding essentially leads to the loss of previously bound image data, so Image::BindData() has to be called after the resize
            void Resize(uint32_t width, uint32_t height) override;

            /// @brief Set the image sampler type used for interpolation
            /// @param sampler New sampler type 
            void SetSampler(ImageSampler sampler) override;

            /// @brief Get the image ID, used by Dear ImGui for differentiating GPU textures
            /// @return Dear ImGui's `ImTextureID` for this image
            ImTextureID GetImTextureID() const override
            {
                return static_cast<ImTextureID>(m_vkDescriptorSet);
            }
        
        private:
            /// @brief Construct a new image implemented for Vulkan, without uploading and binding pixel data 
            /// @param width Initial image width in texels
            /// @param height Initial image height in texels
            /// @param format Image format from the `ImageFormat` enum
            /// @param sampler Image sampler used for interpolation from the `ImageSampler` enum
            VulkanImage(uint32_t width, uint32_t height, ImageFormat format, ImageSampler sampler = ImageSampler::NEAREST);

            /// @brief Construct a new image implemented for Vulkan, and upload and bind initial pixel data 
            /// @param width Initial image width in texels
            /// @param height Initial image height in texels
            /// @param data Pointer to pixel array to upload and bind. The size of the array must be equal to `(width * height * channels)`,
            ///     where `channels` is the number of channels in the image, based on the image format used
            /// @param format Image format from the `ImageFormat` enum
            /// @param sampler Image sampler used for interpolation from the `ImageSampler` enum
            VulkanImage(uint32_t width, uint32_t height, const void* data, ImageFormat format, ImageSampler sampler = ImageSampler::NEAREST);

            /// @brief VulkanImage class destructor
            ~VulkanImage();

            /// @brief Release all Vulkan allocations made by this object
            /// @warning VulkanImage::Release() stalls CPU execution until the GPU is idle
            void Release();

            static constexpr VkFormat GetVulkanFormatFromImageFormat(ImageFormat format)
            {
                switch (format) {
                case ImageFormat::R32G32B32A32_FLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
                case ImageFormat::COUNT:
                default:
                    YART_ABORT("Unknown ImageFormat value passed to `Backend::VulkanImage::GetVulkanFormatFromImageFormat`\n");
                }

                YART_UNREACHABLE();
                return (VkFormat)0;
            }

            // -- DESCRIPTOR SET CREATION -- //

            /// @brief Create Vulkan's descriptor set for the image
            void CreateDescriptorSet();

            /// @brief Create a Vulkan's `VkImage` object
            /// @param device Logical device on which to allocate the image
            /// @param format Format of the image
            /// @param width Width of the image
            /// @param height Height of the image
            /// @return `VkImage` handle or `VK_NULL_HANDLE` on failure
            static VkImage CreateVulkanImage(VkDevice device, VkFormat format, uint32_t width, uint32_t height);

            /// @brief Allocate and bind device memory for a Vulkan image
            /// @param device Logical device on which to allocate memory 
            /// @param physical_device Physical device used for querying memory type availability
            /// @param image Vulkan's `VkImage` handle
            /// @return `VkDeviceMemory` handle or `VK_NULL_HANDLE` on failure
            static VkDeviceMemory BindVulkanImageDeviceMemory(VkDevice device, VkPhysicalDevice physical_device, VkImage image);

            /// @brief Create a staging buffer for uploading image data to the device memory
            /// @param device Logical device on which to allocate the buffer 
            /// @param buffer_size Size of the buffer in bytes
            /// @return `VkBuffer` handle or `VK_NULL_HANDLE` on failure
            static VkBuffer CreateVulkanStagingBuffer(VkDevice device, VkDeviceSize buffer_size);

            /// @brief Allocate and bind device memory for a Vulkan buffer
            /// @param device Logical device on which to allocate memory 
            /// @param physical_device Physical device used for querying memory type availability
            /// @param buffer Vulkan's `VkBuffer` handle
            /// @return `VkDeviceMemory` handle or `VK_NULL_HANDLE` on failure
            static VkDeviceMemory BindVulkanBufferMemory(VkDevice device, VkPhysicalDevice physical_device, VkBuffer buffer);

            // -- GPU MEMORY BINDING -- //

            /// @brief Upload image data to a device staging buffer
            /// @param device Logical device handle on which the staging buffer was allocated
            /// @param staging_buffer_memory Staging buffer memory range to be mapped with the image data
            /// @param data Image data array
            /// @param data_size Size of the `data` array in bytes. Should to be equal to `(width * height * channels)`,
            ///     where `channels` is the number of channels in the image, based on the image format used
            /// @return Whether the data has been successfully uploaded to the staging buffer
            static bool UploadDataToStagingBuffer(VkDevice device, VkDeviceMemory staging_buffer_memory, const void* data, VkDeviceSize data_size);

            /// @brief Upload the staging buffer contents to Vulkan's `VkImage`
            /// @param device Logical device handle on which the staging buffer was allocated
            /// @param command_pool Vulkan's `VkCommandPool` handle used for submitting copy commands
            /// @param queue Logical device's graphics queue
            /// @param buffer Staging buffer from which to copy data
            /// @param image Vulkan's `VkImage` handle
            /// @param width Current image width in texels
            /// @param height Current image height in texels
            /// @return Whether the staging buffer data has been successfully copied to the GPU image
            static bool CopyStagingBufferToImage(VkDevice device, VkCommandPool command_pool, VkQueue queue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        private:
            VkDescriptorSet m_vkDescriptorSet = VK_NULL_HANDLE;
            VkImage m_vkImage = VK_NULL_HANDLE;
            VkDeviceMemory m_vkDeviceMemory = VK_NULL_HANDLE;
            VkImageView m_vkImageView = VK_NULL_HANDLE;
            VkBuffer m_vkStagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory m_vkStagingBufferMemory = VK_NULL_HANDLE;

            // -- FRIEND DECLARATIONS -- //
            friend yart::Backend::Image* CreateImage(uint32_t, uint32_t, ImageFormat, ImageSampler);
            friend yart::Backend::Image* CreateImage(uint32_t, uint32_t, const void*, ImageFormat, ImageSampler);

        };


        /// @brief Container for per frame-in-flight related data
        struct FrameInFlight {
        public:
            FrameInFlight() = default;

            VkFramebuffer vkFrameBuffer = VK_NULL_HANDLE;
            VkCommandPool vkCommandPool = VK_NULL_HANDLE;
            VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

            VkSemaphore vkImageAcquiredSemaphore = VK_NULL_HANDLE;
            VkSemaphore vkRenderCompleteSemaphore = VK_NULL_HANDLE;
            VkFence vkFence = VK_NULL_HANDLE;

        private:
            FrameInFlight(const FrameInFlight&) = delete;
            FrameInFlight& operator=(FrameInFlight const&) = delete;
        
        };

        /// @brief Context struct for the Backend module Vulkan/GLFW implementation
        struct BackendContext {
        public:
            utils::LTStack LT;
            utils::LTStack swapchainLT;
            std::unique_ptr<FrameInFlight[]> framesInFlight = nullptr;
            std::vector<Backend::Image*> allocatedImages = { };

            // -- GLFW TYPES -- //
            GLFWwindow* window = nullptr;

            // -- VULKAN TYPES -- //
            VkInstance vkInstance = VK_NULL_HANDLE;
            VkSurfaceKHR vkSurface = VK_NULL_HANDLE;
            VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
            uint32_t vkQueueFamily = 0U;
            VkDevice vkDevice = VK_NULL_HANDLE;
            VkQueue vkQueue = VK_NULL_HANDLE;
            VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;

            VkSurfaceFormatKHR vkSurfaceFormat = { };
            VkPresentModeKHR vkSurfacePresentMode = (VkPresentModeKHR)0;
            VkExtent2D vkSurfaceExtent = { };
            VkRenderPass vkRenderPass = VK_NULL_HANDLE;
            VkSwapchainKHR vkSwapchain = VK_NULL_HANDLE;
            uint32_t minImageCount = 0;
            uint32_t maxImageCount = 0;
            uint32_t imageCount = 0;
            uint32_t currentFrameInFlightIndex = 0;
            uint32_t currentSemaphoreIndex = 0;
            bool shouldRebuildSwapchain = false;

            // -- IMAGE SAMPLERS -- //
            VkSampler vkSampler_Nearest = VK_NULL_HANDLE;
            VkSampler vkSampler_Linear = VK_NULL_HANDLE;
            VkSampler vkSampler_Cubic = VK_NULL_HANDLE;

            // -- EVENT CALLBACKS -- //
            event_callback_t onDearImGuiSetupCallback = nullptr;
            event_callback_t onWindowCloseCallback = nullptr;

        private:
            BackendContext(const BackendContext&) = delete;
            BackendContext& operator=(BackendContext const&) = delete;

        };


        /// @brief Get the currently active backend context instance
        /// @return The current context
        BackendContext& GetBackendContext();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Various initialization and setup functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Initialize GLFW and create a new window 
        /// @param window_title Initial window title
        /// @param window_width Initial width of the window in pixels
        /// @param window_height Initial height of the window in pixels
        /// @return Whether GLFW has been successfully initialized
        bool InitGLFW(const char* window_title, uint32_t window_width, uint32_t window_height);

        /// @brief Initialize Vulkan with supported extensions and create the required Vk objects
        /// @return Whether Vulkan has been successfully initialized
        bool InitVulkan();

        /// @brief Initialize Dear ImGui for GLFW/Vulkan
        /// @return Whether Dear ImGui has been successfully initialized
        bool InitImGui();


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Vulkan handles creation and configuration helper functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Type used by Vulkan for specifying extensions
        using vk_extension_t = const char*;


        /// @brief Query all the Vulkan extensions minimally required by the application to run  
        /// @return Vector of required extensions
        std::vector<vk_extension_t> GetRequiredVulkanExtensions();

        /// @brief Create Vulkan's `VkInstance` object using given Vulkan extensions
        /// @param extensions Vector of required extensions
        /// @return `VkInstance` handle or `VK_NULL_HANDLE` on failure
        VkInstance CreateVulkanInstance(const std::vector<vk_extension_t>& extensions);

        /// @brief Create an all-purpose Vulkan debug messenger object 
        /// @param instance Vulkan instance for which to build the debug messenger
        /// @param callback Debug event callback function
        /// @return The debug messenger handle or `VK_NULL_HANDLE` on failure
        VkDebugUtilsMessengerEXT CreateVulkanDebugMessenger(VkInstance instance, PFN_vkDebugUtilsMessengerCallbackEXT callback);

        /// @brief Select an available Vulkan physical device, ideally a discrete GPU
        /// @param instance Vulkan instance from which to query devices
        /// @param properties Properties of the returned physical device
        /// @return First discrete GPU found on client machine, or the very first available one
        VkPhysicalDevice SelectVulkanPhysicalDevice(VkInstance instance, VkPhysicalDeviceProperties& properties);

        /// @brief Select a queue family index with optional support for surface presentation (WSI)
        /// @param physical_device GPU for which to query queue families
        /// @param result Output parameter, set with the result family index
        /// @param flags Required Vulkan queue flags 
        /// @param surface Vulkan surface handle for which to query for WSI support, or `VK_NULL_HANDLE` if not required
        /// @return Whether a queue with sufficient support was successfully found, and the `result` variable was set
        bool GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags, VkSurfaceKHR surface = VK_NULL_HANDLE);

        /// @brief Create Vulkan's `VkDevice` object from a physical device with required extensions and a single queue
        /// @param physical_device GPU from which to create the device
        /// @param queue_family A queue family from which to create a queue for the device
        /// @param extensions Required `VkDevice` extensions 
        /// @return `VkDevice` handle or `VK_NULL_HANDLE` on failure
        VkDevice CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family, const std::vector<vk_extension_t>& extensions);

        /// @brief Create Vulkan's `VkDescriptorPool` object for a given logical device
        /// @param device Logical device on which to allocate the pool
        /// @return `VkDescriptorPool` handle or `VK_NULL_HANDLE` on failure
        VkDescriptorPool CreateVulkanDescriptorPool(VkDevice device);

        // -- SWAPCHAIN CREATION -- //

        /// @brief Query present capabilities and create the initial swapchain
        /// @return Whether the swapchain with all required objects have been successfully initialized
        bool InitializeSwapchain();

        /// @brief Create Vulkan's `VkRenderPass` object for a given logical device
        /// @param device Logical device for which to create the render pass
        /// @param surface_format Vulkan surface format
        /// @return `VkRenderPass` handle or `VK_NULL_HANDLE` on failure
        VkRenderPass CreateVulkanRenderPass(VkDevice device, VkSurfaceFormatKHR surface_format);

        /// @brief Create Vulkan's `VkSwapchainKHR` object, or recreate it from an old one
        /// @param device Logical device for which to create the swapchain
        /// @param surface Swapchain surface
        /// @param surface_format Format of the swapchain surface from the `VkSurfaceFormatKHR` enum
        /// @param present_mode Surface present mode from the `VkPresentModeKHR` enum 
        /// @param extent Surface extent
        /// @param min_image_count Minumum swapchain images count
        /// @param old_swapchain Optional old swapchain to recreate
        /// @return `VkSwapchainKHR` handle or `VK_NULL_HANDLE` on failure
        VkSwapchainKHR CreateVulkanSwapchain(VkDevice device, VkSurfaceKHR surface, VkSurfaceFormatKHR surface_format, VkPresentModeKHR present_mode, VkExtent2D extent, uint32_t min_image_count, VkSwapchainKHR old_swapchain = VK_NULL_HANDLE);

        /// @brief Create swapchain `FramesInFlight` objects
        /// @return Whether the frames in flight have been successfully created
        bool CreateSwapchainFramesInFlight();

        /// @brief Create Vulkan's `VkImageView` object from a given `VkImage`
        /// @param device Logical device on which to create the image view
        /// @param format Image view format
        /// @param image Image handle
        /// @return `VkImageView` handle or `VK_NULL_HANDLE` on failure
        VkImageView CreateVulkanImageView(VkDevice device, VkFormat format, VkImage image);

        /// @brief Create Vulkan's `VkFramebuffer` object
        /// @param device  Logical device on which to create the framebuffer
        /// @param render_pass Framebuffer's render pass
        /// @param extent Framebuffer extent
        /// @param image_view A single frame buffer attachment
        /// @return `VkFramebuffer` handle or `VK_NULL_HANDLE` on failure
        VkFramebuffer CreateVulkanFramebuffer(VkDevice device, VkRenderPass render_pass, const VkExtent2D& extent, VkImageView image_view);


        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// Runtime functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        /// @brief Submit draw commands for render
        /// @param draw_data Dear ImGui draw data
        /// @return Whether the swapchain has been invalidated and should be rebuild
        bool FrameRender(ImDrawData* draw_data);

        /// @brief Present the next available frame-in-flight to the GLFW window
        /// @return Whether the swapchain is invalidated and should be rebuild
        bool FramePresent();

        /// @brief Recreate the swapchain with extent of given size 
        /// @param width Swapchain image extent width 
        /// @param height Swapchain image extent height
        void WindowResize(uint32_t width, uint32_t height);


        VkSampler CreateVulkanSampler(VkDevice device, ImageSampler sampler);

        /// @brief Get a Vulkan sampler from a given `Backend::ImageSampler`
        /// @param sampler Backend module image sampler type
        /// @return Vulkan's `VkSampler` handle 
        VkSampler GetVulkanSampler(ImageSampler sampler);

        /// @brief Wait for the GPU and destroy all allocated members
        /// @note It is safe to call this function without prior initialization
        void Cleanup();

    } // namespace Backend
} // namespace yart
