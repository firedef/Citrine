#ifndef CITRINE_GRAPHICSPIPELINE_H
#define CITRINE_GRAPHICSPIPELINE_H

#include <vector>
#include <string>
#include <fstream>
#include <vulkan/vulkan.h>
#include "VkWindow.h"
#include "VkHelper.h"

class GraphicsPipeline {
private:
    std::vector<char> vertexShaderCode;
    std::vector<char> fragmentShaderCode;
    std::vector<char> geometryShaderCode;
    std::vector<char> tesselationShaderCode;
    VkWindow& win;
    
    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;
    VkPipelineLayout pipelineLayout;
    
    VkPipeline graphicsPipeline;
    
    [[nodiscard]] VkShaderModule createShaderModule(const std::vector<char>& src) const;
    static std::vector<char> readFile(const std::string& filename);
public:
    explicit GraphicsPipeline(VkWindow& window) : win(window) {}
    
    void loadVertexShader(const std::string& path);
    void loadFragmentShader(const std::string& path);
    void createPipeline(VkRenderPass renderPass);
    void bindPipeline();
    void destroyPipeline();
};


#endif //CITRINE_GRAPHICSPIPELINE_H
