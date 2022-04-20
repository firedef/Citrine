#ifndef CITRINE_SWAPCHAIN_H
#define CITRINE_SWAPCHAIN_H

#include "VkHelper.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include <vector>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChain {
private:
    void querySwapChainSupport(PhysicalDevice& physicalDevice, VkSurfaceKHR surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice.physicalDevice, surface, &swapChainSupportDetails.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.physicalDevice, surface, &formatCount, nullptr);
        if (formatCount != 0) {
            swapChainSupportDetails.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice.physicalDevice, surface, &formatCount, swapChainSupportDetails.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.physicalDevice, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) {
            swapChainSupportDetails.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice.physicalDevice, surface, &presentModeCount, swapChainSupportDetails.presentModes.data());
        }
    }
    
    void chooseSwapSurfaceFormat() {
        for (const auto &format: swapChainSupportDetails.formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat = format;
                return;
            }
        }

        surfaceFormat = swapChainSupportDetails.formats[0];
    }
    
    void chooseSwapPresentMode() {
        // V-Sync
        // VK_PRESENT_MODE_FIFO_KHR on linux x11 with nvidia drivers lags so much, so avoid it's as much as possible
        swapPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        // no V-Sync
        if (std::find(swapChainSupportDetails.presentModes.begin(), swapChainSupportDetails.presentModes.end(), VK_PRESENT_MODE_IMMEDIATE_KHR) != swapChainSupportDetails.presentModes.end())
            swapPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

        // V-Sync
        if (std::find(swapChainSupportDetails.presentModes.begin(), swapChainSupportDetails.presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != swapChainSupportDetails.presentModes.end())
            swapPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        std::string presentModeName = "?";
        switch (swapPresentMode) {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                presentModeName = "VK_PRESENT_MODE_IMMEDIATE_KHR";
                break;
            case VK_PRESENT_MODE_MAILBOX_KHR:
                presentModeName = "VK_PRESENT_MODE_MAILBOX_KHR";
                break;
            case VK_PRESENT_MODE_FIFO_KHR:
                presentModeName = "VK_PRESENT_MODE_FIFO_KHR";
                break;
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                presentModeName = "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
                break;
            case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
                presentModeName = "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
                break;
            case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
                presentModeName = "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
                break;
            case VK_PRESENT_MODE_MAX_ENUM_KHR:
                presentModeName = "VK_PRESENT_MODE_MAX_ENUM_KHR";
                break;
        }
        std::cout << "chosen " << presentModeName << " as present mode\n";
    }
    
    void chooseSwapExtent(GLFWwindow* glfwWindow) {
        if (swapChainSupportDetails.capabilities.currentExtent.width != UINT32_MAX) {
            swapExtent = swapChainSupportDetails.capabilities.currentExtent;
            return;
        }

        int width, height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        swapExtent = {
                swapExtent.width = std::clamp(static_cast<uint32_t>(width),
                                              swapChainSupportDetails.capabilities.minImageExtent.width,
                                              swapChainSupportDetails.capabilities.maxImageExtent.width),
                swapExtent.height = std::clamp(static_cast<uint32_t>(height),
                                               swapChainSupportDetails.capabilities.minImageExtent.height,
                                               swapChainSupportDetails.capabilities.maxImageExtent.height),
        };
    }

    void createImageViews(LogicalDevice& device) {
        size_t count = swapChainImages.size();
        swapChainImageViews.resize(count);
        for (int i = 0; i < count; ++i) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkCheck(vkCreateImageView(device.device, &createInfo, nullptr, &swapChainImageViews[i]), "vkCreateImageView (SwapChain.h)");
        }
    }
public:
    SwapChainSupportDetails swapChainSupportDetails;
    VkSwapchainKHR swapChain;
    VkExtent2D swapExtent;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR swapPresentMode;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    uint32_t currentImageIndex = 0;
    uint32_t swapchainSize = 0;
    
    void create(GLFWwindow* glfwWindow, PhysicalDevice& physicalDevice, LogicalDevice& device, VkSurfaceKHR surface, Queues& queues) {
        querySwapChainSupport(physicalDevice, surface);
        chooseSwapSurfaceFormat();
        chooseSwapPresentMode();
        chooseSwapExtent(glfwWindow);

        uint32_t imageCount = std::min(swapChainSupportDetails.capabilities.minImageCount + 1, swapChainSupportDetails.capabilities.maxImageCount);

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = swapExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        std::vector<uint32_t> queueFamilies = {queues.graphicsIndex, queues.presentIndex};
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (queues.graphicsIndex != queues.presentIndex) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilies.data();
        }

        createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = swapPresentMode;
        createInfo.clipped = true;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkCheck(vkCreateSwapchainKHR(device.device, &createInfo, nullptr, &swapChain), "vkCreateSwapchainKHR (SwapChain.h)");

        vkGetSwapchainImagesKHR(device.device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.device, swapChain, &imageCount, swapChainImages.data());
        
        createImageViews(device);
    }

    void createFramebuffers(VkRenderPass renderPass, LogicalDevice& device) {
        size_t count = swapChainImageViews.size();
        swapChainFramebuffers.resize(count);

        for (int i = 0; i < count; ++i) {
            VkImageView attachments[] = {swapChainImageViews[i]};

            VkFramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = renderPass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = swapExtent.width;
            framebufferCreateInfo.height = swapExtent.height;
            framebufferCreateInfo.layers = 1;

            VkCheck(vkCreateFramebuffer(device.device, &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]), "vkCreateFramebuffer (SwapChain.h)");
        }
    }
    
    void destroy(LogicalDevice& device) {
        for (auto framebuffer: swapChainFramebuffers) vkDestroyFramebuffer(device.device, framebuffer, nullptr);
        for (auto view: swapChainImageViews) vkDestroyImageView(device.device, view, nullptr);
        vkDestroySwapchainKHR(device.device, swapChain, nullptr);
    }
    
    void recreate(GLFWwindow* glfwWindow, PhysicalDevice& physicalDevice, LogicalDevice& device, VkSurfaceKHR surface, Queues& queues) {
        cleanup(device);
        create(glfwWindow, physicalDevice, device, surface, queues);
    }

    void cleanup(LogicalDevice& device) {
        size_t count = swapChainFramebuffers.size();
        for (int i = 0; i < count; ++i) vkDestroyFramebuffer(device.device, swapChainFramebuffers[i], nullptr);
        for (int i = 0; i < count; ++i) vkDestroyImageView(device.device, swapChainImageViews[i], nullptr);
        vkDestroySwapchainKHR(device.device, swapChain, nullptr);
    }
};

#endif //CITRINE_SWAPCHAIN_H
