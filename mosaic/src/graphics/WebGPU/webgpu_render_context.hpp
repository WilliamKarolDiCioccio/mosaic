#pragma once

#include "mosaic/graphics/renderer_context.hpp"
#include "webgpu_common.hpp"

namespace mosaic
{
namespace graphics
{
namespace webgpu
{

class WebGPURenderContext : public RenderContext
{
   public:
    WebGPURenderContext(const core::Window* _window, const RenderContextSettings& _settings);
    ~WebGPURenderContext();

    void resizeFramebuffer() override;
    void beginFrame() override;
    void updateResources() override;
    void drawScene() override;
    void endFrame() override;

   private:
    std::pair<WGPUSurfaceTexture, WGPUTextureView> getNextSurfaceViewData();
    void pollDevice(int _times = 5);

    WGPUInstance m_instance = nullptr;
    WGPUSurface m_surface = nullptr;
    WGPUAdapter m_adapter = nullptr;
    WGPUDevice m_device = nullptr;
    WGPUQueue m_presentQueue = nullptr;
};

} // namespace webgpu
} // namespace graphics
} // namespace mosaic
