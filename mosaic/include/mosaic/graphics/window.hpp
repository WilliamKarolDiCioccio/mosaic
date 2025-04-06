#pragma once

#include <GLFW/glfw3.h>

#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <string>

#include "mosaic/defines.hpp"

namespace mosaic
{
namespace graphics
{

struct WindowState
{
    bool isFullscreen = false;
    bool isMinimized = false;
    bool isMaximized = false;
    bool isResizeable = false;
};

class MOSAIC_API Window
{
   private:
    GLFWwindow* m_window;

   public:
    Window(const std::string& _title, glm::vec2 _size);
    ~Window();

    void update();

    bool shouldClose() const;

    const std::string getTitle() const;
    const glm::vec2 getSize() const;

    void setTitle(const std::string& title);
    void setSize(glm::vec2 size);

    GLFWwindow* getGLFWHandle() const { return m_window; }

    glm::vec2 getFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return glm::vec2(width, height);
    }

    WindowState getState() const
    {
        WindowState state;
        state.isFullscreen = glfwGetWindowMonitor(m_window) != nullptr;
        state.isMinimized = glfwGetWindowAttrib(m_window, GLFW_ICONIFIED);
        state.isMaximized = glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED);
        state.isResizeable = glfwGetWindowAttrib(m_window, GLFW_RESIZABLE);
        return state;
    }
};

} // namespace graphics
} // namespace mosaic
