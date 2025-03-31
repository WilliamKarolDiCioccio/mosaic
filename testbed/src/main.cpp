#include <iostream>
#include <mosaic/core/application.hpp>
#include <mosaic/core/logger.hpp>
#include <mosaic/graphics/renderer.hpp>
#include <mosaic/graphics/window.hpp>

class TestbedApplication : public mosaic::core::Application
{
   private:
    mosaic::graphics::Window m_window{"Testbed", {1280, 720}};
    mosaic::graphics::Renderer m_renderer;

   public:
    TestbedApplication() : Application("Testbed") {}

   private:
    void onInitialize() override
    {
        m_renderer.setAPI(mosaic::graphics::RendererAPIType::WebGPU);
        m_renderer.initialize(m_window);

        MOSAIC_INFO("Testbed initialized.");
    }

    void onUpdate() override
    {
        if (m_window.shouldClose())
        {
            return shutdown();
        }

        m_renderer.clearScreen();

        m_window.update();
    }

    void onPause() override { MOSAIC_INFO("Testbed paused."); }

    void onResume() override { MOSAIC_INFO("Testbed resumed."); }

    void onShutdown() override { MOSAIC_INFO("Testbed shutdown."); }
};

int main()
{
    TestbedApplication app;
    app.initialize();
    app.update();

    return 0;
}
