set(FRAMEWORK_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/logger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/application.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/tracer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/webgpu_instance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/webgpu_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/webgpu_renderer_api.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/webgpu_commands.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/webgpu_swapchain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/vulkan_renderer_api.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/context/vulkan_instance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/context/vulkan_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/context/vulkan_surface.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/pipelines/vulkan_shader_module.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/pipelines/vulkan_pipeline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/commands/vulkan_command_pool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/Vulkan/commands/vulkan_command_buffer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/commands/vulkan_render_pass.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/vulkan_allocator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/vulkan_framebuffers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/vulkan/vulkan_swapchain.cpp"
)

if(DEFINED EMSCRIPTEN)
    add_library(mosaic STATIC ${FRAMEWORK_SRC})
else()
    add_library(mosaic SHARED ${FRAMEWORK_SRC})
endif()

target_compile_definitions(mosaic PRIVATE _MOSAIC_BUILD_DLL WEBGPU_CPP_IMPLEMENTATION)

target_include_directories(mosaic PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/mosaic/version.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/include/mosaic/version.h")
               
target_link_libraries(
  mosaic
  PUBLIC glm::glm-header-only glfw spdlog::spdlog nlohmann_json::nlohmann_json
  PRIVATE glfw3webgpu webgpu volk::volk GPUOpen::VulkanMemoryAllocator
)

target_compile_definitions(mosaic PRIVATE VK_NO_PROTOTYPES)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   target_compile_definitions(mosaic PRIVATE WIN32_LEAN_AND_MEAN GLFW_EXPOSE_NATIVE_WIN32 VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   target_compile_definitions(mosaic PRIVATE VOLK_STATIC_DEFINES GLFW_EXPOSE_NATIVE_X11 VK_USE_PLATFORM_XLIB_KHR)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_compile_definitions(mosaic PRIVATE VOLK_STATIC_DEFINES GLFW_EXPOSE_NATIVE_COCOA VK_USE_PLATFORM_MACOS_MVK)
endif()

if (DEFINED EMSCRIPTEN)
    target_link_options(mosaic PRIVATE "-sASYNCIFY")
endif()

if(COMMAND target_copy_webgpu_binaries)
    target_copy_webgpu_binaries(mosaic)
else()
    message(ERROR "target_copy_webgpu_binaries() is not defined!")
endif()
