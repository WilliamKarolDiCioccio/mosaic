#pragma once

#include "shared.hpp"

namespace mosaic::graphics
{

WGPUAdapter requestAdapterSync(WGPUInstance _instance, WGPURequestAdapterOptions& _options);

bool isAdapterSuitable(WGPUAdapter _adapter, WGPUAdapterInfo& _infos);

WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor& descriptor);

} // namespace mosaic::graphics
