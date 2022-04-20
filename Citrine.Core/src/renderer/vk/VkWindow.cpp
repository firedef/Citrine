#include "VkWindow.h"

void VkWindow::initVulkan() {
    createInstance();
    VkCheck(glfwCreateWindowSurface(vkInstance.instance, glfwWindow, nullptr, &surface), "glfwCreateWindowSurface (VkWindow.cpp)");
    physicalDevice.create(vkInstance.instance);
    createLogicalDevice();
    swapChain.create(glfwWindow, physicalDevice, device, surface, queues);
}

void VkWindow::createInstance() {
    vkInstance.create(vkRequiredValidationLayers);
}

VkWindow::VkWindow() {
    createGlfwWindow();
    initVulkan();
}

void VkWindow::Close() {
    commandPool.destroy(device);
    
    swapChain.destroy(device);
    
    device.destroy();
    vkDestroySurfaceKHR(vkInstance.instance, surface, nullptr);
    vkInstance.destroy();
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

void VkWindow::createLogicalDevice() {
    device = {};
    device.create(physicalDevice.physicalDevice, surface, vkRequiredValidationLayers, vkRequiredDeviceExtensions);
    queues.graphicsIndex = device.vkQueueFamilyIndices.graphics.value();
    queues.presentIndex = device.vkQueueFamilyIndices.present.value();
    queues.get(device.device);
}

void VkWindow::createFramebuffers(VkRenderPass renderPass) {
    swapChain.createFramebuffers(renderPass, device);
    createCommandPool();
}

void VkWindow::createCommandPool() {
    commandPool.create(queues, device);
}

bool VkWindow::startCommandBuffer() {
    vkWaitForFences(device.device, 1, &commandPool.currentInFlightFence(), true, UINT64_MAX);
    if (vkAcquireNextImageKHR(device.device, swapChain.swapChain, UINT64_MAX, commandPool.currentImageAvailableSemaphore(), VK_NULL_HANDLE, &swapChain.currentImageIndex) == VK_ERROR_OUT_OF_DATE_KHR)
        return false;
    vkResetFences(device.device, 1, &commandPool.currentInFlightFence());
    commandPool.currentCommandBuffer().reset();
    commandPool.currentCommandBuffer().record();
    return true;
}

void VkWindow::endCommandBuffer() {
    commandPool.currentCommandBuffer().end();

    std::vector<VkSemaphore> waitSemaphores = {commandPool.currentImageAvailableSemaphore()};
    std::vector<VkSemaphore> signalSemaphores = {commandPool.currentRenderFinishedSemaphore()};
    queues.Submit(waitSemaphores, signalSemaphores, commandPool.currentInFlightFence(), commandPool.currentCommandBuffer().vk);
    
    if (queues.Present({swapChain.swapChain}, signalSemaphores, &swapChain.currentImageIndex)) commandPool.currentFrameIndex = (commandPool.currentFrameIndex + 1) % maxFramesInFlight;
    else commandPool.currentFrameIndex = 0;
}

void VkWindow::recreateSwapChain() {
    device.WaitIdle();
    swapChain.recreate(glfwWindow, physicalDevice, device, surface, queues);
}


