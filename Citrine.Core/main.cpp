#include <iostream>
#include "src/renderer/vk/VkHelper.h"
#include "src/renderer/vk/RenderPass.h"
#include "src/renderer/vk/VkWindow.h"
#include "src/renderer/vk/GraphicsPipeline.h"

int main() {
    VkHelper::Initialize();
    VkWindow win = VkWindow();
    
    RenderPass pass(win);
    pass.createRenderPass();
    
    GraphicsPipeline pipeline(win);
    pipeline.loadVertexShader("shaders/basic/basic.vert");
    pipeline.loadFragmentShader("shaders/basic/basic.frag");
    pipeline.createPipeline(pass.renderPass);
    
    win.createFramebuffers(pass.renderPass);

    while (!glfwWindowShouldClose(win.glfwWindow)) {
        glfwPollEvents();
        
        win.startCommandBuffer();
        pass.startRenderPass();
        pipeline.bindPipeline();
        pass.endRenderPass();
        win.endCommandBuffer();
    }
    
    pass.destroyRenderPass();
    pipeline.destroyPipeline();
    win.Close();
    return 0;
}
