#include "mosaic/graphics/window.hpp"

#include <iostream>
#include <stdexcept>

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

    MOSAIC_INFO("Window created: {0} [{1} x {2}]", _title, _size.x, _size.y);
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

void Window::update()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

const std::string Window::getTitle() const { return "[Title Management Placeholder]"; }

const glm::vec2 Window::getSize() const
{
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    return glm::vec2(width, height);
}

void Window::setTitle(const std::string& title) { glfwSetWindowTitle(m_window, title.c_str()); }

void Window::setSize(glm::vec2 size)
{
    glfwSetWindowSize(m_window, static_cast<int>(size.x), static_cast<int>(size.y));
}

} // namespace graphics
} // namespace mosaic
