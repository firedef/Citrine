#ifndef CITRINE_VULKANINSTANCE_H
#define CITRINE_VULKANINSTANCE_H

#include "VkHelper.h"

struct VulkanInstance {
private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }
    static void getRequiredExtensions(std::vector<const char*>& requiredDeviceExtensions) {
        uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (int i = 0; i < glfwExtensionCount; ++i) requiredDeviceExtensions.push_back(glfwExtensions[i]);
        if (enableVkValidationLayers) requiredDeviceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    static void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }
    void setupDebugMessenger() {
        if (!enableVkValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        populateDebugMessenger(createInfo);

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        VkCheck(func != nullptr ? func(instance, &createInfo, nullptr, &debugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT, "vkCreateDebugUtilsMessengerEXT (VulkanInstance.h)")
    }
    void destroyDebugMessenger() const {
        if (!enableVkValidationLayers) return;
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) func(instance, debugMessenger, nullptr);
    }
public:
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    
    void create(const std::vector<const char*>& vkRequiredValidationLayers) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Citrine engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = 0;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableVkValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(vkRequiredValidationLayers.size());
            createInfo.ppEnabledLayerNames = vkRequiredValidationLayers.data();
            populateDebugMessenger(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        }

        std::vector<const char*> extensions;
        getRequiredExtensions(extensions);
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = 0;

        VkCheck(vkCreateInstance(&createInfo, nullptr, &instance), "vkCreateInstance (VulkanInstance.h)");
        
        setupDebugMessenger();
    }
    
    void destroy() {
        destroyDebugMessenger();

        vkDestroyInstance(instance, nullptr);
    }
};

#endif //CITRINE_VULKANINSTANCE_H
