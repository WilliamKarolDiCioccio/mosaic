#pragma once

#include "mosaic/graphics/renderer_api.hpp"
#include "shared.hpp"

namespace mosaic::graphics::webgpu
{

class WebGPURendererAPI : public RendererAPI
{
   public:
    WebGPURendererAPI() = default;
    ~WebGPURendererAPI() override = default;

    void initialize(const Window& _window) override;
    void shutdown() override;
    void beginFrame();
    void updateResources();
    void drawScene();
    void endFrame();

   private:
    WGPUTextureView getNextSurfaceTextureView();
    void pollDevice(int _times = 5);

    WGPUInstance m_instance = nullptr;
    WGPUSurface m_surface = nullptr;
    WGPUAdapter m_adapter = nullptr;
    WGPUDevice m_device = nullptr;
    WGPUQueue m_presentQueue = nullptr;
};

} // namespace mosaic::graphics::webgpu
