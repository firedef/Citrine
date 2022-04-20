#ifndef CITRINE_QUEUES_H
#define CITRINE_QUEUES_H

#include "VkHelper.h"
#include <vector>

struct Queues {
    VkQueue graphics;
    VkQueue present;
    uint32_t graphicsIndex;
    uint32_t presentIndex;
    
    void get(VkDevice device) {
        vkGetDeviceQueue(device, graphicsIndex, 0, &graphics);
        vkGetDeviceQueue(device, presentIndex, 0, &present);
    }
    
    void Submit(std::vector<VkSemaphore> waitSemaphores, std::vector<VkSemaphore> signalSemaphores, VkFence inFlightFence, VkCommandBuffer commandBuffer) {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.pWaitDstStageMask = waitStages;
        
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        
        submitInfo.signalSemaphoreCount = signalSemaphores.size();
        submitInfo.pSignalSemaphores = signalSemaphores.data();

        VkCheck(vkQueueSubmit(graphics, 1, &submitInfo, inFlightFence), "vkQueueSubmit (Queues.h)");
    }
    
    bool Present(std::vector<VkSwapchainKHR> swapChains, std::vector<VkSemaphore> signalSemaphores, uint32_t* currentImageIndex) {
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = signalSemaphores.size();
        presentInfo.pWaitSemaphores = signalSemaphores.data();

        presentInfo.swapchainCount = swapChains.size();
        presentInfo.pSwapchains = swapChains.data();
        presentInfo.pImageIndices = currentImageIndex;
        
        VkResult presentResult = vkQueuePresentKHR(present, &presentInfo);

        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR) return false;
        VkCheck(presentResult, "vkQueuePresentKHR (VkWindow.cpp)");
        return true;
    }
};

#endif //CITRINE_QUEUES_H
