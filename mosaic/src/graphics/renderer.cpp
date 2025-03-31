#include "mosaic/graphics/renderer.hpp"

#include "WebGPU/web_gpu_renderer_api.hpp"

namespace mosaic::graphics
{

Renderer::~Renderer() { s_rendererAPI->shutdown(); }

void Renderer::setAPI(RendererAPIType _apiType)
{
    switch (_apiType)
    {
        case RendererAPIType::WebGPU:
            s_rendererAPI = std::make_unique<WebGPURendererAPI>();
            break;
    }
}

void Renderer::initialize(const Window& _window) { s_rendererAPI->initialize(_window); }

void Renderer::render()
{
    s_rendererAPI->beginFrame();
    s_rendererAPI->updateResources();
    s_rendererAPI->drawScene();
    s_rendererAPI->endFrame();
}

} // namespace mosaic::graphics
