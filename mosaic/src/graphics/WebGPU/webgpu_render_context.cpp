#include "webgpu_render_context.hpp"

#include <vector>

#include "webgpu_device.hpp"
#include "webgpu_instance.hpp"
#include "webgpu_commands.hpp"
#include "webgpu_swapchain.hpp"

namespace mosaic
{
namespace graphics
{
namespace webgpu
{

WebGPURenderContext::WebGPURenderContext(const core::Window* _window,
                                         const RenderContextSettings& _settings)
    : RenderContext(_window, _settings)
{
    m_instance = createInstance();

    m_surface = glfwCreateWindowWGPUSurface(m_instance, m_window->getGLFWHandle());

    m_adapter = requestAdapter(m_instance, m_surface);

    if (!isAdapterSuitable(m_adapter))
    {
        MOSAIC_ERROR("WebGPU adapter is not suitable!");
        return;
    }

    m_device = createDevice(m_adapter);

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

    configureSwapchain(m_adapter, m_device, m_surface, m_window->getGLFWHandle(),
                       m_window->getFramebufferSize());

    wgpuAdapterRelease(m_adapter);
}

WebGPURenderContext::~WebGPURenderContext()
{
    wgpuQueueRelease(m_presentQueue);
    wgpuDeviceRelease(m_device);
    wgpuSurfaceRelease(m_surface);
    wgpuSurfaceUnconfigure(m_surface);
    wgpuInstanceRelease(m_instance);
}

void WebGPURenderContext::resizeFramebuffer() {}

std::pair<WGPUSurfaceTexture, WGPUTextureView> WebGPURenderContext::getNextSurfaceViewData()
{
    WGPUSurfaceTexture surfaceTexture;

    wgpuSurfaceGetCurrentTexture(m_surface, &surfaceTexture);

    if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
    {
        return std::pair<WGPUSurfaceTexture, WGPUTextureView>(nullptr, nullptr);
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

    return std::pair<WGPUSurfaceTexture, WGPUTextureView>(surfaceTexture, targetView);
}

void WebGPURenderContext::pollDevice(int _times)
{
    for (int i = 0; i < _times; ++i)
    {
#if defined(WEBGPU_BACKEND_DAWN)
        wgpuDeviceTick(m_device);
#elif defined(WEBGPU_BACKEND_WGPU)
        wgpuDevicePoll(m_device, false, nullptr);
#endif
    }
}

void WebGPURenderContext::beginFrame()
{
    auto [surfaceTexture, targetView] = getNextSurfaceViewData();

    if (!targetView)
    {
        MOSAIC_ERROR("Could not get WebGPU target view!");
        return;
    }

    std::vector<WGPUCommandBuffer> commands;

    WGPUCommandEncoder encoder = createCommandEncoder(m_device, "Clear Screen Encoder");

    WGPURenderPassEncoder renderPass =
        beginRenderPass(encoder, targetView, {0.25f, 0.1f, 0.5f, 1.0f});

    // Draw calls go here...

    endRenderPass(renderPass);

    WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
    cmdBufferDescriptor.nextInChain = nullptr;
    cmdBufferDescriptor.label = WGPUStringView("Command buffer", 15);

    WGPUCommandBuffer command = createCommandBuffer(encoder, cmdBufferDescriptor);

    commands.push_back(command);

    submitCommands(m_presentQueue, commands);

    pollDevice();

    wgpuTextureViewRelease(targetView);

#ifndef __EMSCRIPTEN__
    wgpuSurfacePresent(m_surface);
#endif

#ifdef WEBGPU_BACKEND_WGPU
    wgpuTextureRelease(surfaceTexture.texture);
#endif
}

void WebGPURenderContext::updateResources() {}

void WebGPURenderContext::drawScene() {}

void WebGPURenderContext::endFrame() {}

} // namespace webgpu
} // namespace graphics
} // namespace mosaic
