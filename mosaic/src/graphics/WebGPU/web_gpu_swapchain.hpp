#pragma once

#include "shared.hpp"

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
