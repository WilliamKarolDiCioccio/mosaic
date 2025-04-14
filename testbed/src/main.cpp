#include <iostream>
#include <mosaic/core/application.hpp>
#include <mosaic/core/logger.hpp>
#include <mosaic/graphics/window.hpp>
#include <mosaic/core/timer.hpp>
#include <mosaic/input/input_system.hpp>
#include <mosaic/graphics/renderer.hpp>
#include <mosaic/utils/memory_leak.hpp>

using namespace mosaic;

class TestbedApplication : public mosaic::core::Application
{
   private:
    mosaic::graphics::Window m_window{"Testbed", {1280, 720}};

   public:
    TestbedApplication() : Application("Testbed") {}

   private:
    void onInitialize() override
    {
        input::InputSystem& inputSystem = input::InputSystem::getGlobalInputSystem();

        auto inputContext = inputSystem.registerWindow(m_window);

        inputContext->updateVirtualKeyboardKeys({
            {"closeApp", input::KeyboardKey::key_escape},
            {"moveLeft", input::KeyboardKey::key_a},
            {"moveRight", input::KeyboardKey::key_d},
            {"moveUp", input::KeyboardKey::key_w},
            {"moveDown", input::KeyboardKey::key_s},
            {"resetCamera", input::KeyboardKey::key_r},
        });

        inputContext->registerActions({
            {
                "moveLeft",
                {
                    input::KeyboardKeyActionTrigger{
                        {"moveLeft"},
                        input::KeyboardKey::key_a,
                        [](const std::unordered_map<std::string, input::KeyboardKeyEvent>& events)
                        {
                            return utils::hasFlag(events.at("moveLeft").state,
                                                  input::KeyButtonState::hold);
                        },
                    },
                },
            },
            {
                "moveRight",
                {
                    input::KeyboardKeyActionTrigger{
                        {"moveRight"},
                        input::KeyboardKey::key_d,
                        [](const std::unordered_map<std::string, input::KeyboardKeyEvent>& events)
                        {
                            return utils::hasFlag(events.at("moveRight").state,
                                                  input::KeyButtonState::hold);
                        },
                    },
                },
            },
            {
                "moveUp",
                {
                    input::KeyboardKeyActionTrigger{
                        {"moveUp"},
                        input::KeyboardKey::key_w,
                        [](const std::unordered_map<std::string, input::KeyboardKeyEvent>& events)
                        {
                            return utils::hasFlag(events.at("moveUp").state,
                                                  input::KeyButtonState::hold);
                        },
                    },
                },
            },
            {
                "moveDown",
                {
                    input::KeyboardKeyActionTrigger{
                        {"moveDown"},
                        input::KeyboardKey::key_s,
                        [](const std::unordered_map<std::string, input::KeyboardKeyEvent>& events)
                        {
                            return utils::hasFlag(events.at("moveDown").state,
                                                  input::KeyButtonState::hold);
                        },
                    },
                },
            },
            {
                "resetCamera",
                {
                    input::KeyboardKeyActionTrigger{
                        {"resetCamera"},
                        input::KeyboardKey::key_r,
                        [](const std::unordered_map<std::string, input::KeyboardKeyEvent>& events)
                        {
                            return utils::hasFlag(events.at("resetCamera").state,
                                                  input::KeyButtonState::press);
                        },
                    },
                },
            },
            {
                "closeApp",
                {
                    input::KeyboardKeyActionTrigger{
                        {"closeApp"},
                        input::KeyboardKey::key_escape,
                        [](const std::unordered_map<std::string, input::KeyboardKeyEvent>& events)
                        {
                            return utils::hasFlag(events.at("closeApp").state,
                                                  input::KeyButtonState::double_press);
                        },
                    },
                },
            },
        });

        graphics::Renderer& renderer = graphics::Renderer::getGlobalRendererAPI();

        renderer.setAPI(graphics::RendererAPIType::vulkan);
        renderer.initialize(&m_window);

        m_window.setResizeable(true);

        MOSAIC_INFO("Testbed initialized.");
    }

    void onUpdate() override
    {
        core::Timer::tick();

        graphics::Renderer& renderer = graphics::Renderer::getGlobalRendererAPI();

        renderer.render();

        input::InputSystem& inputSystem = input::InputSystem::getGlobalInputSystem();

        inputSystem.updateContexts();

        input::InputContext* inputContext = inputSystem.getContext(m_window);

        if (inputContext->isActionTriggered("moveLeft"))
        {
            MOSAIC_INFO("Moving left.");
        }

        if (inputContext->isActionTriggered("moveRight"))
        {
            MOSAIC_INFO("Moving right.");
        }

        if (inputContext->isActionTriggered("moveUp"))
        {
            MOSAIC_INFO("Moving up.");
        }

        if (inputContext->isActionTriggered("moveDown"))
        {
            MOSAIC_INFO("Moving down.");
        }

        if (inputContext->isActionTriggered("resetCamera"))
        {
            MOSAIC_INFO("Resetting camera.");
        }

        if (m_window.shouldClose() || inputContext->isActionTriggered("closeApp"))
        {
            return shutdown();
        }
    }

    void onPause() override { MOSAIC_INFO("Testbed paused."); }

    void onResume() override { MOSAIC_INFO("Testbed resumed."); }

    void onShutdown() override
    {
        auto& renderer = graphics::Renderer::getGlobalRendererAPI();

        renderer.shutdown();

        auto& inputManager = input::InputSystem::getGlobalInputSystem();

        inputManager.unregisterWindow(m_window);

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
