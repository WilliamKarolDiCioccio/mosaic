#include <iostream>
#include <memory>

#include <mosaic/core/application.hpp>
#include <mosaic/core/logger.hpp>
#include <mosaic/core/window.hpp>
#include <mosaic/core/timer.hpp>
#include <mosaic/input/input_system.hpp>
#include <mosaic/graphics/render_system.hpp>
#include <mosaic/utils/memory_leak.hpp>

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
    void onInitialize() override
    {
        m_window = std::make_unique<mosaic::core::Window>("Testbed", glm::vec2(1280, 720));
        m_window->setResizeable(true);

        m_inputSystem = std::make_unique<mosaic::input::InputSystem>();

        auto registrationResult = m_inputSystem->registerWindow(m_window.get());

        if (registrationResult.isErr())
        {
            return shutdown();
        }

        auto inputContext = registrationResult.unwrap();

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

        m_renderSystem = graphics::RenderSystem::create(graphics::RendererAPIType::vulkan);

        auto creationResult = m_renderSystem->createContext(m_window.get());

        if (creationResult.isErr())
        {
            return shutdown();
        }

        MOSAIC_INFO("Testbed initialized.");
    }

    void onUpdate() override
    {
        core::Timer::tick();

        m_renderSystem->render();

        m_inputSystem->poll();

        auto inputContext = m_inputSystem->getContext(m_window.get());

        if (inputContext->isActionTriggered("moveLeft")) MOSAIC_INFO("Moving left.");
        if (inputContext->isActionTriggered("moveRight")) MOSAIC_INFO("Moving right.");
        if (inputContext->isActionTriggered("moveUp")) MOSAIC_INFO("Moving up.");
        if (inputContext->isActionTriggered("moveDown")) MOSAIC_INFO("Moving down.");
        if (inputContext->isActionTriggered("resetCamera")) MOSAIC_INFO("Resetting camera.");

        if (m_window->shouldClose() || inputContext->isActionTriggered("closeApp"))
        {
            return shutdown();
        }
    }

    void onPause() override { MOSAIC_INFO("Testbed paused."); }

    void onResume() override { MOSAIC_INFO("Testbed resumed."); }

    void onShutdown() override
    {
        m_renderSystem->destroyAllContexts();
        m_inputSystem->unregisterAllWindows();

        MOSAIC_INFO("Testbed shutdown.");
    }
};

int main()
{
#ifdef _DEBUG
    mosaic::utils::enableMemoryLeakDetection();
#endif

    TestbedApplication app;

    auto result = app.initialize().andThen(std::mem_fn(&TestbedApplication::run));

    if (result.isErr())
    {
        MOSAIC_ERROR("Failed to initialize the application: {0}", result.error());
        return 1;
    }

    return 0;
}
