#pragma once

#include "mosaic/graphics/renderer_api.hpp"
#include "shared.hpp"

namespace mosaic
{
namespace graphics
{
namespace webgpu
{

class WebGPURendererAPI : public RendererAPI
{
   public:
    WebGPURendererAPI() = default;
    ~WebGPURendererAPI() override = default;

    void initialize(const Window& _window) override;
    void shutdown() override;
    void beginFrame() override;
    void updateResources() override;
    void drawScene() override;
    void endFrame() override;

   private:
    WGPUTextureView getNextSurfaceTextureView();
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
