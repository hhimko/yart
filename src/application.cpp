#include "application.h"

#include <iostream>

#include "utils/vk_utils.h"


#define ASSERT_HANDLE_INIT(handle, err)     \
    if (handle == VK_NULL_HANDLE) {         \
        std::cerr << err << std::endl;      \
        return 0;                           \
    }

#define YART_WINDOW_TITLE "Yet Another Ray Tracer"


// TODO: error logging
static void on_glfw_error(int error_code, const char *description) 
{
    std::cerr << "GLFW Error " << error_code << ": " << description << std::endl;
}

static void on_glfw_window_close(GLFWwindow* window)
{
    UNUSED(window);
    yart::Application::Get().Close();    
}

// TODO: Proper vk debug logging
#ifdef YART_VULKAN_DEBUG_UTILS
static VkBool32 VKAPI_PTR on_vulkan_debug_message(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    VkDebugUtilsMessageTypeFlagsEXT msg_type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* user_data) 
{
    UNUSED(msg_severity, msg_type, user_data);

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
        // TODO: assert not m_running
        m_running = true;

        while (m_running) {
            glfwPollEvents();
        }

        return EXIT_SUCCESS;
    }

    void Application::Close()
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
        m_window = glfwCreateWindow(win_w, win_h, YART_WINDOW_TITLE, NULL, NULL);
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
            vkDestroyInstance(var, nullptr);
        });

    #ifdef YART_VULKAN_DEBUG_UTILS
        auto debug_messenger = CreateVulkanDebugMessenger(m_vkInstance, on_vulkan_debug_message);
        ASSERT_HANDLE_INIT(debug_messenger, "Failed to create Vulkan debug messenger");

        m_ltStack.Push<VkDebugUtilsMessengerEXT>(debug_messenger, [&](auto var){ 
            LOAD_VK_INSTANCE_FP(m_vkInstance, vkDestroyDebugUtilsMessengerEXT);
            vkDestroyDebugUtilsMessengerEXT(m_vkInstance, var, nullptr);
        });
    #endif

        VkPhysicalDeviceProperties gpu_properties = {};
        auto gpu = SelectVulkanPhysicalDevice(m_vkInstance, gpu_properties);
        ASSERT_HANDLE_INIT(gpu, "Failed to locate a physical Vulkan device");

        // Select graphics queue family index from the gpu 
        uint32_t queue_family;
        if (!GetVulkanQueueFamilyIndex(gpu, &queue_family, VK_QUEUE_GRAPHICS_BIT)) {
            return 0;
        }

        // Create a VkDevice with a single queue and `VK_KHR_swapchain` extension 
        auto device = CreateVulkanLogicalDevice(gpu, queue_family);
        ASSERT_HANDLE_INIT(device, "Failed to create Vulkan device");

        m_ltStack.Push<VkDevice>(device, [](auto var){ 
            vkDestroyDevice(var, nullptr);
        });

        // Extract the graphics queue from the logical device
        VkQueue queue = VK_NULL_HANDLE;
        vkGetDeviceQueue(device, queue_family, 0, &queue);
        ASSERT_HANDLE_INIT(queue, "Failed to retrieve graphics queue from Vulkan device");

        // Create a Vulkan surface for the main GLFW window
        VkSurfaceKHR surface;
        res = glfwCreateWindowSurface(m_vkInstance, m_window, nullptr, &surface);
        CHECK_VK_RESULT_RETURN(res, 0);

        m_ltStack.Push<VkSurfaceKHR>(surface, [&](auto var){ 
            vkDestroySurfaceKHR(m_vkInstance, var, nullptr);
        });

        // Check for Windowing System Integration support on GPU
        VkBool32 wsi_support;
        res = vkGetPhysicalDeviceSurfaceSupportKHR(gpu, queue_family, surface, &wsi_support);
        if (wsi_support != VK_TRUE) {
            std::cerr << "No WSI support on physical Vulkan device" << std::endl;
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

    VkInstance Application::CreateVulkanInstance(std::vector<const char*> extensions)
    {
        VkInstance instance = VK_NULL_HANDLE;

        int err = utils::CheckVulkanExtensionsAvailable(extensions);
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

        VkResult res = vkCreateInstance(&instance_ci, nullptr, &instance);
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
        VkResult res = vkCreateDebugUtilsMessengerEXT(instance, &debug_messenger_ci, nullptr, &debug_messenger);
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

    bool Application::GetVulkanQueueFamilyIndex(VkPhysicalDevice physical_device, uint32_t* result, VkQueueFlags flags)
    {
        uint32_t queue_count;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, NULL);

        VkQueueFamilyProperties* queues = new VkQueueFamilyProperties[queue_count];
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_count, queues);

        for (uint32_t i = 0; i < queue_count; i++) {
            if (queues[i].queueFlags & flags) {
                *result = i;

                delete[] queues;
                return true;
            }
        }

        delete[] queues;
        return false;
    }

    VkDevice Application::CreateVulkanLogicalDevice(VkPhysicalDevice physical_device, uint32_t queue_family)
    {
        VkDevice device = VK_NULL_HANDLE;

        const float queue_priority[] = { 1.0f };
        const char* device_extensions[] = { "VK_KHR_swapchain" };

        VkDeviceQueueCreateInfo queue_ci[1] = {};
        queue_ci[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_ci[0].pQueuePriorities = queue_priority;
        queue_ci[0].queueFamilyIndex = queue_family;
        queue_ci[0].queueCount = 1;

        VkDeviceCreateInfo device_ci = {};
        device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_ci.ppEnabledExtensionNames = device_extensions;
        device_ci.pQueueCreateInfos = queue_ci;
        device_ci.enabledExtensionCount = 1;
        device_ci.queueCreateInfoCount = 1;

        VkResult res = vkCreateDevice(physical_device, &device_ci, nullptr, &device);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return device;
    }

    void Application::Cleanup() 
    {
        // Unwind all allocations from the LTStack
        m_ltStack.Release();
        glfwTerminate();
    }
} // namespace yart
