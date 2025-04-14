#pragma once

#include <string>
#include <array>

#include <GLFW/glfw3.h>
#ifdef __EMSCRIPTEN__
#define GLM_FORCE_PURE
#endif
#include <glm/glm.hpp>

#include "mosaic/defines.hpp"

namespace mosaic
{
namespace graphics
{

/**
 * @brief Enumeration for different cursor types.
 *
 * It is used to provide mapping between the cursor type and the corresponding icon.
 * This allows for easy customization of the cursor appearance based on the context.
 */
enum class CursorType
{
    arrow,
    hand,
    text,
    crosshair,
    resize_ns,
    resize_we,
    resize_nwse,
    resize_nesw,
    i_beam,
    custom
};

/**
 * @brief Enumeration for different cursor modes.
 *
 * It is used to define the visibility and interaction mode of the cursor.
 * This allows for better control over the cursor behavior in different contexts.
 */
enum class CursorMode
{
    normal,
    captured,
    hidden,
    disabled
};

/**
 * @brief Structure to hold properties related to the cursor.
 *
 * This includes the current cursor type, paths to cursor images for each cursor type, and
 * visibility status in the window.
 */
struct CursorProperties
{
    CursorType currentType;
    CursorMode currentMode;
    std::array<std::string, 10> srcPaths;
    bool isVisible;
    bool isClipped;

    CursorProperties()
        : currentType(CursorType::arrow), srcPaths(), isVisible(true), isClipped(false) {};
};

/**
 * @brief Structure to hold properties related to the window.
 *
 * This includes the title, size, fullscreen status, minimized/maximized status, resizeable status,
 * VSync status, and cursor properties.
 */
struct WindowProperties
{
    std::string title;
    glm::ivec2 size;
    bool isFullscreen;
    bool isMinimized;
    bool isMaximized;
    bool isResizeable;
    bool isVSync;
    CursorProperties cursorProperties;

    WindowProperties()
        : title("Game Window"),
          size({1280, 720}),
          isFullscreen(false),
          isMinimized(false),
          isMaximized(false),
          isResizeable(true),
          isVSync(false),
          cursorProperties() {};
};

/**
 * @brief Class representing a window in the application.
 *
 * This class provides an interface to facilitate GLFW window management. It keeps track of the
 * window properties and cursor properties and allows to manipulate them easily.
 *
 * @see WindowProperties
 * @see CursorProperties
 */
class MOSAIC_API Window
{
   private:
    GLFWwindow* m_window;
    WindowProperties m_properties;

   public:
    Window(const std::string& _title, glm::vec2 _size);
    ~Window();

    /**
     * @brief Check if the window should close, based on user input or system events.
     *
     * @return true if the window should close, false otherwise.
     * @return false if the window should remain open.
     */
    bool shouldClose() const;

    // Window properties
    void setTitle(const std::string& _title);
    void setMinimized(bool _minimized);
    void setMaximized(bool _maximized);
    void setFullscreen(bool _fullscreen);
    void setSize(glm::vec2 _size);
    void setResizeable(bool _resizeable);
    void setVSync(bool _enabled);

    // Cursor properties
    void setCursorMode(CursorMode _mode);
    void setCursorType(CursorType _type);
    void setCursorTypeIcon(CursorType _type, const std::string& _path, int _width = 0,
                           int _height = 0);

    // Direct manipulation of cursor properties
    void setCursorIcon(const std::string& _path, int _width = 0, int _height = 0);
    void resetCursorIcon();
    void setWindowIcon(const std::string& _path, int _width = 0, int _height = 0);
    void resetWindowIcon();

    inline GLFWwindow* getGLFWHandle() const { return m_window; }

    inline glm::vec2 getFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return glm::vec2(width, height);
    }

    inline const WindowProperties& getWindowProperties() const { return m_properties; }

    inline const CursorProperties& getCursorProperties() const
    {
        return m_properties.cursorProperties;
    }
};

} // namespace graphics
} // namespace mosaic
