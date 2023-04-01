#include "application.h"

#include <algorithm>
#include <iostream>

#include "utils/vk_utils.h"


#define ASSERT_HANDLE_INIT(handle, err)     \
    if (handle == VK_NULL_HANDLE) {         \
        std::cerr << err << std::endl;      \
        return 0;                           \
    }

// Helper macro to help locate all VkAllocationCallbacks dependencies
#define DEFAULT_VK_ALLOC VK_NULL_HANDLE

#define WINDOW_TITLE "Yet Another Ray Tracer"
#define MIN_IMAGE_COUNT 2


// TODO: error logging
static void on_glfw_error(int error_code, const char *description) 
{
    std::cerr << "GLFW Error " << error_code << ": " << description << std::endl;
}

static void on_glfw_window_close(GLFWwindow* window)
{
    YART_UNUSED(window);
    yart::Application::Get().Shutdown();    
}

// TODO: Proper vk debug logging
#ifdef YART_VULKAN_DEBUG_UTILS
static VkBool32 VKAPI_PTR on_vulkan_debug_message(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    VkDebugUtilsMessageTypeFlagsEXT msg_type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* user_data) 
{
    YART_UNUSED(msg_severity, msg_type, user_data);

    std::cout << "[VK DEBUG]: " << data->pMessage << std::endl;
    return VK_FALSE;
}
#endif

static yart::Application* s_instance = nullptr;


namespace yart
{
    Application::Application(int win_w, int win_h) 
    {
        s_instance = this;
        if (!InitGLFW(win_w, win_h))
            return;

        if (!InitVulkan())
            return;

        if (!InitImGUI())
            return;

        m_initialized = true;
    }

    Application::~Application() 
    {
        s_instance = nullptr;
        Cleanup();
    }

    Application& Application::Get()
    {
        // TODO: assert s_instance
        return *s_instance;
    }

    int Application::Run() 
    {
        if (!m_initialized) 
            return EXIT_FAILURE;

        // TODO: assert not m_running
        m_running = true; // Directly controlled by Application::Close() and on_glfw_window_close

        bool rebuild_swapchain = false;
        while (m_running) {
            // Poll and handle incoming events
            glfwPollEvents();

            // Resize the swapchain if previously invalidated  
            if (rebuild_swapchain) {
                int win_w, win_h;
                glfwGetFramebufferSize(m_window, &win_w, &win_h);

                // Don't rebuild if window is minimized
                if (!(win_w && win_h))
                    continue; 

                WindowResize(static_cast<uint32_t>(win_w), static_cast<uint32_t>(win_h));
                rebuild_swapchain = false;
            }

            // Begin an ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // TODO: ImGui render commands 

            // Finalize frame and retrieve render commands from ImGui
            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();

            rebuild_swapchain |= FrameRender(draw_data);
            if (!rebuild_swapchain){
                rebuild_swapchain |= FramePresent();
            }
        }

        return EXIT_SUCCESS;
    }

    void Application::Shutdown()
    {
        m_running = false;
    }

    int Application::InitGLFW(int win_w, int win_h) 
    {
        glfwSetErrorCallback(on_glfw_error);
        if (!glfwInit()) {
            return 0;
        }

        if (!glfwVulkanSupported()) {
            fprintf(stderr, "GLFW: Vulkan Not Supported\n");
            return 0;
        }

        // Create window with Vulkan context
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(win_w, win_h, WINDOW_TITLE, NULL, NULL);
        if (m_window == NULL) {
            return 0;
        }

        // Set custom GLFW event callbacks
        glfwSetWindowCloseCallback(m_window, on_glfw_window_close);

        return 1;
    }

    int Application::InitVulkan() 
    {
        VkResult res;

        auto exts = GetRequiredVulkanExtensions();

        m_vkInstance = CreateVulkanInstance(exts);
        ASSERT_HANDLE_INIT(m_vkInstance, "Failed to create Vulkan instance");

        m_ltStack.Push<VkInstance>(m_vkInstance, [](auto var){ 
            vkDestroyInstance(var, DEFAULT_VK_ALLOC);
        });

    #ifdef YART_VULKAN_DEBUG_UTILS
        auto debug_messenger = CreateVulkanDebugMessenger(m_vkInstance, on_vulkan_debug_message);
        ASSERT_HANDLE_INIT(debug_messenger, "Failed to create Vulkan debug messenger");

        m_ltStack.Push<VkDebugUtilsMessengerEXT>(debug_messenger, [&](auto var){ 
            LOAD_VK_INSTANCE_FP(m_vkInstance, vkDestroyDebugUtilsMessengerEXT);
            vkDestroyDebugUtilsMessengerEXT(m_vkInstance, var, DEFAULT_VK_ALLOC);
        });
    #endif

        // Create a Vulkan surface for the main GLFW window
        VkSurfaceKHR surface;
        res = glfwCreateWindowSurface(m_vkInstance, m_window, DEFAULT_VK_ALLOC, &surface);
        CHECK_VK_RESULT_RETURN(res, 0);

        m_ltStack.Push<VkSurfaceKHR>(surface, [&](auto var){ 
            vkDestroySurfaceKHR(m_vkInstance, var, DEFAULT_VK_ALLOC);
        });

        // Query a physical device from client machine, ideally a discrete GPU
        VkPhysicalDeviceProperties gpu_properties = { };
        m_vkPhysicalDevice = SelectVulkanPhysicalDevice(m_vkInstance, gpu_properties);
        ASSERT_HANDLE_INIT(m_vkPhysicalDevice, "Failed to locate a physical Vulkan device");

        // Make sure the physical device supports the `VK_KHR_swapchain` extension 
        std::vector<const char*> swapchain_ext = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        if (utils::CheckVulkanDeviceExtensionsAvailable(m_vkPhysicalDevice, swapchain_ext) >= 0) {
            std::cout << "GPU does not support swapchain operations" << std::endl;
            return 0;
        }

        // Select queue family index from the gpu with support for graphics and surface presentation (WSI)
        if (!GetVulkanQueueFamilyIndex(m_vkPhysicalDevice, &m_queueFamily, VK_QUEUE_GRAPHICS_BIT, surface)) {
            std::cerr << "No queue family with graphics and presentation support found on GPU" << std::endl;
            return 0;
        }

        // Create a VkDevice with a single queue and `VK_KHR_swapchain` extension 
        m_vkDevice = CreateVulkanLogicalDevice(m_vkPhysicalDevice, m_queueFamily, swapchain_ext);
        ASSERT_HANDLE_INIT(m_vkDevice, "Failed to create Vulkan device");

        m_ltStack.Push<VkDevice>(m_vkDevice, [](auto var){ 
            vkDestroyDevice(var, DEFAULT_VK_ALLOC);
        });

        // Extract the graphics queue from the logical device
        vkGetDeviceQueue(m_vkDevice, m_queueFamily, 0, &m_vkQueue);
        ASSERT_HANDLE_INIT(m_vkQueue, "Failed to retrieve graphics queue from Vulkan device");

        // Create a Vulkan descriptor pool used by ImGui 
        m_vkDescriptorPool = CreateVulkanDescriptorPool(m_vkDevice);
        ASSERT_HANDLE_INIT(m_vkDescriptorPool, "Failed to create Vulkan Descriptor pool");

        m_ltStack.Push<VkDescriptorPool>(m_vkDescriptorPool, [&](auto var){ 
            vkDestroyDescriptorPool(m_vkDevice, var, DEFAULT_VK_ALLOC);
        });

        // Create the initial swapchain
        if (!InitializeSwapchain(surface)) {
            std::cerr << "Failed to initialize swapchain" << std::endl;
            return 0;
        }

        return 1;
    }

    std::vector<const char*> Application::GetRequiredVulkanExtensions()
    {
        std::vector<const char*> exts;

        // Query Vulkan extensions required by GLFW
        uint32_t glfw_ext_count = 0;
        const char** glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_ext_count);
        if (glfw_exts == NULL)
            return exts;

        exts.reserve(glfw_ext_count);
        for (size_t i = 0; i < glfw_ext_count; ++i)
            exts.push_back(glfw_exts[i]);

    #ifdef YART_VULKAN_DEBUG_UTILS
        // Vulkan debug utils additionally require VK_EXT_debug_utils
        exts.push_back("VK_EXT_debug_utils");
    #endif

        return exts;
    }

    VkInstance Application::CreateVulkanInstance(std::vector<const char*>& extensions)
    {
        VkInstance instance = VK_NULL_HANDLE;

        int err = utils::CheckVulkanInstanceExtensionsAvailable(extensions);
        if (err >= 0) {
            std::cout << extensions[(size_t)err] << " extension is not available" << std::endl;
            return VK_NULL_HANDLE;
        }

        VkInstanceCreateInfo instance_ci = {};
        instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instance_ci.ppEnabledExtensionNames = extensions.data();

    #ifdef YART_VULKAN_DEBUG_UTILS
        // Enable Vulkan validation layers for debug reporting features 
        const char* layers[1] = { "VK_LAYER_KHRONOS_validation" };
        instance_ci.ppEnabledLayerNames = layers;
        instance_ci.enabledLayerCount = 1;
    #endif

        VkResult res = vkCreateInstance(&instance_ci, DEFAULT_VK_ALLOC, &instance);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return instance; 
    }

    VkDebugUtilsMessengerEXT Application::CreateVulkanDebugMessenger(VkInstance instance, vk_debug_callback_t callback)
    {
        // Create a single all-purpose debug messenger object  
        VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

        constexpr VkDebugUtilsMessageSeverityFlagsEXT message_severity_flag =
          //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        constexpr VkDebugUtilsMessageTypeFlagsEXT message_type_flag =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     | 
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;

        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_ci = {};
        debug_messenger_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_messenger_ci.pfnUserCallback = callback;
        debug_messenger_ci.messageSeverity = message_severity_flag;
        debug_messenger_ci.messageType = message_type_flag;

        // Extension function pointers need to be retrieved from the VkInstance
        LOAD_VK_INSTANCE_FP(instance, vkCreateDebugUtilsMessengerEXT);
        VkResult res = vkCreateDebugUtilsMessengerEXT(instance, &debug_messenger_ci, DEFAULT_VK_ALLOC, &debug_messenger);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return debug_messenger; 
    }

    /* Returns the first discrete GPU found on client machine, or the very first available one */
    VkPhysicalDevice Application::SelectVulkanPhysicalDevice(VkInstance instance, VkPhysicalDeviceProperties& properties)
    {
        VkPhysicalDevice gpu = VK_NULL_HANDLE; 

        uint32_t gpu_count;
        VkResult res = vkEnumeratePhysicalDevices(instance, &gpu_count, NULL);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        VkPhysicalDevice* gpus = new VkPhysicalDevice[gpu_count];
        res = vkEnumeratePhysicalDevices(instance, &gpu_count, gpus);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        VkPhysicalDeviceProperties props = {};
        for (size_t i = 0; i < (size_t)gpu_count; ++i) {
            vkGetPhysicalDeviceProperties(gpus[i], &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                properties = props;
                gpu = gpus[i];

                delete[] gpus;
                return gpu;
            }
        }

        vkGetPhysicalDeviceProperties(gpus[0], &props);
        properties = props;
        gpu = gpus[0];

        delete[] gpus;
        return gpu;
    }

    bool Application::GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags, VkSurfaceKHR surface)
    {
        uint32_t queue_count;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, NULL);

        auto queues = std::make_unique<VkQueueFamilyProperties[]>(queue_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, queues.get());

        for (uint32_t i = 0; i < queue_count; i++) {
            if (queues[i].queueFlags & flags) {
                // Queue family additionally requires a surface presentation support 
                if (surface != VK_NULL_HANDLE) {
                    // Check for Windowing System Integration support on GPU queue family
                    VkBool32 wsi_support;
                    VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &wsi_support);
                    CHECK_VK_RESULT_RETURN(res, false);
                    if (wsi_support != VK_TRUE)
                        continue;
                }

                *result = i;
                return true;
            }
        }

        return false;
    }

    VkDevice Application::CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family, const std::vector<const char*>& extensions)
    {
        VkDevice device = VK_NULL_HANDLE;

        int err = utils::CheckVulkanDeviceExtensionsAvailable(physical_device, extensions);
        if (err >= 0) {
            std::cout << extensions[(size_t)err] << " device extension is not available" << std::endl;
            return VK_NULL_HANDLE;
        }

        const float queue_priority[] = { 1.0f };

        VkDeviceQueueCreateInfo queue_ci[1] = {};
        queue_ci[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci[0].pQueuePriorities = queue_priority;
        queue_ci[0].queueFamilyIndex = queue_family;
        queue_ci[0].queueCount = 1;

        VkDeviceCreateInfo device_ci = {};
        device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        device_ci.ppEnabledExtensionNames = extensions.data();
        device_ci.pQueueCreateInfos = queue_ci;
        device_ci.queueCreateInfoCount = 1;

        VkResult res = vkCreateDevice(physical_device, &device_ci, DEFAULT_VK_ALLOC, &device);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return device;
    }

    VkDescriptorPool Application::CreateVulkanDescriptorPool(VkDevice device)
    {
        VkDescriptorPool pool = VK_NULL_HANDLE;

        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 16 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 16 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 16 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 16 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 16 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 16 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 16 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 16 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 16 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 16 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 16 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // allow for descriptor sets allocated from the pool to be individually freed back to the pool
        pool_info.maxSets = static_cast<uint32_t>(16 * YART_ARRAYSIZE(pool_sizes));
        pool_info.poolSizeCount = static_cast<uint32_t>(YART_ARRAYSIZE(pool_sizes));
        pool_info.pPoolSizes = pool_sizes;

        VkResult res = vkCreateDescriptorPool(device, &pool_info, DEFAULT_VK_ALLOC, &pool);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return pool;
    }

    bool Application::InitializeSwapchain(VkSurfaceKHR surface)
    {
        VkResult res;

        // Initialize m_swapchainData members
        m_swapchainData.surface = surface;

        // Select an available surface format (preferably (VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
        m_swapchainData.surface_format = utils::RequestVulkanSurfaceFormat(m_vkPhysicalDevice, surface, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
 
        // Select surface presentation mode
        VkPresentModeKHR preferred_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
        m_swapchainData.present_mode = utils::RequestVulkanSurfacePresentMode(m_vkPhysicalDevice, surface, preferred_present_mode);

        VkSurfaceCapabilitiesKHR surface_capabilities = {};
        res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, surface, &surface_capabilities);
        CHECK_VK_RESULT_RETURN(res, false);

        // NOTE: 
        // min and max image count variables are set just once here and reused throughout the application runtime for swapchain rebuilds
        // in case of the capabilities not being constant - these should probably be retrieved and set every rebuild, but I can't find much info of such possibility 
        uint32_t min_img_count = utils::GetMinImageCountFromPresentMode(m_swapchainData.present_mode);
        m_swapchainData.min_image_count = std::max<uint32_t>(min_img_count, surface_capabilities.minImageCount);

        m_swapchainData.max_image_count = surface_capabilities.maxImageCount;
        if (surface_capabilities.maxImageCount != 0) // maxImageCount = 0 means there is no maximum
            m_swapchainData.min_image_count = std::min<uint32_t>(m_swapchainData.min_image_count, m_swapchainData.max_image_count);

        // Create a Vulkan render pass with a single subpass
        m_vkRenderPass = CreateVulkanRenderPass(m_vkDevice, m_swapchainData);
        ASSERT_HANDLE_INIT(m_vkRenderPass, "Failed to create Vulkan render pass");

        m_ltStack.Push<VkRenderPass>(m_vkRenderPass, [&](VkRenderPass var){
            vkDestroyRenderPass(m_vkDevice, var, DEFAULT_VK_ALLOC);
        });

        // Create initial Vulkan swapchain 
        m_vkSwapchain = CreateVulkanSwapchain(m_vkDevice, m_swapchainData, surface_capabilities.currentExtent);
        ASSERT_HANDLE_INIT(m_vkSwapchain, "Failed to create Vulkan swapchain");

        // Create frame in flight objects 
        if (!CreateSwapchainFramesInFlight(surface_capabilities.currentExtent)) {
            std::cerr << "Failed to create swapchain frames in flight" << std::endl;
            return false;
        }
        
        return true;
    }

    VkSwapchainKHR Application::CreateVulkanSwapchain(VkDevice device, const SwapchainData& data, VkExtent2D& current_extent, VkSwapchainKHR old_swapchain)
    {
        YART_ASSERT(data.surface != VK_NULL_HANDLE);
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        VkSwapchainCreateInfoKHR swapchain_ci = {};
        swapchain_ci.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_ci.surface          = data.surface;
        swapchain_ci.imageFormat      = data.surface_format.format;
        swapchain_ci.imageColorSpace  = data.surface_format.colorSpace;
        swapchain_ci.presentMode      = data.present_mode;
        swapchain_ci.imageExtent      = current_extent;
        swapchain_ci.minImageCount    = data.min_image_count;
        swapchain_ci.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Only in case when graphics family = present family
        swapchain_ci.imageArrayLayers = 1;
        swapchain_ci.preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_ci.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_ci.oldSwapchain     = old_swapchain;
        swapchain_ci.clipped          = VK_TRUE;

        VkResult res = vkCreateSwapchainKHR(device, &swapchain_ci, DEFAULT_VK_ALLOC, &swapchain);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return swapchain; 
    }

    bool Application::CreateSwapchainFramesInFlight(VkExtent2D& current_extent)
    {
        VkResult res;

        // Query the swapchain image count and the initial set of images
        res = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &m_swapchainData.image_count, nullptr);
        CHECK_VK_RESULT_RETURN(res, false);

        auto images = std::make_unique<VkImage[]>(m_swapchainData.image_count);
        res = vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &m_swapchainData.image_count, images.get());
        CHECK_VK_RESULT_RETURN(res, false);

        // Create image views for each swapchain image
        auto image_views = std::make_unique<VkImageView[]>(m_swapchainData.image_count);
        for (uint32_t i = 0; i < m_swapchainData.image_count; ++i) {
            image_views[i] = CreateVulkanImageView(m_vkDevice, m_swapchainData.surface_format.format, images[i]);
            ASSERT_HANDLE_INIT(image_views[i], "Failed to create swapchain image view");

            m_swapchainData.ltStack.Push<VkImageView>(image_views[i], [&](VkImageView var){
                vkDestroyImageView(m_vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Create framebuffers for each image view
        auto frame_buffers = std::make_unique<VkFramebuffer[]>(m_swapchainData.image_count);
        for (uint32_t i = 0; i < m_swapchainData.image_count; ++i) {
            frame_buffers[i] = CreateVulkanFramebuffer(m_vkDevice, m_vkRenderPass, current_extent, image_views[i]);
            ASSERT_HANDLE_INIT(frame_buffers[i], "Failed to create swapchain framebuffer");

            m_swapchainData.ltStack.Push<VkFramebuffer>(frame_buffers[i], [&](VkFramebuffer var){
                vkDestroyFramebuffer(m_vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Create a Vulkan command pool for each frame
        m_swapchainData.vk_command_pools = std::make_unique<VkCommandPool[]>(m_swapchainData.image_count);

        VkCommandPoolCreateInfo pool_ci = {};
        pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_ci.queueFamilyIndex = m_queueFamily;

        for (uint32_t i = 0; i < m_swapchainData.image_count; ++i) {
            res = vkCreateCommandPool(m_vkDevice, &pool_ci, DEFAULT_VK_ALLOC, &m_swapchainData.vk_command_pools[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            m_swapchainData.ltStack.Push<VkCommandPool>(m_swapchainData.vk_command_pools[i], [&](VkCommandPool var){
                vkDestroyCommandPool(m_vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Allocate Vulkan command buffers from the pool for each frame
        m_swapchainData.vk_command_buffers = std::make_unique<VkCommandBuffer[]>(m_swapchainData.image_count);

        VkCommandBufferAllocateInfo buffer_ai = {};
        buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        buffer_ai.commandBufferCount = 1;

        for (uint32_t i = 0; i < m_swapchainData.image_count; ++i) {
            buffer_ai.commandPool = m_swapchainData.vk_command_pools[i];

            res = vkAllocateCommandBuffers(m_vkDevice, &buffer_ai, &m_swapchainData.vk_command_buffers[i]);
            CHECK_VK_RESULT_RETURN(res, false);
            // Vulkan command buffers get released automatically with the destruction of the pools they were allocated from
        }

        // Create Vulkan semaphores for each frame 
        m_swapchainData.vk_image_acquired_semaphore = std::make_unique<VkSemaphore[]>(m_swapchainData.image_count);
        m_swapchainData.vk_render_complete_semaphore = std::make_unique<VkSemaphore[]>(m_swapchainData.image_count);

        VkSemaphoreCreateInfo semaphore_ci = {};
        semaphore_ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (uint32_t i = 0; i < m_swapchainData.image_count; ++i) {
            // Image Acquired Semaphore
            res = vkCreateSemaphore(m_vkDevice, &semaphore_ci, DEFAULT_VK_ALLOC, &m_swapchainData.vk_image_acquired_semaphore[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            m_swapchainData.ltStack.Push<VkSemaphore>(m_swapchainData.vk_image_acquired_semaphore[i], [&](VkSemaphore var){
                vkDestroySemaphore(m_vkDevice, var, DEFAULT_VK_ALLOC);
            });

            // Render Complete Semaphore
            res = vkCreateSemaphore(m_vkDevice, &semaphore_ci, DEFAULT_VK_ALLOC, &m_swapchainData.vk_render_complete_semaphore[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            m_swapchainData.ltStack.Push<VkSemaphore>(m_swapchainData.vk_render_complete_semaphore[i], [&](VkSemaphore var){
                vkDestroySemaphore(m_vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Create Vulkan fences for each frame 
        m_swapchainData.vk_fences = std::make_unique<VkFence[]>(m_swapchainData.image_count);

        VkFenceCreateInfo fence_ci = {};
        fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < m_swapchainData.image_count; ++i) {
            res = vkCreateFence(m_vkDevice, &fence_ci, DEFAULT_VK_ALLOC, &m_swapchainData.vk_fences[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            m_swapchainData.ltStack.Push<VkFence>(m_swapchainData.vk_fences[i], [&](VkFence var){
                vkDestroyFence(m_vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        return true;
    }

    VkRenderPass Application::CreateVulkanRenderPass(VkDevice device, const SwapchainData& data)
    {
        YART_ASSERT(data.surface != VK_NULL_HANDLE);
        VkRenderPass render_pass = VK_NULL_HANDLE;

        VkAttachmentDescription attachment = {};
        attachment.format         = data.surface_format.format;
        attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // VK_ATTACHMENT_LOAD_OP_CLEAR
        attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment = {};
        color_attachment.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachment.attachment = 0;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments    = &color_attachment;
        subpass.colorAttachmentCount = 1;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_ci = {};
        render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_ci.pAttachments    = &attachment;
        render_pass_ci.attachmentCount = 1;
        render_pass_ci.pSubpasses      = &subpass;
        render_pass_ci.subpassCount    = 1;
        render_pass_ci.pDependencies   = &dependency;
        render_pass_ci.dependencyCount = 1;

        VkResult res = vkCreateRenderPass(device, &render_pass_ci, DEFAULT_VK_ALLOC, &render_pass);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return render_pass;
    }

    VkImageView Application::CreateVulkanImageView(VkDevice device, VkFormat format, VkImage image)
    {
        VkImageView view = VK_NULL_HANDLE;

        VkImageViewCreateInfo view_ci = {}; 
        view_ci.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_ci.format   = format;
        view_ci.image    = image;
        view_ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_ci.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        view_ci.subresourceRange.baseMipLevel   = 0;
        view_ci.subresourceRange.levelCount     = 1;
        view_ci.subresourceRange.baseArrayLayer = 0;
        view_ci.subresourceRange.layerCount     = 1;

        VkResult res = vkCreateImageView(device, &view_ci, DEFAULT_VK_ALLOC, &view);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return view;
    }

    VkFramebuffer Application::CreateVulkanFramebuffer(VkDevice device, VkRenderPass render_pass, VkExtent2D& extent, VkImageView image_view)
    {
        VkFramebuffer frame_buffer = VK_NULL_HANDLE;

        VkFramebufferCreateInfo frame_buffer_ci = {};
        frame_buffer_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frame_buffer_ci.renderPass = render_pass;
        frame_buffer_ci.width = extent.width;
        frame_buffer_ci.height = extent.height;
        frame_buffer_ci.pAttachments = &image_view;
        frame_buffer_ci.attachmentCount = 1;
        frame_buffer_ci.layers = 1;

        VkResult res = vkCreateFramebuffer(device, &frame_buffer_ci, DEFAULT_VK_ALLOC, &frame_buffer);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return frame_buffer;
    }

    bool Application::InitImGUI()
    {
        VkResult res;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls

        // Set ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(m_window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance       = m_vkInstance;
        init_info.PhysicalDevice = m_vkPhysicalDevice;
        init_info.Device         = m_vkDevice;
        init_info.Queue          = m_vkQueue;
        init_info.QueueFamily    = m_queueFamily;
        init_info.DescriptorPool = m_vkDescriptorPool;
        init_info.MinImageCount  = m_swapchainData.min_image_count;
        init_info.ImageCount     = m_swapchainData.image_count;
        init_info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;
        // init_info.CheckVkResultFn = check_vk_result;

        // Create ImGui render pipeline
        ImGui_ImplVulkan_Init(&init_info, m_vkRenderPass);

        // Upload fonts
        VkCommandPool command_pool = m_swapchainData.vk_command_pools[0];       // |
        VkCommandBuffer command_buffer = m_swapchainData.vk_command_buffers[0]; // | use whichever command queue

        res = vkResetCommandPool(m_vkDevice, command_pool, (VkCommandPoolResetFlags)0);
        CHECK_VK_RESULT_RETURN(res, false); 

        VkCommandBufferBeginInfo cmd_begin_info = {}; 
        cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        res = vkBeginCommandBuffer(command_buffer, &cmd_begin_info);
        CHECK_VK_RESULT_RETURN(res, false); 

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pCommandBuffers = &command_buffer;
        submit_info.commandBufferCount = 1;

        res = vkEndCommandBuffer(command_buffer);
        CHECK_VK_RESULT_RETURN(res, false); 

        res = vkQueueSubmit(m_vkQueue, 1, &submit_info, VK_NULL_HANDLE);
        CHECK_VK_RESULT_RETURN(res, false); 

        res = vkDeviceWaitIdle(m_vkDevice);
        CHECK_VK_RESULT_RETURN(res, false); 

        ImGui_ImplVulkan_DestroyFontUploadObjects();

        return true;
    }

    void Application::WindowResize(uint32_t width, uint32_t height)
    {
        // Wait for the GPU to finish execution 
        VkResult res = vkDeviceWaitIdle(m_vkDevice);
        CHECK_VK_RESULT_ABORT(res);

        // Release all swapchain related objects
        m_swapchainData.ltStack.Release();

        // ImGui_ImplVulkan_SetMinImageCount(m_swapchainData.min_image_count); // NOTE: min_image_count is kept constant throughout the application life span

        // Recreate Vulkan swapchain
        VkExtent2D current_extent = { width, height };
        VkSwapchainKHR old_swapchain = m_vkSwapchain;
        m_vkSwapchain = CreateVulkanSwapchain(m_vkDevice, m_swapchainData, current_extent, old_swapchain);
        YART_ASSERT(m_vkSwapchain != VK_NULL_HANDLE);

        // Release previous Vulkan swapchain
        vkDestroySwapchainKHR(m_vkDevice, old_swapchain, DEFAULT_VK_ALLOC);

        // Create frame in flight objects 
        if (!CreateSwapchainFramesInFlight(current_extent)) {
            YART_ABORT("Failed to create swapchain frames in flight");
        }

        // g_MainWindowData.FrameIndex = 0;
    }

    bool Application::FrameRender(ImDrawData* draw_data)
    {
        // VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
        // VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        // VkResult res = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
        // if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
        //     return true;
        // }
        // CHECK_VK_RESULT_RETURN(err, ???);

        // Wait for frame fences 

        // Reset command pool

        // Begin command buffer

        // Begin render pass

        // Record dear imgui primitives into command buffer
        // ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);

        // Submit command buffer

        // End command buffer

        // Submit queue 

        return false;
    }

    bool Application::FramePresent()
    {
        return false;
    }

    void Application::Cleanup()
    {
        // Wait for the GPU to finish execution 
        VkResult res = vkDeviceWaitIdle(m_vkDevice);
        CHECK_VK_RESULT_ABORT(res);

        // Release all swapchain related objects
        m_swapchainData.ltStack.Release();
        if (m_vkSwapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(m_vkDevice, m_vkSwapchain, DEFAULT_VK_ALLOC);

        // Release ImGui pipeline objects
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Unwind all allocations from the LTStack
        m_ltStack.Release();

        // Quit GLFW
        glfwDestroyWindow(m_window);
        glfwTerminate();

        m_initialized = false;
    }
} // namespace yart
