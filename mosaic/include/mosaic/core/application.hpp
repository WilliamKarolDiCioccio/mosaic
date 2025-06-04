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
#include "platform.hpp"

namespace mosaic
{
namespace core
{

struct ApplicationState
{
    bool isInitialized;
    bool isPaused;
    bool isRunning;

    ApplicationState() : isInitialized(false), isPaused(false), isRunning(false) {};
};

class MOSAIC_API Application
{
   public:
    Application(const std::string& _appName);
    ~Application();

    Application(Application&) = delete;
    Application& operator=(Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

   public:
    pieces::RefResult<Application, std::string> initialize();
    pieces::RefResult<Application, std::string> run();
    void pause();
    void resume();
    void shutdown();

   protected:
    virtual void onInitialize() = 0;
    virtual void onUpdate() = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onShutdown() = 0;

   private:
    void realRun();

    static bool s_created;

    ApplicationState m_state;

    std::unique_ptr<platform::Platform> m_platform;
};

} // namespace core
} // namespace mosaic
