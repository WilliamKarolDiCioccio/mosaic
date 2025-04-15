#include "vulkan_renderer_api.hpp"

namespace mosaic
{
namespace graphics
{
namespace vulkan
{

void VulkanRendererAPI::initialize(const core::Window* _window)
{
    m_window = _window;

    createInstance(m_instance);
    createSurface(m_surface, m_instance, _window->getGLFWHandle());
    createDevice(m_device, m_instance, m_surface);
    createSwapchain(m_swapchain, m_device, m_surface, _window);
    createRenderPass(m_renderPass, m_device, m_swapchain);
    createGraphicsPipeline(m_pipeline, m_device, m_swapchain, m_renderPass);
    createSwapchainFramebuffers(m_swapchain, m_device, m_renderPass);
    createCommandPool(m_commandPool, m_device, m_surface);

    createFrames();

    const_cast<core::Window*>(m_window)->registerWindowResizeCallback(
        [this](GLFWwindow* _window, int _width, int _height) { m_framebufferResized = true; });
}

void VulkanRendererAPI::shutdown()
{
    vkDeviceWaitIdle(m_device.device);

    destroyFrames();

    destroyCommandPool(m_commandPool, m_device);
    destroySwapchainFramebuffers(m_swapchain, m_device);
    destroyGraphicsPipeline(m_pipeline, m_device);
    destroyRenderPass(m_renderPass, m_device);
    destroySwapchain(m_swapchain);
    destroyDevice(m_device);
    destroySurface(m_surface, m_instance);
    destroyInstance(m_instance);
}

void VulkanRendererAPI::recreateSwapchain()
{
    auto framebufferSize = m_window->getFramebufferSize();

    while (framebufferSize.x == 0 || framebufferSize.y == 0)
    {
        glfwWaitEvents();
        framebufferSize = m_window->getFramebufferSize();
    }

    vkDeviceWaitIdle(m_device.device);

    destroySwapchainFramebuffers(m_swapchain, m_device);
    destroyGraphicsPipeline(m_pipeline, m_device);
    destroyRenderPass(m_renderPass, m_device);
    destroySwapchain(m_swapchain);

    createSwapchain(m_swapchain, m_device, m_surface, m_window);
    createRenderPass(m_renderPass, m_device, m_swapchain);
    createGraphicsPipeline(m_pipeline, m_device, m_swapchain, m_renderPass);
    createSwapchainFramebuffers(m_swapchain, m_device, m_renderPass);

    m_framebufferResized = false;
}

void VulkanRendererAPI::beginFrame()
{
    auto& frame = m_frameData[m_currentFrame];

    vkWaitForFences(m_device.device, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult acquireResult =
        vkAcquireNextImageKHR(m_device.device, m_swapchain.swapchain, UINT64_MAX,
                              frame.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return recreateSwapchain();
    }
    else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(m_device.device, 1, &frame.inFlightFence);

    vkResetCommandBuffer(frame.commandBuffer, 0);

    // Begin command buffer recording

    beingCommandBuffer(frame.commandBuffer, m_device, m_surface);

    bindGraphicsPipeline(m_pipeline, m_device, frame.commandBuffer);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapchain.extent.width);
    viewport.height = static_cast<float>(m_swapchain.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapchain.extent;

    vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);

    beginRenderPass(m_renderPass, m_device, m_swapchain, frame.commandBuffer, imageIndex);

    vkCmdDraw(frame.commandBuffer, 3, 1, 0, 0);

    endRenderPass(m_renderPass, m_device, frame.commandBuffer);

    endCommandBuffer(frame.commandBuffer, m_device);

    // End command buffer recording

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {frame.imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &frame.commandBuffer;

    VkSemaphore signalSemaphores[] = {frame.renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_device.graphicsQueue, 1, &submitInfo, frame.inFlightFence) != VK_SUCCESS)
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

    VkResult presentResult = vkQueuePresentKHR(m_device.presentQueue, &presentInfo);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR ||
        m_framebufferResized)
    {
        recreateSwapchain();
    }
    else if (presentResult != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRendererAPI::updateResources() {}

void VulkanRendererAPI::drawScene() {}

void VulkanRendererAPI::endFrame() {}

void VulkanRendererAPI::createFrames()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (auto& frame : m_frameData)
    {
        createCommandBuffer(frame.commandBuffer, m_device, m_commandPool);

        if (vkCreateSemaphore(m_device.device, &semaphoreInfo, nullptr,
                              &frame.imageAvailableSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(m_device.device, &semaphoreInfo, nullptr,
                              &frame.renderFinishedSemaphore) != VK_SUCCESS ||
            vkCreateFence(m_device.device, &fenceInfo, nullptr, &frame.inFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanRendererAPI::destroyFrames()
{
    for (auto& frame : m_frameData)
    {
        vkDestroySemaphore(m_device.device, frame.imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(m_device.device, frame.renderFinishedSemaphore, nullptr);
        vkDestroyFence(m_device.device, frame.inFlightFence, nullptr);

        destroyCommandBuffer(frame.commandBuffer, m_device, m_commandPool);
    }
}

} // namespace vulkan
} // namespace graphics
} // namespace mosaic
