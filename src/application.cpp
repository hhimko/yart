#include "application.h"

#include <iostream>

#include "utils/vk_utils.h"


#define YART_WINDOW_TITLE "Yet Another Ray Tracer"


// TODO: error logging
static void on_glfw_error(int error_code, const char *description) 
{
    std::cerr << "GLFW Error " << error_code << ": " << description << std::endl;
}

// TODO: Proper vk debug logging
#ifdef YART_VULKAN_DEBUG_UTILS
static VkBool32 VKAPI_PTR on_vulkan_debug_message(
    VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
    VkDebugUtilsMessageTypeFlagsEXT msg_type,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* user_data) 
{
    UNUSED(msg_severity, msg_type, data, user_data);

    std::cout << data->pMessage << std::endl;
    return VK_FALSE;
}
#endif


namespace yart
{
    Application::Application(int win_w, int win_h) 
    {
        if (!InitGLFW(win_w, win_h))
            return;

        if (!InitVulkan())
            return;
    }

    Application::~Application() 
    {
        Cleanup();
    }

    int Application::Run() 
    {
        while (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();
        }

        return EXIT_SUCCESS;
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

        return 1;
    }

    int Application::InitVulkan() 
    {
        auto exts = GetRequiredVulkanExtensions();
        if (exts.empty())
            return 0;

        int res = utils::CheckVulkanExtensionsAvailable(exts);
        if (res >= 0) {
            std::cout << exts[(size_t)res] << " extension is not available" << std::endl;
            return 0;
        }

        m_vkInstance = CreateVulkanInstance(exts);
        if (m_vkInstance == VK_NULL_HANDLE) {
            return 0;
        }

    #ifdef YART_VULKAN_DEBUG_UTILS
        VkDebugUtilsMessengerEXT debug_messenger = CreateVulkanDebugMessenger(on_vulkan_debug_message);
        if (debug_messenger == VK_NULL_HANDLE) {
            return 0;
        }

        m_ltStack.Push<VkDebugUtilsMessengerEXT>(debug_messenger, [&](auto var){ 
            LOAD_VK_INSTANCE_FP(m_vkInstance, vkDestroyDebugUtilsMessengerEXT);
            vkDestroyDebugUtilsMessengerEXT(m_vkInstance, var, nullptr);
        });
    #endif
    
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
        VkInstance instance;

        VkInstanceCreateInfo instance_ci = {};
        instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_ci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instance_ci.ppEnabledExtensionNames = extensions.data();

    #ifdef YART_VULKAN_DEBUG_UTILS
        // Enable Vulkan validation layers for debug reporting features 
        const char* layers[1] = { "VK_LAYER_KHRONOS_validation" };
        instance_ci.enabledLayerCount = 1;
        instance_ci.ppEnabledLayerNames = layers;
    #endif

        VkResult res = vkCreateInstance(&instance_ci, nullptr, &instance);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return instance; 
    }

    VkDebugUtilsMessengerEXT Application::CreateVulkanDebugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT callback)
    {
        // Create a single all-purpose debug messenger object  
        VkDebugUtilsMessengerEXT debug_messenger;

        VkDebugUtilsMessageSeverityFlagsEXT message_severity_flag =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        VkDebugUtilsMessageTypeFlagsEXT message_type_flag =
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
        LOAD_VK_INSTANCE_FP(m_vkInstance, vkCreateDebugUtilsMessengerEXT);
        VkResult res = vkCreateDebugUtilsMessengerEXT(m_vkInstance, &debug_messenger_ci, nullptr, &debug_messenger);
        CHECK_VK_RESULT_RETURN(res, VK_NULL_HANDLE);

        return debug_messenger; 
    }

    void Application::Cleanup() 
    {
        m_ltStack.Release();

        // Vulkan cleanup
        vkDestroyInstance(m_vkInstance, nullptr);

        // GLFW cleanup
        glfwTerminate();
    }
} // namespace yart
