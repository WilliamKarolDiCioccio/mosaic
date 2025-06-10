#include "agdk_window.hpp"

namespace mosaic
{
namespace platform
{
namespace agdk
{

AGDKWindow::AGDKWindow(const std::string& _title, glm::ivec2 _size) : core::Window(_title, _size)
{
    static int windowCount = 0;

    if (windowCount > 0)
    {
        throw std::runtime_error("Only one GLFW window is allowed in Emscripten builds.");
    }

    windowCount++;

    auto context =
        static_cast<AGDKPlatformContext*>(AGDKPlatform::getInstance()->getPlatformContext());

    m_window = context->window;

    if (!m_window)
    {
        MOSAIC_ERROR("Failed to create AGDK window.");
        return;
    }

    ANativeWindow_acquire(m_window);

    registerCallbacks();
}

AGDKWindow::~AGDKWindow()
{
    unregisterCallbacks();

    if (m_window)
    {
        ANativeWindow_release(m_window);
        m_window = nullptr;
    }
}

void* AGDKWindow::getNativeHandle() const { return static_cast<void*>(m_window); }

bool AGDKWindow::shouldClose() const { return false; }

glm::ivec2 AGDKWindow::getFramebufferSize() const { return glm::ivec2(); }

void AGDKWindow::setTitle(const std::string& _title) {}

void AGDKWindow::setMinimized(bool _minimized)
{
    MOSAIC_WARN("AGDKWindow::setMinimized: Not implemented for AGDK platform.");
}

void AGDKWindow::setMaximized(bool _maximized)
{
    MOSAIC_WARN("AGDKWindow::setMaximized: Not implemented for AGDK platform.");
}

void AGDKWindow::setFullscreen(bool _fullscreen)
{
    MOSAIC_WARN("AGDKWindow::setFullscreen: Not implemented for AGDK platform.");
}

void AGDKWindow::setSize(glm::vec2 _size)
{
    MOSAIC_WARN("AGDKWindow::setSize: Not implemented for AGDK platform.");
}

void AGDKWindow::setResizeable(bool _resizeable)
{
    MOSAIC_WARN("AGDKWindow::setResizeable: Not implemented for AGDK platform.");
}

void AGDKWindow::setVSync(bool _enabled) {}

void AGDKWindow::setCursorMode(core::CursorMode _mode)
{
    MOSAIC_WARN("AGDKWindow::setCursorMode: Not implemented for AGDK platform.");
}

void AGDKWindow::setCursorType(core::CursorType _type)
{
    MOSAIC_WARN("AGDKWindow::setCursorType: Not implemented for AGDK platform.");
}

void AGDKWindow::setCursorTypeIcon(core::CursorType _type, const std::string& _path, int _width,
                                   int _height)
{
    MOSAIC_WARN("AGDKWindow::setCursorTypeIcon: Not implemented for AGDK platform.");
}

void AGDKWindow::setCursorIcon(const std::string& _path, int _width, int _height)
{
    MOSAIC_WARN("AGDKWindow::setCursorIcon: Not implemented for AGDK platform.");
}

void AGDKWindow::resetCursorIcon()
{
    MOSAIC_WARN("AGDKWindow::resetCursorIcon: Not implemented for AGDK platform.");
}

void AGDKWindow::setWindowIcon(const std::string& _path, int _width, int _height)
{
    MOSAIC_WARN("AGDKWindow::setWindowIcon: Not implemented for AGDK platform.");
}

void AGDKWindow::resetWindowIcon()
{
    MOSAIC_WARN("AGDKWindow::resetWindowIcon: Not implemented for AGDK platform.");
}

void AGDKWindow::setClipboardString(const std::string& _string)
{
    MOSAIC_WARN("AGDKWindow::setClipboardString: Not implemented for AGDK platform.");
}

std::string AGDKWindow::getClipboardString() const
{
    MOSAIC_WARN("AGDKWindow::getClipboardString: Not implemented for AGDK platform.");
    return "";
}

void AGDKWindow::registerCallbacks() {}

void AGDKWindow::unregisterCallbacks() {}

} // namespace agdk
} // namespace platform
} // namespace mosaic
