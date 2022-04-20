#ifndef CITRINE_VKWINDOW_H
#define CITRINE_VKWINDOW_H

#define GLFW_INCLUDE_VULKAN
#include "../glfw/Window.h"
#include "VkHelper.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <optional>
#include <GLFW/glfw3native.h>
#include <set>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VkWindow : public Window {
public:
    VkInstance vkInstance;
    VkPhysicalDevice physicalDevice;
    VkDevice vkDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures physicalDeviceFeatures;

    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR swapPresentMode;
    VkExtent2D swapExtent;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    
    uint32_t currentImageIndex = 0;
    
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    
private:
#ifdef NDEBUG
    const bool enableVkValidationLayers = false;
#else
    const bool enableVkValidationLayers = true;
#endif
    uint32_t vkExtensionCount;
    std::vector<VkExtensionProperties> vkExtensions;

    uint32_t vkValidationLayerCount;
    std::vector<VkLayerProperties> vkValidationLayers;
    
    uint32_t glfwExtensionCount;
    const char** glfwExtensions;
    
    QueueFamilyIndices vkQueueFamilyIndices{};
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    
    SwapChainSupportDetails swapChainSupportDetails{};
    
    VkDebugUtilsMessengerEXT debugMessenger;
    
    const std::vector<const char*> vkRequiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> vkRequiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    
    void initVulkan();
    void createInstance();
    std::vector<const char*> getRequiredExtensions();
    static void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void setupDebugMessenger();
    //static VkBool32 debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT* callbackDataExt, void* userData);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
    
    void pickPhysicalDevice();
    VkPhysicalDevice chooseDevice(const std::vector<VkPhysicalDevice>& devices);

    void createLogicalDevice();
    void findQueueFamilyIndices();
    
    void querySwapChainSupport();
    void chooseSwapSurfaceFormat();
    void chooseSwapPresentMode();
    void chooseSwapExtent();
    void createSwapChain();
    
    void createImageViews();

    void createSurface();
    
    void createCommandPool();
    void createCommandBuffer();
    void recordCommandBuffer(VkCommandBuffer buffer, uint32_t imageIndex);
    
    void createSyncObjects();
    void destroyDebugMessenger();
public:
    VkWindow();
    void createFramebuffers(VkRenderPass renderPass);
    void startCommandBuffer();
    void endCommandBuffer();
    
    void waitForFence();
    
    void Close() override;
};


#endif //CITRINE_VKWINDOW_H
