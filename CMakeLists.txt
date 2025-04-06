cmake_minimum_required(VERSION 3.12...3.31)

# Enable Hot Reload for MSVC compilers if supported.
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  # Address sanitizer doesn't work if edit-and-continue is specified, which is enabled by default for new CMake projects
  # set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# See https://github.com/gabime/spdlog/issues/3251 and
# https://github.com/gabime/spdlog/issues/3251#issue-2648376977
if(MSVC)
  add_compile_options(/utf-8)
  set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT testbed)
endif()

# Automatically use vcpkg in manifest mode
if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE
      "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
  set(VCPKG_MANIFEST_MODE
      ON
      CACHE BOOL "Use vcpkg manifest mode")
endif()

project(
  mosaic_framework
  VERSION 0.0.1
  DESCRIPTION "A cross-platform game engine using modern C++"
  LANGUAGES CXX C
)

# Enforce C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable testing globally
enable_testing()

# Find dependencies
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

# Third party dependencies unavailable via vcpkg
add_subdirectory("vendor/webgpu")

# Despite being available via vcpkg, we need to build GLFW3 ourselves due to integration issues with the mescripten configuration
if (DEFINED EMSCRIPTEN)
    add_library(glfw INTERFACE)
    target_link_options(glfw INTERFACE "-sUSE_GLFW=3")
    target_include_directories(glfw INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw3/include")
else()
    add_subdirectory("vendor/glfw3")
endif()

add_subdirectory("vendor/glfw3webgpu")

# Add subdirectories
add_subdirectory("mosaic")
add_subdirectory("mosaic/tests")
add_subdirectory("testbed")
add_subdirectory("tests/integration")
