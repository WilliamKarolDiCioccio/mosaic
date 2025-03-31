#pragma once

#define GLFW_EXPOSE_NATIVE_WIN32

#include <webgpu/webgpu.h>
#include "mosaic/graphics/window.hpp"
#include <glfw3webgpu.h>

#ifdef WEBGPU_BACKEND_WGPU
#include <webgpu/wgpu.h>
#endif

#include "mosaic/core/logger.hpp"
