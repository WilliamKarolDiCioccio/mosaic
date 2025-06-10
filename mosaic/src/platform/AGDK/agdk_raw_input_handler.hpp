#pragma once

#include "mosaic/input/raw_input_handler.hpp"

namespace mosaic
{
namespace platform
{
namespace agdk
{

class AGDKRawInputHandler : public input::RawInputHandler
{
   private:
    core::Window* m_window;

   public:
    AGDKRawInputHandler(core::Window* window);
    ~AGDKRawInputHandler() override = default;

    input::RawInputHandler::KeyboardKeyInputData getKeyboardKeyInput(
        input::KeyboardKey key) const override;
    input::RawInputHandler::MouseButtonInputData getMouseButtonInput(
        input::MouseButton button) const override;
    input::RawInputHandler::CursorPosInputData getCursorPosInput() override;

   private:
    void registerCallbacks();
};

} // namespace agdk
} // namespace platform
} // namespace mosaic
