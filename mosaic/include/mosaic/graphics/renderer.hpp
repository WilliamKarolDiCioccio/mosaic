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
    web_gpu,
    vulkan,
    none
};

class MOSAIC_API Renderer
{
   private:
    std::unique_ptr<RendererAPI> s_rendererAPI = nullptr;
    RendererAPIType m_apiType = RendererAPIType::none;

   public:
    Renderer() = default;
    ~Renderer() = default;

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

   public:
    void setAPI(RendererAPIType _apiType);
    void initialize(const Window* _window);
    void shutdown();
    void render();

    inline static Renderer& getGlobalRendererAPI()
    {
        static std::unique_ptr<Renderer> instance = std::make_unique<Renderer>();
        return *instance;
    }
};

} // namespace graphics
} // namespace mosaic
