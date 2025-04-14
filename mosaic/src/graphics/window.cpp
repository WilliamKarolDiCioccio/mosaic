#include "mosaic/graphics/window.hpp"

#include <iostream>
#include <stdexcept>

#include <stb_image.h>
#include <stb_image_resize2.h>

#include "mosaic/core/logger.hpp"

namespace mosaic
{
namespace graphics
{

Window::Window(const std::string& _title, glm::vec2 _size) : m_window(nullptr)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(static_cast<int>(_size.x), static_cast<int>(_size.y),
                                _title.c_str(), nullptr, nullptr);

    if (!m_window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window.");
    }

    glfwSetWindowUserPointer(m_window, this);

    glfwSetWindowSizeCallback(m_window,
                              [](GLFWwindow* window, int width, int height)
                              {
                                  Window* win =
                                      static_cast<Window*>(glfwGetWindowUserPointer(window));
                                  if (win) win->m_properties.size = {width, height};
                              });

    glfwSetWindowIconifyCallback(m_window,
                                 [](GLFWwindow* window, int iconified)
                                 {
                                     Window* win =
                                         static_cast<Window*>(glfwGetWindowUserPointer(window));
                                     if (win) win->m_properties.isMinimized = iconified;
                                 });

    glfwSetWindowMaximizeCallback(m_window,
                                  [](GLFWwindow* window, int maximized)
                                  {
                                      Window* win =
                                          static_cast<Window*>(glfwGetWindowUserPointer(window));
                                      if (win) win->m_properties.isMaximized = maximized;
                                  });

    MOSAIC_INFO("Window created: {0} ({1} x {2})", _title, _size.x, _size.y);
}

Window::~Window()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
    }

    glfwTerminate();

    m_window = nullptr;

    MOSAIC_INFO("Window destroyed.");
}

bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }

void Window::setTitle(const std::string& title)
{
    glfwSetWindowTitle(m_window, title.c_str());
    m_properties.title = title;
}

void Window::setSize(glm::vec2 size)
{
    glfwSetWindowSize(m_window, static_cast<int>(size.x), static_cast<int>(size.y));
    m_properties.size = size;
}

void Window::setMinimized(bool minimized)
{
    minimized ? glfwIconifyWindow(m_window) : glfwRestoreWindow(m_window);
    m_properties.isMinimized = minimized;
}

void Window::setMaximized(bool maximized)
{
    maximized ? glfwMaximizeWindow(m_window) : glfwRestoreWindow(m_window);
    m_properties.isMaximized = maximized;
}

void Window::setFullscreen(bool fullscreen)
{
    if (fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(m_window, nullptr, 100, 100, 800, 600, 0);
    }

    m_properties.isFullscreen = fullscreen;
}

void Window::setResizeable(bool resizeable)
{
    glfwSetWindowAttrib(m_window, GLFW_RESIZABLE, resizeable ? GLFW_TRUE : GLFW_FALSE);
    m_properties.isResizeable = resizeable;
}

void Window::setVSync(bool enabled)
{
    enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
    m_properties.isVSync = enabled;
}

void Window::setCursorMode(CursorMode _mode)
{
    switch (_mode)
    {
        case mosaic::graphics::CursorMode::normal:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case mosaic::graphics::CursorMode::captured:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
            break;
        case mosaic::graphics::CursorMode::hidden:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case mosaic::graphics::CursorMode::disabled:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
    }
}

void Window::setCursorType(CursorType _type)
{
    if (m_properties.cursorProperties.currentType == _type) return;

    m_properties.cursorProperties.currentType = _type;
    setCursorIcon(m_properties.cursorProperties.srcPaths[static_cast<int>(_type)]);
}

void Window::setCursorTypeIcon(CursorType _type, const std::string& _path, int _width, int _height)
{
    if (m_properties.cursorProperties.srcPaths[static_cast<int>(_type)] == _path) return;

    m_properties.cursorProperties.srcPaths[static_cast<int>(_type)] = _path;
    setCursorIcon(_path, _width, _height);
}

void Window::setCursorIcon(const std::string& _path, int _width, int _height)
{
    int w, h, channels;
    unsigned char* data = stbi_load(_path.c_str(), &w, &h, &channels, 4);

    if (!data)
    {
        MOSAIC_ERROR("Failed to load cursor icon: {0}", _path);
        return;
    }

    if (_width > 0 && _height > 0 && (_width != w || _height != h))
    {
        unsigned char* resized = (unsigned char*)malloc(_width * _height * 4);

        stbir_resize_uint8_linear(data, w, h, 0, resized, _width, _height, 0,
                                  static_cast<stbir_pixel_layout>(4));

        if (!resized)
        {
            MOSAIC_ERROR("Failed to resize cursor icon: {0}", _path);
            stbi_image_free(data);
            return;
        }

        stbi_image_free(data);

        data = resized;
        w = _width;
        h = _height;
    }

    GLFWimage image;
    image.width = w;
    image.height = h;
    image.pixels = data;

    GLFWcursor* cursor = glfwCreateCursor(&image, w / 2, h / 2);

    if (!cursor)
    {
        MOSAIC_ERROR("Failed to create cursor from image: {0}", _path);
        stbi_image_free(data);
        return;
    }

    glfwSetCursor(m_window, cursor);
    stbi_image_free(data);
}

void Window::resetCursorIcon() { glfwSetCursor(m_window, nullptr); }

void Window::setWindowIcon(const std::string& _path, int _width, int _height)
{
    int w, h, channels;
    unsigned char* data = stbi_load(_path.c_str(), &w, &h, &channels, 4);

    if (!data)
    {
        MOSAIC_ERROR("Failed to load window icon: {0}", _path);
        return;
    }

    if (_width > 0 && _height > 0 && (_width != w || _height != h))
    {
        unsigned char* resized = (unsigned char*)malloc(_width * _height * 4);

        stbir_resize_uint8_srgb(data, w, h, 0, resized, _width, _height, 0,
                                static_cast<stbir_pixel_layout>(4));

        if (!resized)
        {
            MOSAIC_ERROR("Failed to resize window icon: {0}", _path);
            stbi_image_free(data);
            return;
        }

        stbi_image_free(data);

        data = resized;
        w = _width;
        h = _height;
    }

    GLFWimage image;
    image.width = w;
    image.height = h;
    image.pixels = data;

    glfwSetWindowIcon(m_window, 1, &image);
    stbi_image_free(data);
}

void Window::resetWindowIcon() { glfwSetWindowIcon(m_window, 0, nullptr); }

} // namespace graphics
} // namespace mosaic
