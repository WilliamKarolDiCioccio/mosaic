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

class MOSAIC_API Platform
{
   private:
    // Unlike other singletons here we choose to store an instance pointer, due to
    // some platforms requiring a static instance for their main loop.
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
