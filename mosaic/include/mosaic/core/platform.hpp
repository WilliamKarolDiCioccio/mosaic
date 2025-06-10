#pragma once

#include <string>
#include <memory>

#include <pieces/result.hpp>

#include "mosaic/defines.hpp"

#include "application.hpp"

namespace mosaic
{
namespace core
{

/**
 * @brief Abstract base class representing the platform layer of the application.
 *
 * This class provides a generic interface for managing the interaction between the platform and the
 * application. Specifically, it handles the lifecycle and resources and enforces platform-specific
 * behaviors.
 *
 * Web and mobile runtimes are the primary reasons for this class, as they require specific
 * initialization, running, pausing, resuming, and shutdown behaviors that differ from traditional
 * desktop applications.
 */
class MOSAIC_API Platform
{
   private:
    static Platform* s_instance;

   protected:
    Application* m_app;
    void* m_platformContext;

   public:
    Platform(Application* _app);
    virtual ~Platform() = default;

    static std::unique_ptr<Platform> create(Application* _app);

    [[nodiscard]] static Platform* getInstance() { return s_instance; }

    [[nodiscard]] void* getPlatformContext() { return m_platformContext; }

    void setPlatformContext(void* _context) { m_platformContext = _context; }

   public:
    virtual pieces::RefResult<Platform, std::string> initialize() = 0;
    virtual pieces::RefResult<Platform, std::string> run() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void shutdown() = 0;
};

} // namespace core
} // namespace mosaic
