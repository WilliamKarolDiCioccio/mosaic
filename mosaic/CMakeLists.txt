set(FRAMEWORK_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/logger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/application.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/tracer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/web_gpu_instance.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/web_gpu_device.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/web_gpu_renderer_api.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/renderer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/web_gpu_commands.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/WebGPU/web_gpu_swapchain.cpp"
)

add_library(mosaic SHARED ${FRAMEWORK_SRC})

set_target_properties(mosaic PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(mosaic PRIVATE _MOSAIC_BUILD_DLL WEBGPU_CPP_IMPLEMENTATION)

target_include_directories(mosaic PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/mosaic/version.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/include/mosaic/version.h")
               
target_link_libraries(
  mosaic
  PUBLIC glm::glm-header-only
  PUBLIC glfw3webgpu
  PRIVATE webgpu
  PRIVATE glfw
  PRIVATE spdlog::spdlog
  PRIVATE nlohmann_json::nlohmann_json
)

if(COMMAND target_copy_webgpu_binaries)
    target_copy_webgpu_binaries(mosaic)
else()
    message(ERROR "target_copy_webgpu_binaries() is not defined!")
endif()
