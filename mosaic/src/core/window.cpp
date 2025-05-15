#include "mosaic/core/window.hpp"

#include <iostream>
#include <stdexcept>

#include <stb_image.h>
#include <stb_image_resize2.h>

#include "mosaic/core/logger.hpp"

namespace mosaic
{
namespace core
{

Window::Window(const std::string& _title, glm::ivec2 _size) : m_window(nullptr)
{
    m_properties.title = _title;
    m_properties.size = _size;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

#ifndef __EMSCRIPTEN__
    glfwWindowHint(GLFW_POSITION_X, m_properties.position.x);
    glfwWindowHint(GLFW_POSITION_Y, m_properties.position.y);
#endif

    m_window = glfwCreateWindow(_size.x, _size.y, _title.c_str(), nullptr, nullptr);

    if (!m_window)
    {
        throw std::runtime_error("Failed to create GLFW window.");
    }

    registerCallbacks();

    MOSAIC_DEBUG("Window created: {0} ({1} x {2})", _title, _size.x, _size.y);
}

Window::~Window()
{
    if (!m_window)
    {
        MOSAIC_ERROR("Window already destroyed.");
        return;
    }

    unregisterCallbacks();

    glfwDestroyWindow(m_window);

    m_window = nullptr;

    MOSAIC_DEBUG("Window destroyed.");
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
        case CursorMode::normal:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case CursorMode::captured:
#ifndef __EMSCRIPTEN__
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
#endif
            break;
        case CursorMode::hidden:
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case CursorMode::disabled:
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
        MOSAIC_ERROR("Failed to load cursor icon: {0}", _path.c_str());
        return;
    }

    if (_width > 0 && _height > 0 && (_width != w || _height != h))
    {
        unsigned char* resized = (unsigned char*)malloc(_width * _height * 4);

        stbir_resize_uint8_linear(data, w, h, 0, resized, _width, _height, 0,
                                  static_cast<stbir_pixel_layout>(4));

        if (!resized)
        {
            MOSAIC_ERROR("Failed to resize cursor icon: {0}", _path.c_str());
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
        MOSAIC_ERROR("Failed to create cursor from image: {0}", _path.c_str());
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
        MOSAIC_ERROR("Failed to load window icon: {0}", _path.c_str());
        return;
    }

    if (_width > 0 && _height > 0 && (_width != w || _height != h))
    {
        unsigned char* resized = (unsigned char*)malloc(_width * _height * 4);

        stbir_resize_uint8_srgb(data, w, h, 0, resized, _width, _height, 0,
                                static_cast<stbir_pixel_layout>(4));

        if (!resized)
        {
            MOSAIC_ERROR("Failed to resize window icon: {0}", _path.c_str());
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

void Window::registerCallbacks()
{
    glfwSetWindowUserPointer(m_window, this);

    glfwSetWindowCloseCallback(m_window,
                               [](GLFWwindow* _window)
                               {
                                   auto win =
                                       static_cast<Window*>(glfwGetWindowUserPointer(_window));

                                   if (!win) return;

                                   for (auto& callback : win->m_windowCloseCallbacks)
                                   {
                                       callback();
                                   }
                               });

    glfwSetWindowFocusCallback(m_window,
                               [](GLFWwindow* _window, int _focused)
                               {
                                   auto win =
                                       static_cast<Window*>(glfwGetWindowUserPointer(_window));

                                   if (!win) return;

                                   for (auto& callback : win->m_windowFocusCallbacks)
                                   {
                                       callback(_focused);
                                   }
                               });

    glfwSetWindowSizeCallback(m_window,
                              [](GLFWwindow* _window, int _width, int _height)
                              {
                                  auto win =
                                      static_cast<Window*>(glfwGetWindowUserPointer(_window));

                                  if (!win) return;

                                  win->m_properties.size = {_width, _height};

                                  for (auto& callback : win->m_windowResizeCallbacks)
                                  {
                                      callback(_width, _height);
                                  }
                              });

    glfwSetWindowRefreshCallback(m_window,
                                 [](GLFWwindow* _window)
                                 {
                                     auto win =
                                         static_cast<Window*>(glfwGetWindowUserPointer(_window));

                                     if (!win) return;

                                     for (auto& callback : win->m_windowRefreshCallbacks)
                                     {
                                         callback(_window);
                                     }
                                 });

    glfwSetWindowIconifyCallback(m_window,
                                 [](GLFWwindow* _window, int _iconified)
                                 {
                                     auto win =
                                         static_cast<Window*>(glfwGetWindowUserPointer(_window));

                                     if (!win) return;

                                     win->m_properties.isMinimized = _iconified;

                                     for (auto& callback : win->m_windowIconifyCallbacks)
                                     {
                                         callback(_iconified);
                                     }
                                 });

    glfwSetWindowMaximizeCallback(m_window,
                                  [](GLFWwindow* _window, int _maximized)
                                  {
                                      auto win =
                                          static_cast<Window*>(glfwGetWindowUserPointer(_window));

                                      if (!win) return;

                                      win->m_properties.isMaximized = _maximized;

                                      for (auto& callback : win->m_windowMaximizeCallbacks)
                                      {
                                          callback(_maximized);
                                      }
                                  });

    glfwSetDropCallback(m_window,
                        [](GLFWwindow* _window, int _pathsCount, const char** _paths)
                        {
                            auto win = static_cast<Window*>(glfwGetWindowUserPointer(_window));

                            if (!win) return;

                            for (auto& callback : win->m_windowDropCallbacks)
                            {
                                callback(_pathsCount, _paths);
                            }
                        });

    glfwSetScrollCallback(m_window,
                          [](GLFWwindow* _window, double _xoffset, double _yoffset)
                          {
                              auto win = static_cast<Window*>(glfwGetWindowUserPointer(_window));

                              if (!win) return;

                              for (auto& callback : win->m_windowScrollCallbacks)
                              {
                                  callback(_xoffset, _yoffset);
                              }
                          });

    glfwSetWindowPosCallback(m_window,
                             [](GLFWwindow* _window, int _xpos, int _ypos)
                             {
                                 auto win = static_cast<Window*>(glfwGetWindowUserPointer(_window));

                                 if (!win) return;

                                 win->m_properties.position = {_xpos, _ypos};

                                 for (auto& callback : win->m_windowPosCallbacks)
                                 {
                                     callback(_xpos, _ypos);
                                 }
                             });

    glfwSetWindowContentScaleCallback(
        m_window,
        [](GLFWwindow* _window, float _xscale, float _yscale)
        {
            auto win = static_cast<Window*>(glfwGetWindowUserPointer(_window));

            if (!win) return;

            for (auto& callback : win->m_windowContentScaleCallbacks)
            {
                callback(_xscale, _yscale);
            }
        });
}

void Window::unregisterCallbacks()
{
    glfwSetWindowUserPointer(m_window, nullptr);
    glfwSetWindowCloseCallback(m_window, nullptr);
    glfwSetWindowFocusCallback(m_window, nullptr);
    glfwSetWindowSizeCallback(m_window, nullptr);
    glfwSetWindowRefreshCallback(m_window, nullptr);
    glfwSetWindowIconifyCallback(m_window, nullptr);
    glfwSetWindowMaximizeCallback(m_window, nullptr);
    glfwSetDropCallback(m_window, nullptr);
    glfwSetScrollCallback(m_window, nullptr);
    glfwSetWindowPosCallback(m_window, nullptr);
    glfwSetWindowContentScaleCallback(m_window, nullptr);

    glfwSetWindowUserPointer(m_window, nullptr);
}

} // namespace core
} // namespace mosaic
