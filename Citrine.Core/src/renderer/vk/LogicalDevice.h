#ifndef CITRINE_LOGICALDEVICE_H
#define CITRINE_LOGICALDEVICE_H

#include "VkHelper.h"
#include <optional>
#include <vector>
#include <set>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> present;
};

struct LogicalDevice {
private:
    void findQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &family: queueFamilies) {
            if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) vkQueueFamilyIndices.graphics = i;
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) vkQueueFamilyIndices.present = i;
            i++;
        }
    }
    
    void populateQueueCreateInfo(std::vector<VkDeviceQueueCreateInfo>& createInfo) {
        std::set<uint32_t> uniqueQueueFamilies = {vkQueueFamilyIndices.graphics.value(), vkQueueFamilyIndices.present.value()};
        const float priority = 1;

        for (uint32_t family: uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = family;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &priority;
            createInfo.push_back(queueCreateInfo);
        }
    }
public:
    QueueFamilyIndices vkQueueFamilyIndices{};
    VkDevice device{};
    
    void create(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<const char*>& vkRequiredValidationLayers, const std::vector<const char*>& vkRequiredDeviceExtensions) {
        findQueueFamilyIndices(physicalDevice, surface);
        VkPhysicalDeviceFeatures physicalDeviceFeatures{};
        vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
        populateQueueCreateInfo(queueCreateInfo);
        
        VkDeviceCreateInfo createInfo{};

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfo.size());
        createInfo.pQueueCreateInfos = queueCreateInfo.data();
        createInfo.pEnabledFeatures = &physicalDeviceFeatures;

        createInfo.enabledLayerCount = 0;
        if (enableVkValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(vkRequiredValidationLayers.size());
            createInfo.ppEnabledLayerNames = vkRequiredValidationLayers.data();
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(vkRequiredDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = vkRequiredDeviceExtensions.data();

        VkCheck(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device), "vkCreateDevice (LogicalDevice.h)");
    }
    
    void WaitIdle() const {
        vkDeviceWaitIdle(device);
    }
    
    void destroy() const {
        vkDestroyDevice(device, nullptr);
    }
};

#endif //CITRINE_LOGICALDEVICE_H
