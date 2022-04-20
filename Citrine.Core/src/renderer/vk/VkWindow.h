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
#include "CommandBuffer.h"
#include "Queues.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "VulkanInstance.h"
#include "CommandPool.h"
#include "SwapChain.h"




class VkWindow : public Window {
public:
    VulkanInstance vkInstance;
    PhysicalDevice physicalDevice;
    LogicalDevice device;

    VkSurfaceKHR surface;
    SwapChain swapChain;
    
    CommandPool commandPool;
    
private:
    const int maxFramesInFlight = 2;
    
    Queues queues;
    
    //SwapChainSupportDetails swapChainSupportDetails{};
    
    const std::vector<const char*> vkRequiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    const std::vector<const char*> vkRequiredDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    
    void initVulkan();
    void createInstance();
    //static VkBool32 debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT* callbackDataExt, void* userData);
    
    void createLogicalDevice();
    
    void createCommandPool();
public:
    VkWindow();
    void createFramebuffers(VkRenderPass renderPass);
    bool startCommandBuffer();
    void endCommandBuffer();
    void recreateSwapChain();
    
    void Close() override;
};


#endif //CITRINE_VKWINDOW_H
