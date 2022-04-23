#ifndef CITRINE_GRAPHICSPIPELINE_H
#define CITRINE_GRAPHICSPIPELINE_H

#include <vector>
#include <string>
#include <fstream>
#include <vulkan/vulkan.h>
#include "VkWindow.h"
#include "VkHelper.h"
#include <array>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 col;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription description{};
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> description{};

        description[0].binding = 0;
        description[0].location = 0;
        description[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        description[0].offset = offsetof(Vertex, pos);

        description[1].binding = 0;
        description[1].location = 1;
        description[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        description[1].offset = offsetof(Vertex, col);

        return description;
    }
};

class GraphicsPipeline {
private:
    std::vector<char> vertexShaderCode;
    std::vector<char> fragmentShaderCode;
    std::vector<char> geometryShaderCode;
    std::vector<char> tesselationShaderCode;
    VkWindow& win;
    VkRenderPass currentRenderPass;
    
    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;
    VkPipelineLayout pipelineLayout;
    
    VkPipeline graphicsPipeline;
    
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMem;

    std::vector<Vertex> vertices = {
            {{-.5f,.5f,0}, {1,1,0}},
            {{0,-.5f,0}, {1,0,1}},
            {{.5f,.5f,0}, {0,1,1}},
    };
    
    [[nodiscard]] VkShaderModule createShaderModule(const std::vector<char>& src) const;
    static std::vector<char> readFile(const std::string& filename);
public:
    explicit GraphicsPipeline(VkWindow& window) : win(window) {}
    
    [[nodiscard]] uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags flags) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(win.physicalDevice.physicalDevice, &memProperties);

        for (int i = 0; i < memProperties.memoryTypeCount; ++i) {
            if ((filter & (1<<i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) return i;
        }
        throw std::runtime_error("failed to find suitable memory type");
    }
    
    void loadVertexShader(const std::string& path);
    void loadFragmentShader(const std::string& path);
    void createPipeline(VkRenderPass renderPass);
    void bindPipeline();
    void destroyPipeline();
    void recreatePipeline();
};


#endif //CITRINE_GRAPHICSPIPELINE_H
