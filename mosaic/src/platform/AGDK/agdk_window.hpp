#pragma once

#include "mosaic/core/window.hpp"

#include "agdk_platform.hpp"

namespace mosaic
{
namespace platform
{
namespace agdk
{

class AGDKWindow : public core::Window
{
   private:
    ANativeWindow* m_window;

   public:
    AGDKWindow(const std::string& _title, glm::ivec2 _size);
    ~AGDKWindow() override;

    void* getNativeHandle() const override;
    bool shouldClose() const override;
    glm::ivec2 getFramebufferSize() const override;

    void setTitle(const std::string& _title) override;
    void setMinimized(bool _minimized) override;
    void setMaximized(bool _maximized) override;
    void setFullscreen(bool _fullscreen) override;
    void setSize(glm::vec2 _size) override;
    void setResizeable(bool _resizeable) override;
    void setVSync(bool _enabled) override;

    void setCursorMode(core::CursorMode _mode) override;
    void setCursorType(core::CursorType _type) override;
    void setCursorTypeIcon(core::CursorType _type, const std::string& _path, int _width = 0,
                           int _height = 0) override;

    void setCursorIcon(const std::string& _path, int _width = 0, int _height = 0) override;
    void resetCursorIcon() override;
    void setWindowIcon(const std::string& _path, int _width = 0, int _height = 0) override;
    void resetWindowIcon() override;

    void setClipboardString(const std::string& _string) override;
    std::string getClipboardString() const override;

   private:
    void registerCallbacks();
    void unregisterCallbacks();

    static void windowCloseCallback(ANativeWindow* _window);
    static void windowFocusCallback(ANativeWindow* _window, int _focused);
    static void windowSizeCallback(ANativeWindow* _window, int _width, int _height);
    static void windowRefreshCallback(ANativeWindow* _window);
    static void windowIconifyCallback(ANativeWindow* _window, int _iconified);
    static void windowMaximizeCallback(ANativeWindow* _window, int _maximized);
    static void windowDropCallback(ANativeWindow* _window, int _count, const char** _paths);
    static void windowScrollCallback(ANativeWindow* _window, double _xoffset, double _yoffset);
    static void windowCursorEnterCallback(ANativeWindow* _window, int _entered);
    static void windowPosCallback(ANativeWindow* _window, int _x, int _y);
    static void windowContentScaleCallback(ANativeWindow* _window, float _xscale, float _yscale);
};

} // namespace agdk
} // namespace platform
} // namespace mosaic
