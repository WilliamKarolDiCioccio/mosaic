#include "web_gpu_renderer_api.hpp"

#include <vector>

#include "web_gpu_device.hpp"
#include "web_gpu_instance.hpp"

namespace mosaic::graphics
{

template <typename T>
bool contains(const T* vec, size_t size, T target)
{
    return std::find(vec, vec + size, target) != (vec + size);
}

void WebGPURendererAPI::initialize(const Window& _window)
{
    WGPUInstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    m_instance = createInstance();

    m_surface = glfwCreateWindowWGPUSurface(m_instance, _window.getNativeWindow());

    WGPURequestAdapterOptions adapterOpts = {};
    adapterOpts.nextInChain = nullptr;
    adapterOpts.compatibleSurface = m_surface;

    m_adapter = requestAdapterSync(m_instance, adapterOpts);

    if (!m_adapter)
    {
        MOSAIC_ERROR("Could not request WebGPU adapter!");
        return;
    }

    WGPUAdapterInfo infos = {};
    infos.nextInChain = nullptr;

    if (!isAdapterSuitable(m_adapter, infos))
    {
        MOSAIC_ERROR("WebGPU adapter is not suitable!");
        return;
    }

    WGPUDeviceLostCallback onDeviceLost = [](const WGPUDevice* _device,
                                             WGPUDeviceLostReason _reason, WGPUStringView _message,
                                             void* _userData1, void* _userData2)
    { MOSAIC_ERROR("WebGPU device lost: {}", _message.data); };

    WGPUDeviceLostCallbackInfo onDeviceLostCallbackInfo = {
        .callback = onDeviceLost,
        .userdata1 = nullptr,
        .userdata2 = nullptr,
    };

    WGPUUncapturedErrorCallback onDeviceError = [](const WGPUDevice* _device, WGPUErrorType _type,
                                                   WGPUStringView _message, void* _userData1,
                                                   void* _userData2)
    {
        switch (_type)
        {
            case WGPUErrorType_Validation:
                MOSAIC_ERROR("WebGPU validation error: {}", _message.data);
                break;
            case WGPUErrorType_OutOfMemory:
                MOSAIC_ERROR("WebGPU out of memory: {}", _message.data);
                break;
            case WGPUErrorType_Unknown:
                MOSAIC_ERROR("WebGPU unknown error: {}", _message.data);
                break;
            default:
                MOSAIC_ERROR("WebGPU error: {}", _message.data);
                break;
        }
    };

    WGPUUncapturedErrorCallbackInfo uncapturedErrorCallbackInfo = {
        .callback = onDeviceError,
        .userdata1 = nullptr,
        .userdata2 = nullptr,
    };

    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.deviceLostCallbackInfo = onDeviceLostCallbackInfo;
    deviceDesc.uncapturedErrorCallbackInfo = uncapturedErrorCallbackInfo;

    m_device = requestDeviceSync(m_adapter, deviceDesc);

    m_presentQueue = wgpuDeviceGetQueue(m_device);

    WGPUQueueWorkDoneCallback onQueueWorkDone =
        [](WGPUQueueWorkDoneStatus status, void* userData1, void* userData2)
    {
        switch (status)
        {
            case WGPUQueueWorkDoneStatus_Success:
                MOSAIC_INFO("WebGPU queue work done successfully!");
                break;
            case WGPUQueueWorkDoneStatus_Error:
                MOSAIC_ERROR("WebGPU queue work done with error!");
                break;
#if defined(WEBGPU_BACKEND_DAWN)
            case WGPUQueueWorkDoneStatus_InstanceDropped:
                MOSAIC_ERROR("WebGPU queue work done with unknown status!");
                break;
#elif defined(WEBGPU_BACKEND_WGPU)
            case WGPUQueueWorkDoneStatus_Unknown:
                MOSAIC_ERROR("WebGPU queue work done with unknown status!");
                break;
#endif
            default:
                MOSAIC_ERROR("WebGPU queue work done with default status!");
                break;
        }
    };

    WGPUQueueWorkDoneCallbackInfo workDoneCallbackInfo = {
        .callback = onQueueWorkDone,
        .userdata1 = nullptr,
        .userdata2 = nullptr,
    };

    wgpuQueueOnSubmittedWorkDone(m_presentQueue, workDoneCallbackInfo);

    WGPUSurfaceCapabilities surfaceCapabilities;

    if (wgpuSurfaceGetCapabilities(m_surface, m_adapter, &surfaceCapabilities) !=
        WGPUStatus_Success)
    {
        MOSAIC_ERROR("Could not get WebGPU surface capabilities!");
        return;
    }

    if (!contains(surfaceCapabilities.formats, surfaceCapabilities.formatCount,
                  WGPUTextureFormat_RGBA8UnormSrgb))
    {
        MOSAIC_ERROR("WebGPU surface does not support RGBA8UnormSrgb!");
        return;
    }

    if (!(surfaceCapabilities.usages & WGPUTextureUsage_RenderAttachment))
    {
        MOSAIC_ERROR("Surface texture does not support RenderAttachment usage!");
        return;
    }

    WGPUSurfaceConfiguration config = {};
    config.nextInChain = nullptr;
    config.width = _window.getSize().x;
    config.height = _window.getSize().y;
    config.format = WGPUTextureFormat_RGBA8UnormSrgb;
    config.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;
    config.device = m_device;
    config.presentMode = WGPUPresentMode_Fifo;
    config.alphaMode = WGPUCompositeAlphaMode_Auto;
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;

    wgpuSurfaceConfigure(m_surface, &config);

    wgpuAdapterRelease(m_adapter);
}

void WebGPURendererAPI::shutdown()
{
    wgpuQueueRelease(m_presentQueue);
    wgpuDeviceRelease(m_device);
    wgpuSurfaceRelease(m_surface);
    wgpuSurfaceUnconfigure(m_surface);
    wgpuInstanceRelease(m_instance);
}

WGPUTextureView WebGPURendererAPI::getNextSurfaceTextureView()
{
    WGPUSurfaceTexture surfaceTexture;

    wgpuSurfaceGetCurrentTexture(m_surface, &surfaceTexture);

    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
    {
        return nullptr;
    }

    WGPUTextureViewDescriptor viewDescriptor;
    viewDescriptor.nextInChain = nullptr;
    viewDescriptor.label = WGPUStringView("Surface texture view", 21);
    viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
    viewDescriptor.dimension = WGPUTextureViewDimension_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect = WGPUTextureAspect_All;
    viewDescriptor.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding;

    WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

#ifndef WEBGPU_BACKEND_WGPU
    wgpuTextureRelease(surfaceTexture.texture);
#endif

    return targetView;
}

void WebGPURendererAPI::clearScreen()
{
    WGPUTextureView targetView = getNextSurfaceTextureView();

    if (!targetView)
    {
        MOSAIC_ERROR("Could not get WebGPU target view!");
        return;
    }

    std::vector<WGPUCommandBuffer> commands;

    WGPUCommandEncoderDescriptor encoderDesc = {};
    encoderDesc.nextInChain = nullptr;
    encoderDesc.label = WGPUStringView("Clear Screen Encoder", 21);

    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_device, &encoderDesc);

    WGPURenderPassColorAttachment renderPassColorAttachment = {};
    renderPassColorAttachment.view = targetView;
    renderPassColorAttachment.resolveTarget = nullptr;
    renderPassColorAttachment.loadOp = WGPULoadOp_Clear;
    renderPassColorAttachment.storeOp = WGPUStoreOp_Store;
    renderPassColorAttachment.clearValue = WGPUColor{0.1f, 0.1f, 0.1f, 1.0f};

#ifndef WEBGPU_BACKEND_WGPU
    renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif

    WGPURenderPassDescriptor renderPassDesc = {};
    renderPassDesc.nextInChain = nullptr;
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &renderPassColorAttachment;
    renderPassDesc.depthStencilAttachment = nullptr;
    renderPassDesc.timestampWrites = nullptr;

    WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

    wgpuRenderPassEncoderEnd(renderPass);
    wgpuRenderPassEncoderRelease(renderPass);

    WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = WGPUStringView("Command buffer", 15);

    WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);

    wgpuCommandEncoderRelease(encoder);

    commands.push_back(command);

    wgpuQueueSubmit(m_presentQueue, commands.size(), commands.data());

    for (auto cmd : commands)
    {
        wgpuCommandBufferRelease(cmd);
    }

    for (int i = 0; i < 5; ++i)
    {
        MOSAIC_INFO("Polling device...");

#if defined(WEBGPU_BACKEND_DAWN)
        wgpuDeviceTick(m_device);
#elif defined(WEBGPU_BACKEND_WGPU)
        wgpuDevicePoll(m_device, false, nullptr);
#endif
    }

    wgpuTextureViewRelease(targetView);

    wgpuSurfacePresent(m_surface);
}

} // namespace mosaic::graphics
