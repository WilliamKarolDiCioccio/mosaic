#include "vulkan_renderer_api.hpp"

namespace mosaic
{
namespace graphics
{
namespace vulkan
{

void VulkanRendererAPI::initialize(const Window& _window)
{
    createInstance(m_instance);
    createSurface(m_surface, m_instance, _window.getGLFWHandle());
    createDevice(m_device, m_instance, m_surface);
    createSwapchain(m_swapchain, m_device, m_surface, _window);
    createRenderPass(m_renderPass, m_device, m_swapchain);
    createGraphicsPipeline(m_pipeline, m_device, m_swapchain, m_renderPass);
    createSwapchainFramebuffers(m_swapchain, m_device, m_renderPass);
    createCommandPool(m_commandPool, m_device, m_surface);
    createCommandBuffer(m_commandBuffer, m_device, m_commandPool);
    createSynchronizationObjects();
}

void VulkanRendererAPI::shutdown()
{
    vkDeviceWaitIdle(m_device.device);

    destroySynchronizationObjects();
    destroyCommandBuffer(m_commandBuffer, m_device, m_commandPool);
    destroyCommandPool(m_commandPool, m_device);
    destroySwapchainFramebuffers(m_swapchain, m_device);
    destroyGraphicsPipeline(m_pipeline, m_device);
    destroyRenderPass(m_renderPass, m_device);
    destroySwapchain(m_swapchain);
    destroyDevice(m_device);
    destroySurface(m_surface, m_instance);
    destroyInstance(m_instance);
}

void VulkanRendererAPI::beginFrame()
{
    vkWaitForFences(m_device.device, 1, &m_inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.device, 1, &m_inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_device.device, m_swapchain.swapchain, UINT64_MAX,
                          m_imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(m_commandBuffer, 0);

    // Begin command buffer recording

    beingCommandBuffer(m_commandBuffer, m_device, m_surface);

    bindGraphicsPipeline(m_pipeline, m_device, m_commandBuffer);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain.extent.width);
    viewport.height = static_cast<float>(m_swapchain.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchain.extent;

    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

    beginRenderPass(m_renderPass, m_device, m_swapchain, m_commandBuffer, imageIndex);

    vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);

    endRenderPass(m_renderPass, m_device, m_commandBuffer);

    endCommandBuffer(m_commandBuffer, m_device);

    // End command buffer recording

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffer;

    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device.graphicsQueue, 1, &submitInfo, m_inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapchain.swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_device.presentQueue, &presentInfo);
}

void VulkanRendererAPI::updateResources() {}

void VulkanRendererAPI::drawScene() {}

void VulkanRendererAPI::endFrame() {}

void VulkanRendererAPI::createSynchronizationObjects()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(m_device.device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore) !=
            VK_SUCCESS ||
        vkCreateSemaphore(m_device.device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore) !=
            VK_SUCCESS ||
        vkCreateFence(m_device.device, &fenceInfo, nullptr, &m_inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
}

void VulkanRendererAPI::destroySynchronizationObjects()
{
    vkDestroySemaphore(m_device.device, m_renderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_device.device, m_imageAvailableSemaphore, nullptr);
    vkDestroyFence(m_device.device, m_inFlightFence, nullptr);
}

} // namespace vulkan
} // namespace graphics
} // namespace mosaic
