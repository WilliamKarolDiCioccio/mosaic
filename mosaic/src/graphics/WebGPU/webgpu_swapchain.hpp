#pragma once

#include "webgpu_common.hpp"

namespace mosaic
{
namespace graphics
{
namespace webgpu
{

void configureSwapchain(WGPUAdapter _adapter, WGPUDevice _device, WGPUSurface _surface,
                        const Window& _window);

} // namespace webgpu
} // namespace graphics
} // namespace mosaic
