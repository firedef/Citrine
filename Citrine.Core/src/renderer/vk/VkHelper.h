#ifndef CITRINE_VKHELPER_H
#define CITRINE_VKHELPER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

#define VkCheck(result, phase) if (result != VK_SUCCESS) throw std::runtime_error("vulkan error in " phase);

namespace VkHelper {
    static void Initialize() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
};


#endif //CITRINE_VKHELPER_H
