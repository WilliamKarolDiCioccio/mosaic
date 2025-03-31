#include "web_gpu_device.hpp"

namespace mosaic::graphics
{

WGPUAdapter requestAdapterSync(WGPUInstance _instance, WGPURequestAdapterOptions& _options)
{
    struct UserData
    {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };

    UserData userData;

    WGPURequestAdapterCallback onAdapterRequestEnded =
        [](WGPURequestAdapterStatus _status, WGPUAdapter _adapter, WGPUStringView _message,
           void* _userData1, void* _userData2)
    {
        UserData& userData = *reinterpret_cast<UserData*>(_userData1);

        if (_status == WGPURequestAdapterStatus_Success)
        {
            userData.adapter = _adapter;
        }
        else
        {
            MOSAIC_ERROR("Could not request WebGPU adapter: {}", _message.data);
        }

        userData.requestEnded = true;
    };

    WGPURequestAdapterCallbackInfo callbackInfo = {
        .callback = onAdapterRequestEnded,
        .userdata1 = (void*)&userData,
        .userdata2 = nullptr,
    };

    wgpuInstanceRequestAdapter(_instance, &_options, callbackInfo);

    assert(userData.requestEnded);

    return userData.adapter;
}

bool isAdapterSuitable(WGPUAdapter _adapter, WGPUAdapterInfo& _infos)
{
    WGPULimits supportedLimits;
    supportedLimits.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_DAWN
    if (!wgpuAdapterGetLimits(_adapter, &supportedLimits) == WGPUStatus_Success)
#elif defined WEBGPU_BACKEND_WGPU
    if (wgpuAdapterGetLimits(_adapter, &supportedLimits) != WGPUStatus_Success)
#else
#error "Unknown WebGPU backend!"
#endif
    {
        MOSAIC_ERROR("Could not get WebGPU adapter limits!");
        return false;
    }

    WGPUSupportedFeatures supportedFeatures;

    wgpuAdapterGetFeatures(_adapter, &supportedFeatures);

    if (supportedFeatures.featureCount == 0)
    {
        MOSAIC_ERROR("Could not get WebGPU adapter features!");
        return false;
    }

    wgpuAdapterGetInfo(_adapter, &_infos);

    MOSAIC_INFO("WebGPU adapter device: {}", _infos.device.data);
    MOSAIC_INFO("WebGPU adapter vendor: {}", _infos.vendor.data);
    MOSAIC_INFO("WebGPU adapter backendType: {}", static_cast<int>(_infos.backendType));

    return true;
}

WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor& descriptor)
{
    struct UserData
    {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };

    UserData userData;

    WGPURequestDeviceCallback onDeviceRequestEnded = [](WGPURequestDeviceStatus _status,
                                                        WGPUDevice _device, WGPUStringView _message,
                                                        void* _userData1, void* _userData2)
    {
        UserData& userData1 = *reinterpret_cast<UserData*>(_userData1);

        if (_status == WGPURequestDeviceStatus_Success)
        {
            userData1.device = _device;
        }
        else
        {
            MOSAIC_ERROR("Could not request WebGPU device: {}", _message.data);
        }

        userData1.requestEnded = true;
    };

    WGPURequestDeviceCallbackInfo callbackInfo = {
        .callback = onDeviceRequestEnded,
        .userdata1 = (void*)&userData,
        .userdata2 = nullptr,
    };

    wgpuAdapterRequestDevice(adapter, &descriptor, callbackInfo);

    assert(userData.requestEnded);

    return userData.device;
}

} // namespace mosaic::graphics
