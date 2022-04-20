
#include "RenderPass.h"

void RenderPass::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = win.surfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo passCreateInfo{};
    passCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    passCreateInfo.attachmentCount = 1;
    passCreateInfo.pAttachments = &colorAttachment;
    passCreateInfo.subpassCount = 1;
    passCreateInfo.pSubpasses = &subpass;
    passCreateInfo.dependencyCount = 1;
    passCreateInfo.pDependencies = &dependency;

    VkCheck(vkCreateRenderPass(win.vkDevice, &passCreateInfo, nullptr, &renderPass), "vkCreateRenderPass (RenderPass.cpp)");
}

void RenderPass::destroyRenderPass() {
    vkDestroyRenderPass(win.vkDevice, renderPass, nullptr);
}

void RenderPass::startRenderPass() {
    uint32_t imageIndex = win.currentImageIndex;
    
    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = win.swapChainFramebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0,0};
    renderPassBeginInfo.renderArea.extent = win.swapExtent;

    VkClearValue clearColor = {{{1,0,0,1}}};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(win.commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::endRenderPass() {
    vkCmdEndRenderPass(win.commandBuffer);
}
