
#include "GraphicsPipeline.h"

std::vector<char> GraphicsPipeline::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) throw std::runtime_error("failed to open file '" + filename + "'");
    
    size_t size = file.tellg();
    std::vector<char> buffer(size);
    
    file.seekg(0);
    file.read(buffer.data(), (std::streamsize) size);
    
    file.close();
    return buffer;
}

void GraphicsPipeline::loadVertexShader(const std::string& path) {
    vertexShaderCode = readFile(path);
}

void GraphicsPipeline::loadFragmentShader(const std::string& path) {
    fragmentShaderCode = readFile(path);
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char>& src) const {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = src.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

    VkShaderModule shaderModule;
    VkCheck(vkCreateShaderModule(win.device.device, &createInfo, nullptr, &shaderModule), "vkCreateShaderModule (GraphicsPipeline.cpp)")
    return shaderModule;
}

void GraphicsPipeline::createPipeline(VkRenderPass renderPass) {
    vertexShader = createShaderModule(vertexShaderCode);
    fragmentShader = createShaderModule(fragmentShaderCode);
    
    VkPipelineShaderStageCreateInfo vertCreateInfo{};
    vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertCreateInfo.module = vertexShader;
    vertCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragCreateInfo{};
    fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragCreateInfo.module = fragmentShader;
    fragCreateInfo.pName = "main";
    
    VkPipelineVertexInputStateCreateInfo vertInputCreateInfo{};
    vertInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertInputCreateInfo.vertexBindingDescriptionCount = 0;
    vertInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertInputCreateInfo.vertexAttributeDescriptionCount = 0;
    vertInputCreateInfo.pVertexAttributeDescriptions = nullptr;
    
    VkPipelineInputAssemblyStateCreateInfo inputAsmCreateInfo{};
    inputAsmCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAsmCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAsmCreateInfo.primitiveRestartEnable = false;
    
    VkViewport viewport{};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<float>(win.swapChain.swapExtent.width);
    viewport.height = static_cast<float>(win.swapChain.swapExtent.height);
    viewport.minDepth = 0;
    viewport.maxDepth = 1;
    
    VkRect2D scissor{};
    scissor.offset = {0,0};
    scissor.extent = win.swapChain.swapExtent;
    
    VkPipelineViewportStateCreateInfo viewportCreateInfo{};
    viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportCreateInfo.viewportCount = 1;
    viewportCreateInfo.pViewports = &viewport;
    viewportCreateInfo.scissorCount = 1;
    viewportCreateInfo.pScissors = &scissor;
    
    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
    rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthClampEnable = false;
    rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable = false;
    
    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable = false;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    //VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
    
    VkPipelineColorBlendAttachmentState colorBlendCreateInfo{};
    colorBlendCreateInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendCreateInfo.blendEnable = false;
    colorBlendCreateInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendCreateInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendCreateInfo.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendCreateInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendCreateInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendCreateInfo.alphaBlendOp = VK_BLEND_OP_ADD;
    
    VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo{};
    colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingCreateInfo.logicOpEnable = false;
    colorBlendingCreateInfo.attachmentCount = 1;
    colorBlendingCreateInfo.pAttachments = &colorBlendCreateInfo;
    
    VkPipelineLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkCheck(vkCreatePipelineLayout(win.device.device, &layoutCreateInfo, nullptr, &pipelineLayout), "vkCreatePipelineLayout (GraphicsPipeline.cpp)");

    VkPipelineShaderStageCreateInfo stages[] = {vertCreateInfo, fragCreateInfo};
    
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = stages;
    pipelineCreateInfo.pVertexInputState = &vertInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAsmCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.subpass = 0;

    VkCheck(vkCreateGraphicsPipelines(win.device.device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline), "vkCreateGraphicsPipelines (GraphicsPipeline.cs)");
   
    vkDestroyShaderModule(win.device.device, vertexShader, nullptr);
    vkDestroyShaderModule(win.device.device, fragmentShader, nullptr);
    currentRenderPass = renderPass;
}

void GraphicsPipeline::destroyPipeline() {
    vkDestroyPipeline(win.device.device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(win.device.device, pipelineLayout, nullptr);
}

void GraphicsPipeline::bindPipeline() {
    vkCmdBindPipeline(win.commandPool.currentCommandBuffer().vk, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdDraw(win.commandPool.currentCommandBuffer().vk, 3, 1, 0, 0);
}

void GraphicsPipeline::recreatePipeline() {
    destroyPipeline();
    createPipeline(currentRenderPass);
}

