# ----------------------------------------
# Sources
# ----------------------------------------

set(FRAMEWORK_SRC
  # Core
  "src/core/logger.cpp"
  "src/core/application.cpp"
  "src/core/tracer.cpp"
  "src/core/timer.cpp"
  "src/core/window.cpp"

  # Input
  "src/input/input_system.cpp"
  "src/input/input_arena.cpp"
  "src/input/raw_input_handler.cpp"
  "src/input/input_context.cpp"

  # Graphics
  "src/graphics/render_context.cpp"
  "src/graphics/render_system.cpp"

  # WebGPU Backend
  "src/graphics/WebGPU/webgpu_instance.cpp"
  "src/graphics/WebGPU/webgpu_device.cpp"
  "src/graphics/WebGPU/webgpu_render_context.cpp"
  "src/graphics/WebGPU/webgpu_render_system.cpp"
  "src/graphics/WebGPU/webgpu_commands.cpp"
  "src/graphics/WebGPU/webgpu_swapchain.cpp"
  "src/graphics/WebGPU/webgpu_pipeline.cpp"

  # Vulkan Backend
  "src/graphics/vulkan/vulkan_render_context.cpp"
  "src/graphics/vulkan/vulkan_render_system.cpp"
  "src/graphics/vulkan/context/vulkan_instance.cpp"
  "src/graphics/vulkan/context/vulkan_device.cpp"
  "src/graphics/vulkan/context/vulkan_surface.cpp"
  "src/graphics/vulkan/pipelines/vulkan_shader_module.cpp"
  "src/graphics/vulkan/pipelines/vulkan_pipeline.cpp"
  "src/graphics/vulkan/commands/vulkan_command_pool.cpp"
  "src/graphics/vulkan/commands/vulkan_command_buffer.cpp"
  "src/graphics/vulkan/commands/vulkan_render_pass.cpp"
  "src/graphics/vulkan/vulkan_allocator.cpp"
  "src/graphics/vulkan/vulkan_framebuffers.cpp"
  "src/graphics/vulkan/vulkan_swapchain.cpp"

  # External headers that need compilation
  "src/stb.cpp"
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
  PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${Stb_INCLUDE_DIR}"
  PUBLIC  
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
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
    pieces
    glm::glm-header-only
    glfw
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    stduuid
  PRIVATE
    glfw3webgpu
    webgpu
)

if(NOT DEFINED EMSCRIPTEN)
  target_link_libraries(mosaic
    PRIVATE
      volk::volk
      GPUOpen::VulkanMemoryAllocator
  )
endif()

# ----------------------------------------
# Compiler options
# ----------------------------------------
include ("${CMAKE_SOURCE_DIR}/cmake/ConfigureForBuildType.cmake")

configure_for_build_type(mosaic)

include ("${CMAKE_SOURCE_DIR}/cmake/EnableSIMD.cmake")

enable_simd(mosaic)

# ----------------------------------------
# Post-Build Step
# ----------------------------------------

if(COMMAND target_copy_webgpu_binaries)
  target_copy_webgpu_binaries(mosaic)
else()
  message(ERROR "target_copy_webgpu_binaries() is not defined!")
endif()
