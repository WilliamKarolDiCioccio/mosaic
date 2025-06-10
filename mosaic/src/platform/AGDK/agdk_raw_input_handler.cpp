#include "agdk_raw_input_handler.hpp"

namespace mosaic
{
namespace platform
{
namespace agdk
{

AGDKRawInputHandler::AGDKRawInputHandler(core::Window* _window)
    : m_window(_window), input::RawInputHandler(_window)
{
    registerCallbacks();
}

input::RawInputHandler::KeyboardKeyInputData AGDKRawInputHandler::getKeyboardKeyInput(
    input::KeyboardKey _key) const
{
    return static_cast<input::RawInputHandler::KeyboardKeyInputData>(0);
}

input::RawInputHandler::MouseButtonInputData AGDKRawInputHandler::getMouseButtonInput(
    input::MouseButton _button) const
{
    return static_cast<input::RawInputHandler::MouseButtonInputData>(0);
}

input::RawInputHandler::CursorPosInputData AGDKRawInputHandler::getCursorPosInput()
{
    return glm::vec2(0, 0);
}

void AGDKRawInputHandler::registerCallbacks() {}

} // namespace agdk
} // namespace platform
} // namespace mosaic
