#include "mosaic/input/raw_input_handler.hpp"

#if defined(MOSAIC_PLATFORM_WINDOWS) || defined(MOSAIC_PLATFORM_LINUX) || \
    defined(MOSAIC_PLATFORM_MACOS) || defined(MOSAIC_PLATFORM_EMSCRIPTEN)
#include "platform/GLFW/glfw_raw_input_handler.hpp"
#elif defined(MOSAIC_PLATFORM_ANDROID)
#include "platform/AGDK/agdk_raw_input_handler.hpp"
#endif

namespace mosaic
{
namespace input
{

RawInputHandler::RawInputHandler(core::Window* _window)
    : m_nativeHandle(_window->getNativeHandle()), m_isActive(false) {};

std::unique_ptr<RawInputHandler> RawInputHandler::create(core::Window* _window)
{
#if defined(MOSAIC_PLATFORM_WINDOWS) || defined(MOSAIC_PLATFORM_LINUX) || \
    defined(MOSAIC_PLATFORM_MACOS) || defined(MOSAIC_PLATFORM_EMSCRIPTEN)
    return std::make_unique<platform::glfw::GLFWRawInputHandler>(_window);
#elif defined(MOSAIC_PLATFORM_ANDROID)
    return std::make_unique<platform::agdk::AGDKRawInputHandler>(_window);
#endif
}

} // namespace input
} // namespace mosaic
