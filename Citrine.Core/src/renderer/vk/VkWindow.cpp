#include "VkWindow.h"

void VkWindow::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
}

void VkWindow::createInstance() {
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

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = 0;
    
    VkCheck(vkCreateInstance(&createInfo, nullptr, &vkInstance), "vkCreateInstance (VkWindow.cpp)");

    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);
    vkExtensions = std::vector<VkExtensionProperties>(vkExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());
}

VkWindow::VkWindow() {
    createGlfwWindow();
    initVulkan();
}

void VkWindow::Close() {
    if (enableVkValidationLayers) destroyDebugMessenger();

    vkDestroySemaphore(vkDevice, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(vkDevice, renderFinishedSemaphore, nullptr);
    vkDestroyFence(vkDevice, inFlightFence, nullptr);

    vkDestroyCommandPool(vkDevice, commandPool, nullptr);
    
    for (auto framebuffer: swapChainFramebuffers) vkDestroyFramebuffer(vkDevice, framebuffer, nullptr);
    
    for (auto view: swapChainImageViews) vkDestroyImageView(vkDevice, view, nullptr);
    vkDestroySwapchainKHR(vkDevice, swapChain, nullptr);
    
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroySurfaceKHR(vkInstance, surface, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

std::vector<const char*> VkWindow::getRequiredExtensions() {
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enableVkValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

void VkWindow::setupDebugMessenger() {
    if (!enableVkValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessenger(createInfo);

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
    VkCheck(func != nullptr ? func(vkInstance, &createInfo, nullptr, &debugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT, "vkCreateDebugUtilsMessengerEXT (VkWindow.cpp)")
}

//VkBool32 VkWindow::debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types,
//                                 const VkDebugUtilsMessengerCallbackDataEXT* callbackDataExt, void* userData) {
//    std::cout << "[VK " << severity << " " << types << "]: " << callbackDataExt->pMessage << "\n";
//}

void VkWindow::destroyDebugMessenger() {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) func(vkInstance, debugMessenger, nullptr);
}

void VkWindow::populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VkWindow::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0) throw std::runtime_error("no GPUs with vulkan support found");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());
    
    physicalDevice = chooseDevice(devices);
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
}

VkPhysicalDevice VkWindow::chooseDevice(const std::vector<VkPhysicalDevice>& devices) {
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
    
    return bestDevice;
}

void VkWindow::findQueueFamilyIndices() {
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

void VkWindow::createLogicalDevice() {
    findQueueFamilyIndices();
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {vkQueueFamilyIndices.graphics.value(), vkQueueFamilyIndices.present.value()};
    const float priority = 1;

    for (uint32_t family: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = family;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo createInfo{};
    
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &physicalDeviceFeatures;
    
    createInfo.enabledLayerCount = 0;
    if (enableVkValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(vkRequiredValidationLayers.size());
        createInfo.ppEnabledLayerNames = vkRequiredValidationLayers.data();
    }
    
    createInfo.enabledExtensionCount = static_cast<uint32_t>(vkRequiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = vkRequiredDeviceExtensions.data();
    
    VkCheck(vkCreateDevice(physicalDevice, &createInfo, nullptr, &vkDevice), "vkCreateDevice (VkWindow.cpp)");

    vkGetDeviceQueue(vkDevice, vkQueueFamilyIndices.graphics.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(vkDevice, vkQueueFamilyIndices.present.value(), 0, &presentQueue);
}


void VkWindow::createSurface() {
    VkCheck(glfwCreateWindowSurface(vkInstance, glfwWindow, nullptr, &surface), "glfwCreateWindowSurface (VkWindow.cpp)");
}

void VkWindow::querySwapChainSupport() {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainSupportDetails.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0) {
        swapChainSupportDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, swapChainSupportDetails.formats.data());
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        swapChainSupportDetails.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, swapChainSupportDetails.presentModes.data());
    }
}

void VkWindow::chooseSwapSurfaceFormat() {
    for (const auto &format: swapChainSupportDetails.formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = format;
            return;
        }
    }

    surfaceFormat = swapChainSupportDetails.formats[0];
}

void VkWindow::chooseSwapPresentMode() {
    swapPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (std::find(swapChainSupportDetails.presentModes.begin(), swapChainSupportDetails.presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != swapChainSupportDetails.presentModes.end()) 
        swapPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
}

void VkWindow::chooseSwapExtent() {
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

void VkWindow::createSwapChain() {
    querySwapChainSupport();
    chooseSwapSurfaceFormat();
    chooseSwapPresentMode();
    chooseSwapExtent();
    
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
    
    std::vector<uint32_t> queueFamilies = {vkQueueFamilyIndices.graphics.value(), vkQueueFamilyIndices.present.value()};
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkQueueFamilyIndices.graphics != vkQueueFamilyIndices.present) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilies.data();
    }
    
    createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = swapPresentMode;
    createInfo.clipped = true;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkCheck(vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &swapChain), "vkCreateSwapchainKHR (VkWindow.cpp)");

    vkGetSwapchainImagesKHR(vkDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice, swapChain, &imageCount, swapChainImages.data());
}

void VkWindow::createImageViews() {
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

        VkCheck(vkCreateImageView(vkDevice, &createInfo, nullptr, &swapChainImageViews[i]), "vkCreateImageView (VkWindow.cpp)");
    }
}

void VkWindow::createFramebuffers(VkRenderPass renderPass) {
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

        VkCheck(vkCreateFramebuffer(vkDevice, &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]), "vkCreateFramebuffer (VkWindow.cpp)");
    }

    createCommandPool();
    createCommandBuffer();
    createSyncObjects();
}

void VkWindow::createCommandPool() {
    VkCommandPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolCreateInfo.queueFamilyIndex = vkQueueFamilyIndices.graphics.value();
    VkCheck(vkCreateCommandPool(vkDevice, &poolCreateInfo, nullptr, &commandPool), "vkCreateCommandPool (VkWindow.cpp)");
}

void VkWindow::createCommandBuffer() {
    VkCommandBufferAllocateInfo bufferAllocateInfo{};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferAllocateInfo.commandPool = commandPool;
    bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    bufferAllocateInfo.commandBufferCount = 1;
    VkCheck(vkAllocateCommandBuffers(vkDevice, &bufferAllocateInfo, &commandBuffer), "vkAllocateCommandBuffers (VkWindow.cpp)");
}

void VkWindow::recordCommandBuffer(VkCommandBuffer buffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCheck(vkBeginCommandBuffer(buffer, &beginInfo), "vkBeginCommandBuffer (VkWindow.cpp)");
}

void VkWindow::startCommandBuffer() {
    waitForFence();
    vkAcquireNextImageKHR(vkDevice, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &currentImageIndex);
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, currentImageIndex);
}

void VkWindow::endCommandBuffer() {
    VkCheck(vkEndCommandBuffer(commandBuffer), "vkEndCommandBuffer (VkWindow.cpp)");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    VkCheck(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence), "vkQueueSubmit (VkWindow.cpp)");
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &currentImageIndex;

    VkCheck(vkQueuePresentKHR(presentQueue, &presentInfo), "vkQueuePresentKHR (VkWindow.cpp)");
}

void VkWindow::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkCheck(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphore), "vkCreateSemaphore#1 (VkWindow.cpp)");
    VkCheck(vkCreateSemaphore(vkDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphore), "vkCreateSemaphore#2 (VkWindow.cpp)");
    VkCheck(vkCreateFence(vkDevice, &fenceCreateInfo, nullptr, &inFlightFence), "vkCreateFence (VkWindow.cpp)");
}

void VkWindow::waitForFence() {
    vkWaitForFences(vkDevice, 1, &inFlightFence, true, UINT64_MAX);
    vkResetFences(vkDevice, 1, &inFlightFence);
}

