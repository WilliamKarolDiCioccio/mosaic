#include "testbed.hpp"

namespace testbed
{

void TestbedApplication::onInitialize()
{
    m_window = mosaic::core::Window::create("Testbed", glm::vec2(1280, 720));
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

void TestbedApplication::onUpdate()
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

void TestbedApplication::onPause() { MOSAIC_INFO("Testbed paused."); }

void TestbedApplication::onResume() { MOSAIC_INFO("Testbed resumed."); }

void TestbedApplication::onShutdown()
{
    m_renderSystem->destroyAllContexts();
    m_inputSystem->unregisterAllWindows();

    MOSAIC_INFO("Testbed shutdown.");
}

} // namespace testbed