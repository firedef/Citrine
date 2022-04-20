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

//    glfwSetFramebufferSizeCallback(win.glfwWindow, [](GLFWwindow* win, int width, int height) {
//        resize = true;
//    });
    
    double prevTime = glfwGetTime();
    int frames = 0;
    while (!glfwWindowShouldClose(win.glfwWindow)) {
        glfwPollEvents();
        double curTime = glfwGetTime();
        if (curTime >= prevTime + 1) {
            prevTime = curTime;
            std::cout << "fps: " << frames << "\n";
            frames = -1;
        }
        frames++;
        
        int width, height;
        glfwGetWindowSize(win.glfwWindow, &width, &height);
        if (width < 5 || height < 5) {
            std::cout << "adsd";
            continue; 
        }
        
        if (!win.startCommandBuffer()) {
            pipeline.destroyPipeline();
            //pass.destroyRenderPass();
            
            win.recreateSwapChain();
            
            //pass.createRenderPass();
            pipeline.createPipeline(pass.renderPass);
            win.createFramebuffers(pass.renderPass);
            
            continue;
        }
        pass.startRenderPass();
        pipeline.bindPipeline();
        pass.endRenderPass();
        win.endCommandBuffer();
    }
    vkDeviceWaitIdle(win.device.device);
    
    pass.destroyRenderPass();
    pipeline.destroyPipeline();
    win.Close();
    return 0;
}