#include "mosaic/core/application.hpp"

#include <iostream>

namespace mosaic
{
namespace core
{

bool Application::s_created = false;

Application::Application(const std::string& _appName) : m_platform(platform::Platform::create())
{
    assert(!s_created);

    s_created = true;

    std::string logFilePath = "./logs/" + _appName + ".log";
    std::string tracesFilePath = "./traces/" + _appName + ".trace";

    LoggerManager::initialize(_appName, logFilePath);
    TracerManager::initialize(tracesFilePath);
}

Application::~Application()
{
    LoggerManager::shutdown();
    TracerManager::shutdown();
}

pieces::RefResult<Application, std::string> Application::initialize()
{
    assert(!m_state.isInitialized && "Application is already initialized!");

    MOSAIC_DEBUG("Initializing Mosaic {0} application", _MOSAIC_VERSION);

    m_platform->initialize();

    m_state.isInitialized = true;

    onInitialize();

    return pieces::OkRef<Application, std::string>(*this);
}

pieces::RefResult<Application, std::string> Application::run()
{
    m_state.isRunning = true;

#ifdef __EMSCRIPTEN__
    auto callback = [](void* arg)
    {
        Application* pApp = reinterpret_cast<Application*>(arg);

        pApp->realRun();
    };

    emscripten_set_main_loop_arg(callback, this, 0, true);
#else
    while (m_state.isRunning)
    {
        m_platform->update();

        realRun();
    }
#endif

    shutdown();

    return pieces::OkRef<Application, std::string>(*this);
}

void Application::realRun()
{
    if (m_state.isPaused) return;

    onUpdate();
}

void Application::pause()
{
    assert(m_state.isInitialized && "Application is not initialized!");

    MOSAIC_DEBUG("Application paused");

    if (m_state.isPaused)
    {
        return;
    }

    onPause();

    m_state.isPaused = true;
}

void Application::resume()
{
    assert(m_state.isInitialized && "Application is not initialized!");

    MOSAIC_DEBUG("Application resumed");

    if (!m_state.isPaused)
    {
        return;
    }

    onResume();

    m_state.isPaused = false;
}

void Application::shutdown()
{
    assert(m_state.isInitialized && "Application is not initialized!");

    MOSAIC_DEBUG("Shutting down application");

    onShutdown();

    m_platform->shutdown();

    m_state.isRunning = false;
}

} // namespace core
} // namespace mosaic
