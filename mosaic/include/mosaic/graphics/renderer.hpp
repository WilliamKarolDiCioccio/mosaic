#pragma once

#include <memory>

#include "mosaic/defines.hpp"
#include "renderer_api.hpp"

namespace mosaic::graphics
{

enum class RendererAPIType
{
    WebGPU
};

class MOSAIC_API Renderer
{
   private:
    inline static std::unique_ptr<RendererAPI> s_rendererAPI = nullptr;

   public:
    Renderer() = default;
    ~Renderer();

    void setAPI(RendererAPIType _apiType);
    void initialize(const Window& _window);
    void render();
};

} // namespace mosaic::graphics
