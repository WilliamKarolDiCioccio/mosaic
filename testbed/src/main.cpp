#include <iostream>
#include <mosaic/core/application.hpp>
#include <mosaic/core/logger.hpp>
#include <mosaic/graphics/renderer.hpp>
#include <mosaic/graphics/window.hpp>
#include <mosaic/utils/memory_leak.hpp>

class TestbedApplication : public mosaic::core::Application
{
   private:
    mosaic::graphics::Window m_window{"Testbed", {1280, 720}};

   public:
    TestbedApplication() : Application("Testbed") {}

   private:
    void onInitialize() override
    {
        auto& renderer = mosaic::graphics::Renderer::getGlobalRendererAPI();

        renderer.setAPI(mosaic::graphics::RendererAPIType::web_gpu);
        renderer.initialize(m_window);

        MOSAIC_INFO("Testbed initialized.");
    }

    void onUpdate() override
    {
        auto& renderer = mosaic::graphics::Renderer::getGlobalRendererAPI();

        if (m_window.shouldClose())
        {
            return shutdown();
        }

        renderer.render();

        m_window.update();
    }

    void onPause() override { MOSAIC_INFO("Testbed paused."); }

    void onResume() override { MOSAIC_INFO("Testbed resumed."); }

    void onShutdown() override
    {
        auto& renderer = mosaic::graphics::Renderer::getGlobalRendererAPI();

        renderer.shutdown();

        MOSAIC_INFO("Testbed shutdown.");
    }
};

int main()
{
#ifdef _DEBUG
    mosaic::utils::enableMemoryLeakDetection();
#endif

    TestbedApplication app;
    app.initialize();
    app.update();

    return 0;
}
