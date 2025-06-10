#pragma once

#include "mosaic/core/platform.hpp"

#include <jni.h>
#include <game-activity/GameActivity.h>
#include <game-text-input/gametextinput.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

#include "mosaic/platform/AGDK/agdk_platform_context.hpp"

namespace mosaic
{
namespace platform
{
namespace agdk
{

class AGDKPlatform : public core::Platform
{
   public:
    AGDKPlatform(core::Application* _app) : Platform(_app) {};
    ~AGDKPlatform() override = default;

   public:
    pieces::RefResult<Platform, std::string> initialize() override;
    pieces::RefResult<Platform, std::string> run() override;
    void pause() override;
    void resume() override;
    void shutdown() override;
};

} // namespace agdk
} // namespace platform
} // namespace mosaic
