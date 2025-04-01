#pragma once

#include "shared.hpp"

namespace mosaic
{
namespace graphics
{
namespace webgpu
{

WGPUAdapter requestAdapter(WGPUInstance _instance, WGPUSurface _surface);

bool isAdapterSuitable(WGPUAdapter _adapter);

WGPUDevice createDevice(WGPUAdapter adapter);

} // namespace webgpu
} // namespace graphics
} // namespace mosaic
