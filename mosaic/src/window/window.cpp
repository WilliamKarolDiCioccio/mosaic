#include "mosaic/window/window.hpp"

#if defined(MOSAIC_PLATFORM_DESKTOP) || defined(MOSAIC_PLATFORM_WEB)
#include "platform/GLFW/glfw_window.hpp"
#elif defined(MOSAIC_PLATFORM_ANDROID)
#include "platform/AGDK/agdk_window.hpp"
#endif

namespace mosaic
{
namespace window
{

std::unique_ptr<Window> Window::create()
{
#if defined(MOSAIC_PLATFORM_DESKTOP) || defined(MOSAIC_PLATFORM_WEB)
    return std::make_unique<platform::glfw::GLFWWindow>();
#elif defined(MOSAIC_PLATFORM_ANDROID)
    return std::make_unique<platform::agdk::AGDKWindow>();
#endif
}

void Window::invokeCloseCallbacks()
{
    for (auto& callback : m_windowCloseCallbacks) callback.callback();
}

void Window::invokeFocusCallbacks(int _focused)
{
    for (auto& callback : m_windowFocusCallbacks) callback.callback(_focused);
}

void Window::invokeResizeCallbacks(int _width, int _height)
{
    for (auto& callback : m_windowResizeCallbacks) callback.callback(_width, _height);
}

void Window::invokeRefreshCallbacks()
{
    for (auto& callback : m_windowRefreshCallbacks) callback.callback();
}

void Window::invokeIconifyCallbacks(int _iconified)
{
    for (auto& callback : m_windowIconifyCallbacks) callback.callback(_iconified);
}

void Window::invokeMaximizeCallbacks(int _maximized)
{
    for (auto& callback : m_windowMaximizeCallbacks) callback.callback(_maximized);
}

void Window::invokeDropCallbacks(int _count, const char** _paths)
{
    for (auto& callback : m_windowDropCallbacks) callback.callback(_count, _paths);
}

void Window::invokeScrollCallbacks(double _xoffset, double _yoffset)
{
    for (auto& callback : m_windowScrollCallbacks) callback.callback(_xoffset, _yoffset);
}

void Window::invokeCursorEnterCallbacks(int _entered)
{
    for (auto& callback : m_windowCursorEnterCallbacks) callback.callback(_entered);
}

void Window::invokePosCallbacks(int _x, int _y)
{
    for (auto& callback : m_windowPosCallbacks) callback.callback(_x, _y);
}

void Window::invokeContentScaleCallbacks(float _xscale, float _yscale)
{
    for (auto& callback : m_windowContentScaleCallbacks) callback.callback(_xscale, _yscale);
}

void Window::invokeCharCallbacks(unsigned int _codepoint)
{
    for (auto& callback : m_windowCharCallbacks) callback.callback(_codepoint);
}

} // namespace window
} // namespace mosaic
