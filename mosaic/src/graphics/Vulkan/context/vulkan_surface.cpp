#include "vulkan_surface.hpp"

#include <GLFW/glfw3native.h>

#ifdef MOSAIC_PLATFORM_WINDOWS
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#endif

namespace mosaic
{
namespace graphics
{
namespace vulkan
{

#ifdef MOSAIC_PLATFORM_WINDOWS
VkResult createWin32Surface(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo,
                            const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface)
{
    auto func = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
        vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pSurface);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
#endif

void createSurface(Surface& _surface, const Instance& _instance,
                         GLFWwindow* _glfwHandle)
{
#ifdef MOSAIC_PLATFORM_WINDOWS
    const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .hinstance = GetModuleHandle(nullptr),
        .hwnd = glfwGetWin32Window(_glfwHandle),
    };

    if (createWin32Surface(_instance.instance, &surfaceCreateInfo, nullptr, &_surface.surface) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Win32 Vulkan surface!");
    }
#endif
}

void destroySurface(Surface& _surface, const Instance& _instance)
{
    vkDestroySurfaceKHR(_instance.instance, _surface.surface, nullptr);
}

} // namespace vulkan
} // namespace graphics
} // namespace mosaic
