#include <iostream>
#include "src/renderer/vk/VkHelper.h"
#include "src/renderer/vk/RenderPass.h"
#include "src/renderer/vk/VkWindow.h"
#include "src/renderer/vk/GraphicsPipeline.h"
#include <glm/glm.hpp>


bool iconified = true;
int width, height;
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

    glfwMakeContextCurrent(win.glfwWindow);
    iconified = glfwGetWindowAttrib(win.glfwWindow, GLFW_ICONIFIED);
    glfwSetWindowIconifyCallback(win.glfwWindow, [](GLFWwindow* window, int m){
        iconified = m;
    });
    glfwSetFramebufferSizeCallback(win.glfwWindow, [](GLFWwindow* window, int w, int h) {
        width = w;
        height = h;
    });

    glfwGetWindowSize(win.glfwWindow, &width, &height);
    
    
    
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
        
        if (iconified || width < 5 || height < 5) continue;
        
        if (!win.startCommandBuffer()) {
            pipeline.destroyPipeline();
            pass.destroyRenderPass();
            
            win.recreateSwapChain();
            
            pass.createRenderPass();
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