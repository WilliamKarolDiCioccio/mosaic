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
#include "platform/platform.hpp"

namespace mosaic
{
namespace core
{

struct ApplicationProperties
{
    std::string appName;
    std::string appVersion;
    std::string engineVersion;
    std::string logFilePath;
    std::string tracesFilePath;
    std::string workingDirectory;
    std::atomic<bool> isInitialized;
    std::atomic<bool> isPaused;

    ApplicationProperties()
        : appName(""),
          appVersion(""),
          engineVersion(_MOSAIC_VERSION),
          logFilePath("./logs"),
          tracesFilePath("./traces"),
          workingDirectory("./"),
          isInitialized(false),
          isPaused(false) {};
};

class MOSAIC_API Application
{
   public:
    Application(const std::string& _appName);
    ~Application();

    pieces::RefResult<Application, std::string> initialize();
    pieces::RefResult<Application, std::string> run();
    void pause();
    void resume();
    void shutdown();

    inline const ApplicationProperties& getProperties() const { return m_properties; }

   protected:
    virtual void onInitialize() = 0;
    virtual void onUpdate() = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onShutdown() = 0;

   private:
    void realRun();

    ApplicationProperties m_properties;

    std::unique_ptr<platform::Platform> m_platform;
};

} // namespace core
} // namespace mosaic
