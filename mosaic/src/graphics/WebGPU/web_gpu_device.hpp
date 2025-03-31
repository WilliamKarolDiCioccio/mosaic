#pragma once

#include "shared.hpp"

namespace mosaic::graphics
{

WGPUAdapter requestAdapter(WGPUInstance _instance, WGPUSurface _surface);

bool isAdapterSuitable(WGPUAdapter _adapter);

WGPUDevice createDevice(WGPUAdapter adapter);

} // namespace mosaic::graphics
