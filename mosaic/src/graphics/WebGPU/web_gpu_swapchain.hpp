#pragma once

#include "shared.hpp"

namespace mosaic::graphics
{

void configureSwapchain(WGPUAdapter _adapter, WGPUDevice _device, WGPUSurface _surface,
                        const Window& _window);

}
