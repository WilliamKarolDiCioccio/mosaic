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
namespace core
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
        : currentType(CursorType::arrow),
          currentMode(CursorMode::normal),
          srcPaths(),
          isVisible(true),
          isClipped(false) {};
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
    glm::ivec2 position;
    bool isFullscreen;
    bool isMinimized;
    bool isMaximized;
    bool isResizeable;
    bool isVSync;
    CursorProperties cursorProperties;

    WindowProperties()
        : title("Game Window"),
          size({1280, 720}),
          position({100, 100}),
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
    using WindowCloseCallback = std::function<void()>;
    using WindowFocusCallback = std::function<void(int)>;
    using WindowResizeCallback = std::function<void(int, int)>;
    using WindowRefreshCallback = std::function<void(GLFWwindow*)>;
    using WindowIconifyCallback = std::function<void(int)>;
    using WindowMaximizeCallback = std::function<void(int)>;
    using WindowDropCallback = std::function<void(int, const char**)>;
    using WindowScrollCallback = std::function<void(double, double)>;
    using WindowCursorEnterCallback = std::function<void(int)>;
    using WindowPosCallback = std::function<void(int, int)>;
    using WindowContentScaleCallback = std::function<void(float, float)>;

   private:
    GLFWwindow* m_window;
    WindowProperties m_properties;

   public:
    Window(const std::string& _title, glm::ivec2 _size);
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

    // Clipboard
    inline void setClipboardString(const std::string& _string)
    {
        glfwSetClipboardString(nullptr, _string.c_str());
    }

    inline std::string getClipboardString() const { return glfwGetClipboardString(nullptr); }

    inline GLFWwindow* getGLFWHandle() const { return m_window; }

    inline glm::ivec2 getFramebufferSize() const
    {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return glm::ivec2(width, height);
    }

    inline const WindowProperties& getWindowProperties() const { return m_properties; }

    inline const CursorProperties& getCursorProperties() const
    {
        return m_properties.cursorProperties;
    }

    // Public callback registration

    inline void registerWindowCloseCallback(WindowCloseCallback _callback)
    {
        m_windowCloseCallbacks.push_back(_callback);
    }

    inline void registerWindowFocusCallback(WindowFocusCallback _callback)
    {
        m_windowFocusCallbacks.push_back(_callback);
    }

    inline void registerWindowResizeCallback(WindowResizeCallback _callback)
    {
        m_windowResizeCallbacks.push_back(_callback);
    }

    inline void registerWindowRefreshCallback(WindowRefreshCallback _callback)
    {
        m_windowRefreshCallbacks.push_back(_callback);
    }

    inline void registerWindowIconifyCallback(WindowIconifyCallback _callback)
    {
        m_windowIconifyCallbacks.push_back(_callback);
    }

    inline void registerWindowMaximizeCallback(WindowMaximizeCallback _callback)
    {
        m_windowMaximizeCallbacks.push_back(_callback);
    }

    inline void registerWindowDropCallback(WindowDropCallback _callback)
    {
        m_windowDropCallbacks.push_back(_callback);
    }

    inline void registerWindowScrollCallback(WindowScrollCallback _callback)
    {
        m_windowScrollCallbacks.push_back(_callback);
    }

    inline void registerWindowCursorEnterCallback(WindowCursorEnterCallback _callback)
    {
        m_windowCursorEnterCallbacks.push_back(_callback);
    }

    inline void registerWindowPosCallback(WindowPosCallback _callback)
    {
        m_windowPosCallbacks.push_back(_callback);
    }

    inline void registerWindowContentScaleCallback(WindowContentScaleCallback _callback)
    {
        m_windowContentScaleCallbacks.push_back(_callback);
    }

   private:
    std::vector<WindowCloseCallback> m_windowCloseCallbacks;
    std::vector<WindowFocusCallback> m_windowFocusCallbacks;
    std::vector<WindowResizeCallback> m_windowResizeCallbacks;
    std::vector<WindowRefreshCallback> m_windowRefreshCallbacks;
    std::vector<WindowIconifyCallback> m_windowIconifyCallbacks;
    std::vector<WindowMaximizeCallback> m_windowMaximizeCallbacks;
    std::vector<WindowDropCallback> m_windowDropCallbacks;
    std::vector<WindowScrollCallback> m_windowScrollCallbacks;
    std::vector<WindowCursorEnterCallback> m_windowCursorEnterCallbacks;
    std::vector<WindowPosCallback> m_windowPosCallbacks;
    std::vector<WindowContentScaleCallback> m_windowContentScaleCallbacks;

    void registerCallbacks();
    void unregisterCallbacks();
};

} // namespace core
} // namespace mosaic
