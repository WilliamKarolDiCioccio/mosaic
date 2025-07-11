#include "testbed.hpp"

#include <chrono>

using namespace std::chrono_literals;

namespace testbed
{

std::optional<std::string> TestbedApplication::onInitialize()
{
    auto window = m_windowSystem->getWindow("MainWindow");

    window->setResizeable(true);

    auto inputContext = m_inputSystem->getContext(window);

#ifndef MOSAIC_PLATFORM_ANDROID
    inputContext->updateVirtualKeyboardKeys({
        {"closeApp", input::KeyboardKey::key_escape},
        {"moveLeft", input::KeyboardKey::key_a},
        {"moveRight", input::KeyboardKey::key_d},
        {"moveUp", input::KeyboardKey::key_w},
        {"moveDown", input::KeyboardKey::key_s},
        {"resetCamera", input::KeyboardKey::key_r},
    });

    inputContext->addSource<input::MouseInputSource>();
    inputContext->addSource<input::KeyboardInputSource>();
    inputContext->addSource<input::TextInputSource>();

    inputContext->registerActions({
        input::Action{
            "moveLeft",
            "Move the camera left.",
            [](input::InputContext* ctx)
            {
                auto src = ctx->getSource<input::KeyboardInputSource>();

                const auto srcPollCount = src->getPollCount();
                const auto event = src->getKeyEvent(ctx->translateKey("moveLeft"));

                return event.metadata.pollCount == srcPollCount &&
                       utils::hasFlag(event.state, input::ActionableState::hold);
            },
        },
        input::Action{
            "moveRight",
            "Move the camera right.",
            [](input::InputContext* ctx)
            {
                auto src = ctx->getSource<input::KeyboardInputSource>();

                const auto srcPollCount = src->getPollCount();
                const auto event = src->getKeyEvent(ctx->translateKey("moveRight"));

                return event.metadata.pollCount == srcPollCount &&
                       utils::hasFlag(event.state, input::ActionableState::hold);
            },
        },
        input::Action{
            "moveUp",
            "Move the camera up.",
            [](input::InputContext* ctx)
            {
                auto src = ctx->getSource<input::KeyboardInputSource>();

                const auto srcPollCount = src->getPollCount();
                const auto event = src->getKeyEvent(ctx->translateKey("moveUp"));

                return event.metadata.pollCount == srcPollCount &&
                       utils::hasFlag(event.state, input::ActionableState::hold);
            },
        },
        input::Action{
            "moveDown",
            "Move the camera down.",
            [](input::InputContext* ctx)
            {
                auto src = ctx->getSource<input::KeyboardInputSource>();

                const auto srcPollCount = src->getPollCount();
                const auto event = src->getKeyEvent(ctx->translateKey("moveDown"));

                return event.metadata.pollCount == srcPollCount &&
                       utils::hasFlag(event.state, input::ActionableState::hold);
            },
        },
        input::Action{
            "resetCamera",
            "Reset the camera position.",
            [](input::InputContext* ctx)
            {
                auto src = ctx->getSource<input::KeyboardInputSource>();

                const auto srcPollCount = src->getPollCount();
                const auto event = src->getKeyEvent(ctx->translateKey("resetCamera"));

                return event.metadata.pollCount == srcPollCount &&
                       utils::hasFlag(event.state, input::ActionableState::release) &&
                       event.metadata.duration > 2500ms && event.metadata.duration < 5000ms;
            },
        },
        input::Action{
            "closeApp",
            "Close the application.",
            [](input::InputContext* ctx)
            {
                auto src = ctx->getSource<input::KeyboardInputSource>();

                const auto srcPollCount = src->getPollCount();
                const auto event = src->getKeyEvent(ctx->translateKey("closeApp"));

                return event.metadata.pollCount == srcPollCount &&
                       utils::hasFlag(event.state, input::ActionableState::double_press);
            },
        },
    });
#endif

    MOSAIC_INFO("Testbed initialized.");

    return std::nullopt;
}

std::optional<std::string> TestbedApplication::onUpdate()
{
    auto window = m_windowSystem->getWindow("MainWindow");

    auto inputContext = m_inputSystem->getContext(window);

    if (inputContext->isActionTriggered("moveLeft")) MOSAIC_INFO("Moving left.");
    if (inputContext->isActionTriggered("moveRight")) MOSAIC_INFO("Moving right.");
    if (inputContext->isActionTriggered("moveUp")) MOSAIC_INFO("Moving up.");
    if (inputContext->isActionTriggered("moveDown")) MOSAIC_INFO("Moving down.");
    if (inputContext->isActionTriggered("resetCamera")) MOSAIC_INFO("Resetting camera.");

    if (window->shouldClose() || inputContext->isActionTriggered("closeApp"))
    {
        requestExit();

        return std::nullopt;
    }

#ifndef MOSAIC_PLATFORM_ANDROID
    {
        auto src = inputContext->getSource<input::TextInputSource>();

        const auto srcPollCount = src->getPollCount();
        const auto event = src->getTextInputEvent();

        if (srcPollCount == event.metadata.pollCount) MOSAIC_INFO(event.text);
    }
#endif

    return std::nullopt;
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
