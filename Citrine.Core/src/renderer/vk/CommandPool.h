#ifndef CITRINE_COMMANDPOOL_H
#define CITRINE_COMMANDPOOL_H

#include "VkHelper.h"
#include "CommandBuffer.h"
#include "Queues.h"
#include "LogicalDevice.h"
#include <vector>

struct CommandPool {
private:
    void createCommandPool(Queues& queues, LogicalDevice& device) {
        VkCommandPoolCreateInfo poolCreateInfo{};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCreateInfo.queueFamilyIndex = queues.graphicsIndex;
        VkCheck(vkCreateCommandPool(device.device, &poolCreateInfo, nullptr, &commandPool), "vkCreateCommandPool (VkWindow.cpp)");
    }
    
    void createCommandBuffers(LogicalDevice& device) {
        int count = maxFramesInFlight;
        for (int i = 0; i < count; ++i) {
            CommandBuffer buffer{};
            buffer.create(commandPool, device.device);
            commandBuffers.push_back(buffer);
        }
    }
    
    void createSyncObjects(LogicalDevice& device) {
        const int count = maxFramesInFlight;
        imageAvailableSemaphores.resize(count);
        renderFinishedSemaphores.resize(count);
        inFlightFences.resize(count);

        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < count; ++i) {
            VkCheck(vkCreateSemaphore(device.device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]), "vkCreateSemaphore#1 (CommandPool.h)");
            VkCheck(vkCreateSemaphore(device.device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]), "vkCreateSemaphore#2 (CommandPool.h)");
            VkCheck(vkCreateFence(device.device, &fenceCreateInfo, nullptr, &inFlightFences[i]), "vkCreateFence (CommandPool.h)");
        }
    }
public:
    const int maxFramesInFlight = 2;

    VkCommandPool commandPool;
    std::vector<CommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    
    uint32_t currentFrameIndex = 0;
    
    CommandBuffer& currentCommandBuffer() { return commandBuffers[currentFrameIndex]; }
    VkSemaphore& currentImageAvailableSemaphore() { return imageAvailableSemaphores[currentFrameIndex]; }
    VkSemaphore& currentRenderFinishedSemaphore() { return renderFinishedSemaphores[currentFrameIndex]; }
    VkFence& currentInFlightFence() { return inFlightFences[currentFrameIndex]; }
    
    void create(Queues& queues, LogicalDevice& device) {
        createCommandPool(queues, device);
        createCommandBuffers(device);
        createSyncObjects(device);
    }
    
    void destroy(LogicalDevice& device) {
        for (int i = 0; i < maxFramesInFlight; ++i) {
            vkDestroySemaphore(device.device, imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(device.device, renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device.device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(device.device, commandPool, nullptr);
    }
};

#endif //CITRINE_COMMANDPOOL_H
