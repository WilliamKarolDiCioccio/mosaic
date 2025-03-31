#pragma once

#include <memory>
#include <string>

#include "mosaic/defines.hpp"

namespace mosaic::core
{

struct ApplicationState
{
    bool isInitialized;
    bool isPaused;

    ApplicationState() : isInitialized(false), isPaused(false) {}
};

struct ApplicationData
{
    std::string appName;
    std::string logFilePath;
    std::string traceFilePath;

    ApplicationData(const std::string& _appName, const std::string& _logFilePath, const std::string& _traceFilePath)
        : appName(_appName), logFilePath(_logFilePath), traceFilePath(_traceFilePath) {};
};

class MOSAIC_API Application
{
   public:
    Application(const std::string& _appName, const std::string& _logFilePath = "./logs",
                const std::string& _traceFilePath = "./traces");
    ~Application();

    void initialize();
    void update();
    void pause();
    void resume();
    void shutdown();

    const bool isInitialized() const { return m_state.isInitialized; }

   protected:
    virtual void onInitialize() = 0;
    virtual void onUpdate() = 0;
    virtual void onPause() = 0;
    virtual void onResume() = 0;
    virtual void onShutdown() = 0;

   private:
    ApplicationData m_data;
    ApplicationState m_state;
};

} // namespace mosaic::core
