#pragma once

#include "mosaic/graphics/renderer_context.hpp"

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

class VulkanRenderContext : public RenderContext
{
   private:
    struct FrameData
    {
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;
        CommandBuffer commandBuffer;

        FrameData()
            : imageAvailableSemaphore(nullptr),
              renderFinishedSemaphore(nullptr),
              inFlightFence(nullptr),
              commandBuffer(nullptr) {};
    };

   public:
    VulkanRenderContext(const core::Window* _window, const RenderContextSettings& _settings);
    ~VulkanRenderContext();

    void resizeFramebuffer() override;
    void beginFrame() override;
    void updateResources() override;
    void drawScene() override;
    void endFrame() override;

   private:
    void createFrames();
    void destroyFrames();

    Instance m_instance;
    Device m_device;
    Surface m_surface;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    Pipeline m_pipeline;
    CommandPool m_commandPool;

    uint32_t m_currentFrame = 0;
    std::vector<FrameData> m_frameData;

    bool m_framebufferResized = false;
};

} // namespace vulkan
} // namespace graphics
} // namespace mosaic
