#include "backend_impl_vulkan.h"


#include <cstdio>
#include <cstring>

#include "yart/backend/utils/vk_utils.h"


// TODO: Proper Vulkan debug logging
#ifdef YART_VULKAN_DEBUG_UTILS
static VkBool32 VKAPI_PTR on_vulkan_debug_message(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity, VkDebugUtilsMessageTypeFlagsEXT msg_type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data) 
{
    YART_UNUSED(msg_severity, msg_type, user_data);

    YART_LOG_ERR("[VK DEBUG]: %s\n", data->pMessage);
    return VK_FALSE;
}
#endif

/// @brief GLFW error event callback function
/// @param error_code GLFW error code
/// @param description Error description provided by GLFW
static void on_glfw_error(int error_code, const char* description) 
{
    YART_LOG_ERR("GLFW Error (%d): %s\n", error_code, description);
}

/// @brief GLFW window close event callback function
/// @param window The closed window instance
static void on_glfw_window_close(GLFWwindow* window)
{
    YART_UNUSED(window);
    yart::Backend::BackendContext& ctx = yart::Backend::GetBackendContext();
    if (ctx.onWindowCloseCallback)
        ctx.onWindowCloseCallback();
}


namespace yart
{
    bool Backend::Init(const char* window_title, uint32_t window_width, uint32_t window_height)
    {
        if (!InitGLFW(window_title, window_width, window_height)) {
            YART_LOG_ERR("Failed to initialize GLFW\n");
            return false;
        }

        if (!InitVulkan()) {
            YART_LOG_ERR("Failed to initialize Vulkan\n");
            return false;
        }

        if (!InitImGui()) {
            YART_LOG_ERR("Failed to initialize Dear ImGui\n");
            return false;
        }

        return true;
    }

    void Backend::SetDearImGuiSetupCallback(event_callback_t callback)
    {
        BackendContext& ctx = GetBackendContext();
        ctx.onDearImGuiSetupCallback = callback;
    }

    void Backend::SetRenderCallback(event_callback_t callback)
    {
        BackendContext& ctx = GetBackendContext();
        ctx.onRenderCallback = callback;
    }

    void Backend::SetWindowCloseCallback(event_callback_t callback)
    {
        BackendContext& ctx = GetBackendContext();
        ctx.onWindowCloseCallback = callback;
    }
    
    void Backend::PollEvents()
    {
        glfwPollEvents();
    }

    void Backend::Render()
    {
        BackendContext& ctx = GetBackendContext();

        // Begin a new Dear ImGui frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Issue application-defined render commands 
        if (ctx.onRenderCallback)
            ctx.onRenderCallback();

        // Render viewport image using ImGui
        // ImDrawList* bg_draw_list = ImGui::GetBackgroundDrawList();
        // m_viewport->Render(bg_draw_list);

        // Finalize ImGui frame and retrieve the render commands
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        // Render and present the frame to a platform window
        if (!ctx.shouldRebuildSwapchain) {
            ctx.shouldRebuildSwapchain = FrameRender(draw_data);

            if (!ctx.shouldRebuildSwapchain)
                ctx.shouldRebuildSwapchain = FramePresent();
        }
        
        // Resize the swapchain if invalidated  
        if (ctx.shouldRebuildSwapchain) {
            int win_w, win_h;
            glfwGetFramebufferSize(ctx.window, &win_w, &win_h);

            // Don't render/rebuild if window is minimized
            if (!(win_w && win_h))
                return; 

            WindowResize(static_cast<uint32_t>(win_w), static_cast<uint32_t>(win_h));
            ctx.shouldRebuildSwapchain = false;
        }
    }

    void Backend::Close()
    {
        // Clear the context data
        BackendContext& ctx = GetBackendContext();
        memset(&ctx, 0, sizeof(BackendContext));
    }

    Backend::BackendContext& Backend::GetBackendContext()
    {
        static BackendContext context = { }; // Lazily instantiated on first call
        return context;
    }

    bool Backend::InitGLFW(const char *window_title, uint32_t window_width, uint32_t window_height)
    {
        // Error callback can be set prior to calling `glfwInit`
        glfwSetErrorCallback(on_glfw_error);

        if (!glfwInit()) {
            YART_LOG_ERR("GLFW: glfwInit() exited with an error\n");
            return false;
        }

        if (!glfwVulkanSupported()) {
            YART_LOG_ERR("GLFW: Vulkan not supported\n");
            return false;
        }

        // Create window with Vulkan context
        BackendContext& ctx = GetBackendContext();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        ctx.window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
        if (ctx.window == NULL) {
            YART_LOG_ERR("GLFW: Failed to create window\n");
            return false;
        }

        // Set custom GLFW event callbacks
        glfwSetWindowCloseCallback(ctx.window, on_glfw_window_close);

        return true;
    }

    bool Backend::InitVulkan()
    {
        BackendContext& ctx = GetBackendContext();
        VkResult res;

        // Create a Vulkan instance handle
        std::vector<vk_extension_t> instance_exts = GetRequiredVulkanExtensions();
        ctx.vkInstance = CreateVulkanInstance(instance_exts);
        ASSERT_VK_HANDLE_INIT(ctx.vkInstance, "VULKAN: Failed to create Vulkan instance\n");

        ctx.LT.Push<VkInstance>(ctx.vkInstance, [](auto var){ 
            vkDestroyInstance(var, DEFAULT_VK_ALLOC);
        });

    #ifdef YART_VULKAN_DEBUG_UTILS
        auto debug_messenger = CreateVulkanDebugMessenger(ctx.vkInstance, on_vulkan_debug_message);
        ASSERT_VK_HANDLE_INIT(debug_messenger, "VULKAN: Failed to create Vulkan debug messenger\n");

        ctx.LT.Push<VkDebugUtilsMessengerEXT>(debug_messenger, [&](auto var){ 
            LOAD_VK_INSTANCE_FP(ctx.vkInstance, vkDestroyDebugUtilsMessengerEXT);
            vkDestroyDebugUtilsMessengerEXT(ctx.vkInstance, var, DEFAULT_VK_ALLOC);
        });
    #endif

        // Create a Vulkan surface for the main GLFW window
        res = glfwCreateWindowSurface(ctx.vkInstance, ctx.window, DEFAULT_VK_ALLOC, &ctx.vkSurface);
        CHECK_VK_RESULT_RETURN(res, false);

        ctx.LT.Push<VkSurfaceKHR>(ctx.vkSurface, [&](auto var){ 
            vkDestroySurfaceKHR(ctx.vkInstance, var, DEFAULT_VK_ALLOC);
        });

        // Query a physical device from client machine, ideally a discrete GPU
        VkPhysicalDeviceProperties gpu_properties = { };
        ctx.vkPhysicalDevice = SelectVulkanPhysicalDevice(ctx.vkInstance, gpu_properties);
        ASSERT_VK_HANDLE_INIT(ctx.vkPhysicalDevice, "VULKAN: Failed to locate a physical Vulkan device\n");

        // Make sure the physical device supports the `VK_KHR_swapchain` extension 
        std::vector<vk_extension_t> swapchain_ext = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        if (utils::CheckVulkanDeviceExtensionsAvailable(ctx.vkPhysicalDevice, swapchain_ext) >= 0) {
            YART_LOG_ERR("VULKAN: GPU does not support swapchain operations\n");
            return false;
        }

        // Select queue family index from the GPU with support for graphics and surface presentation (WSI)
        if (!GetVulkanQueueFamilyIndex(ctx.vkPhysicalDevice, &ctx.vkQueueFamily, VK_QUEUE_GRAPHICS_BIT, ctx.vkSurface)) {
            YART_LOG_ERR("VULKAN: No queue family with graphics and presentation support found on GPU\n");
            return false;
        }

        // Create a VkDevice with a single queue and `VK_KHR_swapchain` extension 
        ctx.vkDevice = CreateVulkanLogicalDevice(ctx.vkPhysicalDevice, ctx.vkQueueFamily, swapchain_ext);
        ASSERT_VK_HANDLE_INIT(ctx.vkDevice, "VULKAN: Failed to create Vulkan device\n");

        ctx.LT.Push<VkDevice>(ctx.vkDevice, [](auto var){ 
            vkDestroyDevice(var, DEFAULT_VK_ALLOC);
        });

        // Extract the graphics queue from the logical device
        vkGetDeviceQueue(ctx.vkDevice, ctx.vkQueueFamily, 0, &ctx.vkQueue);
        ASSERT_VK_HANDLE_INIT(ctx.vkQueue, "VULKAN: Failed to retrieve graphics queue from Vulkan device\n");

        // Create a Vulkan descriptor pool used by ImGui 
        ctx.vkDescriptorPool = CreateVulkanDescriptorPool(ctx.vkDevice);
        ASSERT_VK_HANDLE_INIT(ctx.vkDescriptorPool, "VULKAN: Failed to create Vulkan Descriptor pool\n");

        ctx.LT.Push<VkDescriptorPool>(ctx.vkDescriptorPool, [&](auto var){ 
            vkDestroyDescriptorPool(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
        });

        // Create the initial swapchain
        if (!InitializeSwapchain()) {
            YART_LOG_ERR("VULKAN: Failed to initialize swapchain\n");
            return false;
        }

        return true;
    }

    bool Backend::InitImGui()
    {
        BackendContext& ctx = GetBackendContext();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(ctx.window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance       = ctx.vkInstance;
        init_info.PhysicalDevice = ctx.vkPhysicalDevice;
        init_info.Device         = ctx.vkDevice;
        init_info.Queue          = ctx.vkQueue;
        init_info.QueueFamily    = ctx.vkQueueFamily;
        init_info.DescriptorPool = ctx.vkDescriptorPool;
        init_info.MinImageCount  = ctx.minImageCount;
        init_info.ImageCount     = ctx.imageCount;
        init_info.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;
        // init_info.CheckVkResultFn = check_vk_result;

        // Create ImGui render pipeline
        ImGui_ImplVulkan_Init(&init_info, ctx.vkRenderPass);

        // Load the default Dear ImGui font
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontDefault();

        // Setup Dear ImGui with an application-level callback
        if (ctx.onDearImGuiSetupCallback)
            ctx.onDearImGuiSetupCallback();

        // Upload fonts
        VkCommandPool command_pool = ctx.framesInFlight[ctx.currentFrameInFlightIndex].vkCommandPool;
        VkCommandBuffer command_buffer = yart::utils::BeginSingleTimeVulkanCommandBuffer(ctx.vkDevice, command_pool);
        if (command_buffer == VK_NULL_HANDLE) 
            return false;

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        yart::utils::EndSingleTimeVulkanCommandBuffer(ctx.vkDevice, command_pool, ctx.vkQueue, command_buffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        return true;
    }

    std::vector<Backend::vk_extension_t> Backend::GetRequiredVulkanExtensions()
    {
        std::vector<vk_extension_t> instance_exts;

        // Query Vulkan extensions required by GLFW
        uint32_t glfw_ext_count = 0;
        const char** glfw_exts = glfwGetRequiredInstanceExtensions(&glfw_ext_count);
        if (glfw_exts == NULL)
            return instance_exts;

        instance_exts.reserve(glfw_ext_count);
        for (size_t i = 0; i < glfw_ext_count; ++i)
            instance_exts.push_back(glfw_exts[i]);

    #ifdef YART_VULKAN_DEBUG_UTILS
        // Vulkan debug utils additionally require VK_EXT_debug_utils
        instance_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

        return instance_exts;
    }

    VkInstance Backend::CreateVulkanInstance(const std::vector<vk_extension_t>& extensions)
    {
        VkInstance instance = VK_NULL_HANDLE;

        int err = utils::CheckVulkanInstanceExtensionsAvailable(extensions);
        if (err >= 0) {
            YART_LOG_ERR("VULKAN: %s extension is not available\n", extensions[static_cast<size_t>(err)]);
            return VK_NULL_HANDLE;
        }

        VkInstanceCreateInfo instance_ci = { };
        instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_ci.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        instance_ci.ppEnabledExtensionNames = extensions.data();

    #ifdef YART_VULKAN_DEBUG_UTILS
        // Enable Vulkan validation layers for debug reporting features 
        const char* layers[1] = { "VK_LAYER_KHRONOS_validation" };

        instance_ci.ppEnabledLayerNames = layers;
        instance_ci.enabledLayerCount = YART_ARRAYSIZE(layers);
    #endif

        VkResult res = vkCreateInstance(&instance_ci, DEFAULT_VK_ALLOC, &instance);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return instance; 
    }

    VkDebugUtilsMessengerEXT Backend::CreateVulkanDebugMessenger(VkInstance instance, PFN_vkDebugUtilsMessengerCallbackEXT callback)
    {
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

        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_ci = { };
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

    VkPhysicalDevice Backend::SelectVulkanPhysicalDevice(VkInstance instance, VkPhysicalDeviceProperties& properties)
    {
        VkPhysicalDevice gpu = VK_NULL_HANDLE; 

        uint32_t gpu_count;
        VkResult res = vkEnumeratePhysicalDevices(instance, &gpu_count, NULL);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        std::unique_ptr<VkPhysicalDevice[]> gpus = std::make_unique<VkPhysicalDevice[]>(gpu_count);
        res = vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.get());
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        VkPhysicalDeviceProperties props = { };
        for (size_t i = 0; i < (size_t)gpu_count; ++i) {
            vkGetPhysicalDeviceProperties(gpus[i], &props);
            if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                properties = props;

                gpu = gpus[i];
                return gpu;
            }
        }

        vkGetPhysicalDeviceProperties(gpus[0], &props);
        properties = props;

        gpu = gpus[0];
        return gpu;
    }

    bool Backend::GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags, VkSurfaceKHR surface)
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

    VkDevice Backend::CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family, const std::vector<vk_extension_t>& extensions)
    {
        VkDevice device = VK_NULL_HANDLE;

        int err = utils::CheckVulkanDeviceExtensionsAvailable(physical_device, extensions);
        if (err >= 0) {
            YART_LOG_ERR("%s device extension is not available\n", extensions[(size_t)err]);
            return VK_NULL_HANDLE;
        }

        const float queue_priority[] = { 1.0f };

        VkDeviceQueueCreateInfo queue_ci[1] = { };
        queue_ci[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci[0].queueFamilyIndex = queue_family;
        queue_ci[0].pQueuePriorities = queue_priority;
        queue_ci[0].queueCount       = YART_ARRAYSIZE(queue_priority);

        VkDeviceCreateInfo device_ci = { };
        device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_ci.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        device_ci.ppEnabledExtensionNames = extensions.data();
        device_ci.pQueueCreateInfos       = queue_ci;
        device_ci.queueCreateInfoCount    = YART_ARRAYSIZE(queue_ci);

        VkResult res = vkCreateDevice(physical_device, &device_ci, DEFAULT_VK_ALLOC, &device);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return device;
    }

    VkDescriptorPool Backend::CreateVulkanDescriptorPool(VkDevice device)
    {
        VkDescriptorPool pool = VK_NULL_HANDLE;

        static const uint32_t size = 1024;
        VkDescriptorPoolSize pool_sizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLER, size },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, size },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, size },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, size },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, size },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, size },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, size }
        };

        VkDescriptorPoolCreateInfo pool_ci = { };
        pool_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_ci.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT; // Allow for descriptor sets allocated from the pool to be individually freed back to the pool
        pool_ci.pPoolSizes    = pool_sizes;
        pool_ci.poolSizeCount = static_cast<uint32_t>(YART_ARRAYSIZE(pool_sizes));
        pool_ci.maxSets       = static_cast<uint32_t>(size * YART_ARRAYSIZE(pool_sizes));

        VkResult res = vkCreateDescriptorPool(device, &pool_ci, DEFAULT_VK_ALLOC, &pool);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return pool;
    }

    bool Backend::InitializeSwapchain()
    {
        BackendContext& ctx = GetBackendContext();
        VkResult res;

        // Select an available surface format (preferably (`VK_FORMAT_B8G8R8A8_SRGB`, `VK_COLOR_SPACE_SRGB_NONLINEAR_KHR`))
        ctx.vkSurfaceFormat = utils::RequestVulkanSurfaceFormat(ctx.vkPhysicalDevice, ctx.vkSurface, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
 
        // Select a surface presentation mode
        VkPresentModeKHR preferred_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
        ctx.vkSurfacePresentMode = utils::RequestVulkanSurfacePresentMode(ctx.vkPhysicalDevice, ctx.vkSurface, preferred_present_mode);

        VkSurfaceCapabilitiesKHR surface_capabilities = { };
        res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(ctx.vkPhysicalDevice, ctx.vkSurface, &surface_capabilities);
        CHECK_VK_RESULT_RETURN(res, false);

        ctx.vkSurfaceExtent = surface_capabilities.currentExtent;

        // NOTE: 
        // Min and max image count variables are set just once here and reused throughout the application runtime for swapchain rebuilds
        // In case of the capabilities not being constant - these should probably be retrieved and set every rebuild, but I can't find much info of such possibility 
        uint32_t min_img_count = utils::GetMinImageCountFromPresentMode(ctx.vkSurfacePresentMode);
        ctx.minImageCount = std::max<uint32_t>(min_img_count, surface_capabilities.minImageCount);

        ctx.maxImageCount = surface_capabilities.maxImageCount;
        if (surface_capabilities.maxImageCount != 0) // maxImageCount == 0 means there is no maximum
            ctx.minImageCount = std::min<uint32_t>(ctx.minImageCount, ctx.maxImageCount);

        // Create a Vulkan render pass with a single subpass
        ctx.vkRenderPass = CreateVulkanRenderPass(ctx.vkDevice, ctx.vkSurfaceFormat);
        ASSERT_VK_HANDLE_INIT(ctx.vkRenderPass, "VULKAN: Failed to create Vulkan render pass\n");

        ctx.LT.Push<VkRenderPass>(ctx.vkRenderPass, [&](VkRenderPass var){
            vkDestroyRenderPass(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
        });

        // Create initial Vulkan swapchain 
        ctx.vkSwapchain = CreateVulkanSwapchain(ctx.vkDevice, ctx.vkSurface, ctx.vkSurfaceFormat, ctx.vkSurfacePresentMode, ctx.vkSurfaceExtent, ctx.minImageCount);
        ASSERT_VK_HANDLE_INIT(ctx.vkSwapchain, "VULKAN: Failed to create Vulkan swapchain\n");

        // Create frame in flight objects 
        if (!CreateSwapchainFramesInFlight()) {
            YART_LOG_ERR("VULKAN: Failed to create swapchain frames in flight\n");
            return false;
        }
        
        return true;
    }

    VkRenderPass Backend::CreateVulkanRenderPass(VkDevice device, VkSurfaceFormatKHR surface_format)
    {
        VkRenderPass render_pass = VK_NULL_HANDLE;

        VkAttachmentDescription attachments[1] = { };
        attachments[0].format         = surface_format.format;
        attachments[0].samples        = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachments[1] = { };
        color_attachments[0].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_attachments[0].attachment = 0;

        VkSubpassDescription subpasses[1] = { };
        subpasses[0].pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpasses[0].pColorAttachments    = color_attachments;
        subpasses[0].colorAttachmentCount = YART_ARRAYSIZE(color_attachments);

        VkSubpassDependency dependencies[1] = { };
        dependencies[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass    = 0;
        dependencies[0].srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = 0;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo render_pass_ci = { };
        render_pass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_ci.pAttachments    = attachments;
        render_pass_ci.attachmentCount = YART_ARRAYSIZE(attachments);
        render_pass_ci.pSubpasses      = subpasses;
        render_pass_ci.subpassCount    = YART_ARRAYSIZE(subpasses);
        render_pass_ci.pDependencies   = dependencies;
        render_pass_ci.dependencyCount = YART_ARRAYSIZE(dependencies);

        VkResult res = vkCreateRenderPass(device, &render_pass_ci, DEFAULT_VK_ALLOC, &render_pass);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return render_pass;
    }

    VkSwapchainKHR Backend::CreateVulkanSwapchain(VkDevice device, VkSurfaceKHR surface, VkSurfaceFormatKHR surface_format, VkPresentModeKHR present_mode, VkExtent2D extent, uint32_t min_image_count, VkSwapchainKHR old_swapchain)
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        VkSwapchainCreateInfoKHR swapchain_ci = {};
        swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_ci.surface          = surface;
        swapchain_ci.imageFormat      = surface_format.format;
        swapchain_ci.imageColorSpace  = surface_format.colorSpace;
        swapchain_ci.presentMode      = present_mode;
        swapchain_ci.imageExtent      = extent;
        swapchain_ci.minImageCount    = min_image_count;
        swapchain_ci.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // Only in case when graphics family == present family
        swapchain_ci.imageArrayLayers = 1;
        swapchain_ci.preTransform     = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_ci.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_ci.oldSwapchain     = old_swapchain;
        swapchain_ci.clipped          = VK_TRUE;

        VkResult res = vkCreateSwapchainKHR(device, &swapchain_ci, DEFAULT_VK_ALLOC, &swapchain);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return swapchain; 
    }

    bool Backend::CreateSwapchainFramesInFlight()
    {
        BackendContext& ctx = GetBackendContext();
        VkResult res;

        // Query the swapchain image count and the initial set of images
        res = vkGetSwapchainImagesKHR(ctx.vkDevice, ctx.vkSwapchain, &ctx.imageCount, nullptr);
        CHECK_VK_RESULT_RETURN(res, false);

        auto images = std::make_unique<VkImage[]>(ctx.imageCount);
        res = vkGetSwapchainImagesKHR(ctx.vkDevice, ctx.vkSwapchain, &ctx.imageCount, images.get());
        CHECK_VK_RESULT_RETURN(res, false);

        // Create image views for each swapchain image
        auto image_views = std::make_unique<VkImageView[]>(ctx.imageCount);
        for (uint32_t i = 0; i < ctx.imageCount; ++i) {
            image_views[i] = CreateVulkanImageView(ctx.vkDevice, ctx.vkSurfaceFormat.format, images[i]);
            ASSERT_VK_HANDLE_INIT(image_views[i], "Failed to create swapchain image view");

            ctx.swapchainLT.Push<VkImageView>(image_views[i], [&](VkImageView var){
                vkDestroyImageView(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Create framebuffers for each image view
        auto frame_buffers = std::make_unique<VkFramebuffer[]>(ctx.imageCount);
        for (uint32_t i = 0; i < ctx.imageCount; ++i) {
            frame_buffers[i] = CreateVulkanFramebuffer(ctx.vkDevice, ctx.vkRenderPass, ctx.vkSurfaceExtent, image_views[i]);
            ASSERT_VK_HANDLE_INIT(frame_buffers[i], "Failed to create swapchain framebuffer");

            ctx.swapchainLT.Push<VkFramebuffer>(frame_buffers[i], [&](VkFramebuffer var){
                vkDestroyFramebuffer(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Create a Vulkan command pool for each frame
        auto command_pools = std::make_unique<VkCommandPool[]>(ctx.imageCount);

        VkCommandPoolCreateInfo pool_ci = {};
        pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_ci.queueFamilyIndex = ctx.vkQueueFamily;

        for (uint32_t i = 0; i < ctx.imageCount; ++i) {
            res = vkCreateCommandPool(ctx.vkDevice, &pool_ci, DEFAULT_VK_ALLOC, &command_pools[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            ctx.swapchainLT.Push<VkCommandPool>(command_pools[i], [&](VkCommandPool var){
                vkDestroyCommandPool(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Allocate Vulkan command buffers from the pool for each frame
        auto command_buffers = std::make_unique<VkCommandBuffer[]>(ctx.imageCount);

        VkCommandBufferAllocateInfo buffer_ai = {};
        buffer_ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        buffer_ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        buffer_ai.commandBufferCount = 1;

        for (uint32_t i = 0; i < ctx.imageCount; ++i) {
            buffer_ai.commandPool = command_pools[i];

            res = vkAllocateCommandBuffers(ctx.vkDevice, &buffer_ai, &command_buffers[i]);
            CHECK_VK_RESULT_RETURN(res, false);
            // Vulkan command buffers get released automatically with the destruction of the pools they were allocated from
        }

        // Create Vulkan semaphores for each frame 
        auto image_acquired_semaphores = std::make_unique<VkSemaphore[]>(ctx.imageCount);
        auto render_complete_semaphores = std::make_unique<VkSemaphore[]>(ctx.imageCount);

        VkSemaphoreCreateInfo semaphore_ci = {};
        semaphore_ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (uint32_t i = 0; i < ctx.imageCount; ++i) {
            // Image Acquired Semaphore
            res = vkCreateSemaphore(ctx.vkDevice, &semaphore_ci, DEFAULT_VK_ALLOC, &image_acquired_semaphores[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            ctx.swapchainLT.Push<VkSemaphore>(image_acquired_semaphores[i], [&](VkSemaphore var){
                vkDestroySemaphore(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
            });

            // Render Complete Semaphore
            res = vkCreateSemaphore(ctx.vkDevice, &semaphore_ci, DEFAULT_VK_ALLOC, &render_complete_semaphores[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            ctx.swapchainLT.Push<VkSemaphore>(render_complete_semaphores[i], [&](VkSemaphore var){
                vkDestroySemaphore(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // Create Vulkan fences for each frame 
        auto fences = std::make_unique<VkFence[]>(ctx.imageCount);

        VkFenceCreateInfo fence_ci = {};
        fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (uint32_t i = 0; i < ctx.imageCount; ++i) {
            res = vkCreateFence(ctx.vkDevice, &fence_ci, DEFAULT_VK_ALLOC, &fences[i]);
            CHECK_VK_RESULT_RETURN(res, false);

            ctx.swapchainLT.Push<VkFence>(fences[i], [&](VkFence var){
                vkDestroyFence(ctx.vkDevice, var, DEFAULT_VK_ALLOC);
            });
        }

        // NOTE: 
        // The frames in flight are reused here because the image count is expected to stay constant throughout the application run time
        if (ctx.framesInFlight == nullptr)
            ctx.framesInFlight = std::make_unique<FrameInFlight[]>(ctx.imageCount);
            
        for (uint32_t i = 0; i < ctx.imageCount; ++i) {
            ctx.framesInFlight[i].vkFrameBuffer = frame_buffers[i];
            ctx.framesInFlight[i].vkCommandPool = command_pools[i];
            ctx.framesInFlight[i].vkCommandBuffer = command_buffers[i];
            ctx.framesInFlight[i].vkImageAcquiredSemaphore = image_acquired_semaphores[i];
            ctx.framesInFlight[i].vkRenderCompleteSemaphore = render_complete_semaphores[i];
            ctx.framesInFlight[i].vkFence = fences[i];
        }

        return true;
    }

    VkImageView Backend::CreateVulkanImageView(VkDevice device, VkFormat format, VkImage image)
    {
        VkImageView view = VK_NULL_HANDLE;

        VkImageViewCreateInfo view_ci = {}; 
        view_ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
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

    VkFramebuffer Backend::CreateVulkanFramebuffer(VkDevice device, VkRenderPass render_pass, const VkExtent2D& extent, VkImageView image_view)
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

    bool Backend::FrameRender(ImDrawData* draw_data)
    {
        BackendContext& ctx = GetBackendContext();
        VkResult res;
                
        // Get the next available frame in flight index
        VkSemaphore image_acquired_semaphore = ctx.framesInFlight[ctx.currentSemaphoreIndex].vkImageAcquiredSemaphore;

        res = vkAcquireNextImageKHR(ctx.vkDevice, ctx.vkSwapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &ctx.currentFrameInFlightIndex);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
            return true; // Signal that swapchain should be rebuilt 
        CHECK_VK_RESULT_ABORT(res);

        // Wait for and reset frame fence
        VkFence fence = ctx.framesInFlight[ctx.currentFrameInFlightIndex].vkFence;

        res = vkWaitForFences(ctx.vkDevice, 1, &fence, VK_TRUE, UINT64_MAX); 
        CHECK_VK_RESULT_ABORT(res);

        res = vkResetFences(ctx.vkDevice, 1, &fence);
        CHECK_VK_RESULT_ABORT(res);

        // Reset command pool
        res = vkResetCommandPool(ctx.vkDevice, ctx.framesInFlight[ctx.currentFrameInFlightIndex].vkCommandPool, (VkCommandPoolResetFlags)0);
        CHECK_VK_RESULT_ABORT(res);

        // Begin command buffer for render commands
        VkCommandBuffer cmd_buffer = ctx.framesInFlight[ctx.currentFrameInFlightIndex].vkCommandBuffer;

        VkCommandBufferBeginInfo cmd_begin_info = {};
        cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        res = vkBeginCommandBuffer(cmd_buffer, &cmd_begin_info);
        CHECK_VK_RESULT_ABORT(res);

        // Begin render pass
        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.framebuffer = ctx.framesInFlight[ctx.currentFrameInFlightIndex].vkFrameBuffer;
        render_pass_info.renderArea.extent = ctx.vkSurfaceExtent;
        render_pass_info.renderPass = ctx.vkRenderPass;
        static const VkClearValue clear_value = {};
        render_pass_info.pClearValues = &clear_value;
        render_pass_info.clearValueCount = 1;

        vkCmdBeginRenderPass(cmd_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        // Record ImGui primitives into the command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, cmd_buffer);

        // Submit command buffer
        vkCmdEndRenderPass(cmd_buffer);

        // End command buffer
        res = vkEndCommandBuffer(cmd_buffer);
        CHECK_VK_RESULT_ABORT(res);

        // Submit queue 
        VkSemaphore render_complete_semaphore = ctx.framesInFlight[ctx.currentSemaphoreIndex].vkRenderCompleteSemaphore;
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit_info = {};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pSignalSemaphores = &render_complete_semaphore;
        submit_info.pWaitSemaphores = &image_acquired_semaphore;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitDstStageMask = &wait_stage;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &cmd_buffer;
        submit_info.signalSemaphoreCount = 1;

        res = vkQueueSubmit(ctx.vkQueue, 1, &submit_info, fence);
        CHECK_VK_RESULT_ABORT(res);

        return false;
    }

    bool Backend::FramePresent()
    {
        BackendContext& ctx = GetBackendContext();
        VkSemaphore render_complete_semaphore = ctx.framesInFlight[ctx.currentSemaphoreIndex].vkRenderCompleteSemaphore;

        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pImageIndices = &ctx.currentFrameInFlightIndex;
        present_info.pWaitSemaphores = &render_complete_semaphore;
        present_info.waitSemaphoreCount = 1;
        present_info.pSwapchains = &ctx.vkSwapchain;
        present_info.swapchainCount = 1;

        VkResult res = vkQueuePresentKHR(ctx.vkQueue, &present_info);
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
            return true; // Signal that swapchain should be rebuilt 
        CHECK_VK_RESULT_ABORT(res);

        // Use the next set of semaphores
        ctx.currentSemaphoreIndex = (ctx.currentSemaphoreIndex + 1) % ctx.imageCount; 

        return false;
    }

    void Backend::WindowResize(uint32_t width, uint32_t height)
    {
        BackendContext& ctx = GetBackendContext();

        // Wait for the GPU to finish execution 
        VkResult res = vkDeviceWaitIdle(ctx.vkDevice);
        CHECK_VK_RESULT_ABORT(res);

        // Release all swapchain related objects
        ctx.swapchainLT.Release();

        // ImGui_ImplVulkan_SetMinImageCount(m_minImageCount); // NOTE: m_minImageCount is kept constant throughout the application life span

        // Recreate Vulkan swapchain
        ctx.vkSurfaceExtent.width = width;
        ctx.vkSurfaceExtent.height = height;

        VkSwapchainKHR old_swapchain = ctx.vkSwapchain;
        ctx.vkSwapchain = CreateVulkanSwapchain(ctx.vkDevice, ctx.vkSurface, ctx.vkSurfaceFormat, ctx.vkSurfacePresentMode, ctx.vkSurfaceExtent, ctx.minImageCount, old_swapchain);
        YART_ASSERT(ctx.vkSwapchain != VK_NULL_HANDLE);

        // Release the previous Vulkan swapchain
        vkDestroySwapchainKHR(ctx.vkDevice, old_swapchain, DEFAULT_VK_ALLOC);

        // Create frame in flight objects 
        if (!CreateSwapchainFramesInFlight())
            YART_ABORT("VULKAN: Failed to create swapchain frames in flight");

        ctx.currentFrameInFlightIndex = 0;
    }

} // namespace yart
