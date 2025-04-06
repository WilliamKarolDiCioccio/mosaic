#pragma once

#include "mosaic/graphics/renderer_api.hpp"

#include "vulkan_common.hpp"

#include "context/vulkan_instance.hpp"
#include "context/vulkan_device.hpp"
#include "context/vulkan_surface.hpp"
#include "vulkan_swapchain.hpp"
#include "commands/vulkan_render_pass.hpp"
#include "pipelines/vulkan_pipeline.hpp"
#include "vulkan_framebuffers.hpp"
#include "commands/vulkan_command_pool.hpp"
#include "commands/vulkan_command_buffer.hpp"

namespace mosaic
{
namespace graphics
{
namespace vulkan
{

class VulkanRendererAPI : public RendererAPI
{
   public:
    VulkanRendererAPI() = default;
    ~VulkanRendererAPI() override = default;

    void initialize(const Window& _window) override;
    void shutdown() override;
    void beginFrame() override;
    void updateResources() override;
    void drawScene() override;
    void endFrame() override;

   private:
    void createSynchronizationObjects();
    void destroySynchronizationObjects();

    Instance m_instance;
    Device m_device;
    Surface m_surface;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    Pipeline m_pipeline;
    CommandPool m_commandPool;
    CommandBuffer m_commandBuffer;
    VkSemaphore m_imageAvailableSemaphore;
    VkSemaphore m_renderFinishedSemaphore;
    VkFence m_inFlightFence;
};

} // namespace vulkan
} // namespace graphics
} // namespace mosaic
