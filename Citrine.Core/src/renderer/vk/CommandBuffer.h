#ifndef CITRINE_COMMANDBUFFER_H
#define CITRINE_COMMANDBUFFER_H

#include "VkHelper.h"

struct CommandBuffer {
    VkCommandBuffer vk;
    
    void create(VkCommandPool pool, VkDevice device) {
        VkCommandBufferAllocateInfo bufferAllocateInfo{};
        bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        bufferAllocateInfo.commandPool = pool;
        bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        bufferAllocateInfo.commandBufferCount = 1;
        
        VkCheck(vkAllocateCommandBuffers(device, &bufferAllocateInfo, &vk), "vkAllocateCommandBuffers (CommandBuffer.h)");
    }
    
    void reset() const {
        vkResetCommandBuffer(vk, 0);
    }
    
    void record() const {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VkCheck(vkBeginCommandBuffer(vk, &beginInfo), "vkBeginCommandBuffer (CommandBuffer.h)");
    }
    
    void end() const {
        VkCheck(vkEndCommandBuffer(vk), "vkEndCommandBuffer (CommandBuffer.h)");
    }
};

#endif //CITRINE_COMMANDBUFFER_H
