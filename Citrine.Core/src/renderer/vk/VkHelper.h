#ifndef CITRINE_VKHELPER_H
#define CITRINE_VKHELPER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

#define VkCheck(result, phase)              \
{                                           \
    VkResult r = (result);                  \
    [[unlikely]]                            \
    if (r != VK_SUCCESS) {                  \
        std::string msg;                    \
        msg.append("vulkan error in ");     \
        msg.append(phase);                  \
        msg.append(": ");                   \
        msg.append(std::to_string(r));      \
        throw std::runtime_error(msg);      \
    }                                       \
}

#ifdef NDEBUG
const bool enableVkValidationLayers = false;
#else
const bool enableVkValidationLayers = true;
#endif

namespace VkHelper {
    static void Initialize() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
};


#endif //CITRINE_VKHELPER_H
