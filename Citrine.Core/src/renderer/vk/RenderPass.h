#ifndef CITRINE_RENDERPASS_H
#define CITRINE_RENDERPASS_H

#include <vector>
#include <string>
#include <fstream>
#include <vulkan/vulkan.h>
#include "VkWindow.h"
#include "VkHelper.h"

class RenderPass {
private:
    VkWindow& win;
public:
    VkRenderPass renderPass;
    
    explicit RenderPass(VkWindow& window) : win(window) {}
    void createRenderPass();
    void startRenderPass();
    void endRenderPass();
    void destroyRenderPass();
    void recreateRenderPass();
};


#endif //CITRINE_RENDERPASS_H
