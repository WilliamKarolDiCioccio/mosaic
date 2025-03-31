#pragma once

#include "mosaic/graphics/renderer_api.hpp"
#include "shared.hpp"

namespace mosaic::graphics
{

class WebGPURendererAPI : public RendererAPI
{
   public:
    WebGPURendererAPI() = default;
    ~WebGPURendererAPI() override = default;

    void initialize(const Window& _window) override;
    void shutdown() override;
    void clearScreen() override;

   private:
    WGPUTextureView getNextSurfaceTextureView();

    WGPUInstance m_instance = nullptr;
    WGPUSurface m_surface = nullptr;
    WGPUAdapter m_adapter = nullptr;
    WGPUDevice m_device = nullptr;
    WGPUQueue m_presentQueue = nullptr;
};

} // namespace mosaic::graphics
