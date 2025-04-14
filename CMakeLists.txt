cmake_minimum_required(VERSION 3.12...3.31)

# ---------------------------
# CMake Policies and Features
# ---------------------------
if(POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
endif()

# ---------------------------
# Project Metadata
# ---------------------------
project(
  mosaic_framework
  VERSION 0.0.1
  DESCRIPTION "A cross-platform game engine using modern C++"
  LANGUAGES C CXX
)

# ---------------------------
# Compiler & C++ Configuration
# ---------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------
# Toolchain: VCPKG
# ---------------------------
if(DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
  set(VCPKG_MANIFEST_MODE ON CACHE BOOL "Use vcpkg manifest mode")
endif()

# ---------------------------
# MSVC-Specific Config
# ---------------------------
if(MSVC)
  add_compile_options(/utf-8)
  set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT testbed)
endif()

# ---------------------------
# Dependencies via find_package
# ---------------------------
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(volk CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(stduuid CONFIG REQUIRED)
find_package(Stb REQUIRED)

# ---------------------------
# Third-Party (Vendored) Libraries
# ---------------------------
add_subdirectory("vendor/webgpu")

# Special case: GLFW handling
if(DEFINED EMSCRIPTEN)
  add_library(glfw INTERFACE)
  target_link_options(glfw INTERFACE "-sUSE_GLFW=3")
  target_include_directories(glfw INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw3/include")
else()
  add_subdirectory("vendor/glfw3")
endif()

add_subdirectory("vendor/glfw3webgpu")

# ---------------------------
# Project Modules
# ---------------------------
add_subdirectory("mosaic")
add_subdirectory("mosaic/tests")

# ---------------------------
# Applications & Tests
# ---------------------------
add_subdirectory("testbed")
add_subdirectory("tests/integration")

# ---------------------------
# Enable Global Testing
# ---------------------------
enable_testing()
