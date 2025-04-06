#include "mosaic/graphics/renderer.hpp"

#include "WebGPU/webgpu_renderer_api.hpp"
#include "Vulkan/vulkan_renderer_api.hpp"

namespace mosaic
{
namespace graphics
{

std::unique_ptr<RendererAPI> Renderer::s_rendererAPI = nullptr;

Renderer::~Renderer() { s_rendererAPI->shutdown(); }

void Renderer::setAPI(RendererAPIType _apiType)
{
    if (s_rendererAPI)
    {
        s_rendererAPI->shutdown();
    }

    switch (_apiType)
    {
        case RendererAPIType::WebGPU:
            s_rendererAPI = std::make_unique<webgpu::WebGPURendererAPI>();
            break;
        case RendererAPIType::Vulkan:
            s_rendererAPI = std::make_unique<vulkan::VulkanRendererAPI>();
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

} // namespace graphics
} // namespace mosaic
