# ----------------------------------------
# Sources
# ----------------------------------------

set(FRAMEWORK_SRC
  # Core
  src/core/logger.cpp
  src/core/application.cpp
  src/core/tracer.cpp

  # Graphics
  src/graphics/window.cpp
  src/graphics/renderer.cpp

  # WebGPU Backend
  src/graphics/WebGPU/webgpu_instance.cpp
  src/graphics/WebGPU/webgpu_device.cpp
  src/graphics/WebGPU/webgpu_renderer_api.cpp
  src/graphics/WebGPU/webgpu_commands.cpp
  src/graphics/WebGPU/webgpu_swapchain.cpp

  # Vulkan Backend
  src/graphics/vulkan/vulkan_renderer_api.cpp
  src/graphics/vulkan/context/vulkan_instance.cpp
  src/graphics/vulkan/context/vulkan_device.cpp
  src/graphics/vulkan/context/vulkan_surface.cpp
  src/graphics/vulkan/pipelines/vulkan_shader_module.cpp
  src/graphics/vulkan/pipelines/vulkan_pipeline.cpp
  src/graphics/vulkan/commands/vulkan_command_pool.cpp
  src/graphics/vulkan/commands/vulkan_command_buffer.cpp
  src/graphics/vulkan/commands/vulkan_render_pass.cpp
  src/graphics/vulkan/vulkan_allocator.cpp
  src/graphics/vulkan/vulkan_framebuffers.cpp
  src/graphics/vulkan/vulkan_swapchain.cpp
)

# ----------------------------------------
# Target Definition
# ----------------------------------------

if(DEFINED EMSCRIPTEN)
  add_library(mosaic STATIC ${FRAMEWORK_SRC})
else()
  add_library(mosaic SHARED ${FRAMEWORK_SRC})
endif()

# ----------------------------------------
# Preprocessor Definitions
# ----------------------------------------

target_compile_definitions(mosaic
  PRIVATE
    _MOSAIC_BUILD_DLL
    WEBGPU_CPP_IMPLEMENTATION
    VK_NO_PROTOTYPES
)

# Platform-specific Vulkan and GLFW defines
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  target_compile_definitions(mosaic PRIVATE
    WIN32_LEAN_AND_MEAN
    GLFW_EXPOSE_NATIVE_WIN32
    VOLK_STATIC_DEFINES
    VK_USE_PLATFORM_WIN32_KHR
  )
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
  target_compile_definitions(mosaic PRIVATE
    GLFW_EXPOSE_NATIVE_X11
    VOLK_STATIC_DEFINES
    VK_USE_PLATFORM_XLIB_KHR
  )
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  target_compile_definitions(mosaic PRIVATE
    GLFW_EXPOSE_NATIVE_COCOA
    VOLK_STATIC_DEFINES
    VK_USE_PLATFORM_MACOS_MVK
  )
endif()

# Emscripten-specific flags
if(DEFINED EMSCRIPTEN)
  target_link_options(mosaic PRIVATE "-sASYNCIFY")
endif()

# ----------------------------------------
# Include Paths
# ----------------------------------------

target_include_directories(mosaic
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
  PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# ----------------------------------------
# Version Header
# ----------------------------------------

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/mosaic/version.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/mosaic/version.h"
)

# ----------------------------------------
# Precompiled Headers
# ----------------------------------------
target_precompile_headers(mosaic PRIVATE
  <vector>
  <string>
  <iostream>
  <algorithm>
  <unordered_map>
  <unordered_set>
  <functional>
  <memory>
  <array>
  <chrono>
)

# ----------------------------------------
# Link Libraries
# ----------------------------------------

target_link_libraries(mosaic
  PUBLIC
    glm::glm-header-only
    glfw
    spdlog::spdlog
    nlohmann_json::nlohmann_json
  PRIVATE
    glfw3webgpu
    webgpu
    volk::volk
    GPUOpen::VulkanMemoryAllocator
)

# ----------------------------------------
# Post-Build Step
# ----------------------------------------

if(COMMAND target_copy_webgpu_binaries)
  target_copy_webgpu_binaries(mosaic)
else()
  message(ERROR "target_copy_webgpu_binaries() is not defined!")
endif()
