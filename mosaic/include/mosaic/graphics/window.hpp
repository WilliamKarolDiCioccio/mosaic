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

    GLFWwindow* getNativeWindow() const { return m_window; }
};

} // namespace graphics
} // namespace mosaic
