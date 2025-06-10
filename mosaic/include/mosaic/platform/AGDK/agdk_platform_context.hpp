#pragma once

#include <jni.h>
#include <game-activity/GameActivity.h>
#include <game-text-input/gametextinput.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace mosaic
{
namespace platform
{
namespace agdk
{

struct AGDKPlatformContext
{
    android_app* app;
    ANativeWindow* window;
    AAssetManager* assetManager;
    JNIEnv* env;
};

} // namespace agdk
} // namespace platform
} // namespace mosaic