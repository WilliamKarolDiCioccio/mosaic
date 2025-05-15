#include "mosaic/core/application.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

namespace mosaic
{
namespace core
{

Application::Application(const std::string& _appName)
{
    m_properties.appName = _appName;
    m_properties.appVersion = _MOSAIC_VERSION;
    m_properties.logFilePath = "./logs/" + _appName + ".log";
    m_properties.tracesFilePath = "./traces/" + _appName + ".trace";
    m_properties.workingDirectory = "./";

    LoggerManager::initialize(m_properties.appName, m_properties.logFilePath);
    TracerManager::initialize(m_properties.tracesFilePath);
}

Application::~Application()
{
    LoggerManager::shutdown();
    TracerManager::shutdown();
}

pieces::RefResult<Application, std::string> Application::initialize()
{
    assert(!m_properties.isInitialized && "Application is already initialized!");

    MOSAIC_DEBUG("Initializing Mosaic {0} application", _MOSAIC_VERSION);

    initializePlatform();

    m_properties.isInitialized = true;

    return pieces::OkRef<Application, std::string>(*this);
}

pieces::RefResult<Application, std::string> Application::run()
{
    onInitialize();

#ifdef __EMSCRIPTEN__
    auto callback = [](void* arg)
    {
        Application* pApp = reinterpret_cast<Application*>(arg);

        pApp->realRun();
    };

    emscripten_set_main_loop_arg(callback, this, 0, true);
#else
    while (m_properties.isInitialized && !m_properties.isPaused)
    {
        realRun();
    }
#endif

    onShutdown();

    return pieces::OkRef<Application, std::string>(*this);
}

void Application::realRun()
{
    if (m_properties.isPaused) return;

    onUpdate();
}

void Application::pause()
{
    assert(m_properties.isInitialized && "Application is not initialized!");

    MOSAIC_DEBUG("Application paused");

    if (m_properties.isPaused)
    {
        return;
    }

    onPause();

    m_properties.isPaused = true;
}

void Application::resume()
{
    assert(m_properties.isInitialized && "Application is not initialized!");

    MOSAIC_DEBUG("Application resumed");

    if (!m_properties.isPaused)
    {
        return;
    }

    onResume();

    m_properties.isPaused = false;
}

void Application::shutdown()
{
    assert(m_properties.isInitialized && "Application is not initialized!");

    shutdownPlatform();

    MOSAIC_DEBUG("Shutting down application");

    m_properties.isInitialized = false;
}

void Application::initializePlatform()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW.");
    }
}

void Application::shutdownPlatform() { glfwTerminate(); }

} // namespace core
} // namespace mosaic
