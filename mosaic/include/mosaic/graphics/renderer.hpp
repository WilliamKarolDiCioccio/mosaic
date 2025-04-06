#pragma once

#include <memory>

#include "mosaic/defines.hpp"
#include "renderer_api.hpp"

namespace mosaic
{
namespace graphics
{

enum class RendererAPIType
{
    WebGPU,
    Vulkan
};

class MOSAIC_API Renderer
{
   private:
    static std::unique_ptr<RendererAPI> s_rendererAPI;

   public:
    Renderer() = default;
    ~Renderer();

    void setAPI(RendererAPIType _apiType);
    void initialize(const Window& _window);
    void render();
};

} // namespace graphics
} // namespace mosaic
