#include "web_gpu_instance.hpp"

namespace mosaic::graphics
{

WGPUInstance createInstance()
{
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_DAWN
    WGPUDawnTogglesDescriptor toggles;
    toggles.chain.next = nullptr;
    toggles.chain.sType = WGPUSType_DawnTogglesDescriptor;
    toggles.disabledToggleCount = 0;
    toggles.enabledToggleCount = 1;
    const char* toggleName = "enable_immediate_error_handling";
    toggles.enabledToggles = &toggleName;

    desc.nextInChain = &toggles.chain;
#endif

    WGPUInstance instance = wgpuCreateInstance(&desc);

    if (!instance)
    {
        MOSAIC_ERROR("Could not create WebGPU instance!");
    }

    return instance;
}

} // namespace mosaic::graphics
