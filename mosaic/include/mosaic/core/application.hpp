#pragma once

#include <memory>
#include <string>
#include <vector>
#include <atomic>

#include <pieces/result.hpp>

#include "mosaic/defines.hpp"
#include "mosaic/version.hpp"

#include "logger.hpp"
#include "tracer.hpp"

namespace mosaic
{
namespace core
{

enum class ApplicationState
{
    resumed,
    paused,
};

class MOSAIC_API Application
{
   private:
    static bool s_created;
    bool m_initialized;
    bool m_exitRequested;

    std::string m_appName;
    ApplicationState m_state;

   public:
    Application(const std::string& _appName);
    virtual ~Application() = default;

    Application(Application&) = delete;
    Application& operator=(Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

   public:
    pieces::RefResult<Application, std::string> initialize();
    pieces::RefResult<Application, std::string> update();
    void pause();
    void resume();
    void shutdown();

    [[nodiscard]] inline bool shouldExit() const { return m_exitRequested; }
    void requestExit() { m_exitRequested = true; }

   protected:
    virtual void onInitialize() = 0;
    virtual void onUpdate() = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onShutdown() = 0;
};

} // namespace core
} // namespace mosaic
