#ifndef CITRINE_PHYSICALDEVICE_H
#define CITRINE_PHYSICALDEVICE_H

#include "VkHelper.h"
#include <vector>

struct PhysicalDevice {
private:
    void chooseDevice(const std::vector<VkPhysicalDevice>& devices) {
        VkPhysicalDevice bestDevice = devices[0];
        uint32_t bestDevicePoints = 0;

        for (const auto &device: devices) {
            uint32_t currentPoints = 0;
            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) currentPoints += 1000;
            if (deviceFeatures.geometryShader) currentPoints += 1000;
            currentPoints += deviceProperties.limits.maxImageDimension2D;

            if (currentPoints <= bestDevicePoints) continue;
            bestDevice = device;
            bestDevicePoints = currentPoints;
        }

        physicalDevice = bestDevice;
    }
public:    
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    
    void create(VkInstance instance) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) throw std::runtime_error("no GPUs with vulkan support found");
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        chooseDevice(devices);
        
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
        vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    }
};

#endif //CITRINE_PHYSICALDEVICE_H
