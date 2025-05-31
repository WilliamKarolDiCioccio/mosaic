#include <iostream>
#include <memory>

#include <mosaic/core/application.hpp>
#include <mosaic/core/logger.hpp>
#include <mosaic/core/window.hpp>
#include <mosaic/core/timer.hpp>
#include <mosaic/input/input_system.hpp>
#include <mosaic/graphics/render_system.hpp>

namespace testbed
{

using namespace mosaic;

class TestbedApplication : public mosaic::core::Application
{
   private:
    std::unique_ptr<mosaic::core::Window> m_window;
    std::unique_ptr<mosaic::input::InputSystem> m_inputSystem;
    std::unique_ptr<mosaic::graphics::RenderSystem> m_renderSystem;

   public:
    TestbedApplication() : Application("Testbed") {}

   private:
    void onInitialize() override;
    void onUpdate() override;
    void onPause() override;
    void onResume() override;
    void onShutdown() override;
};

} // namespace testbed
