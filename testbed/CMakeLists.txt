# ----------------------------------------
# Source Files
# ----------------------------------------

set(TESTBED_SRC
  "src/main.cpp"
  "src/testbed.cpp"
)

# ----------------------------------------
# Executable Target
# ----------------------------------------

add_executable(testbed ${TESTBED_SRC})

if (WIN32)
  set_property(TARGET testbed PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# ----------------------------------------
# Dependencies
# ----------------------------------------

target_link_libraries(testbed
  PRIVATE
    mosaic
    fmt::fmt
)

# ----------------------------------------
# Emscripten Settings
# ----------------------------------------

if(EMSCRIPTEN)
  set_target_properties(testbed PROPERTIES SUFFIX ".html")
endif()

# ----------------------------------------
# Compiler options
# ----------------------------------------
include ("${CMAKE_SOURCE_DIR}/cmake/ConfigureForBuildType.cmake")

configure_for_build_type(testbed)

include ("${CMAKE_SOURCE_DIR}/cmake/EnableSIMD.cmake")

if(NOT EMSCRIPTEN AND NOT ANDROID)
  enable_simd(testbed)
endif()

# ----------------------------------------
# Shader Compilation Setup
# ----------------------------------------

set(SHADER_INPUT_DIR "${CMAKE_SOURCE_DIR}/assets/vulkan/shaders")
set(TARGET_SHADER_DIR "$<TARGET_FILE_DIR:testbed>/assets/shaders/bin")

if(NOT EMSCRIPTEN)
  if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_custom_command(TARGET testbed POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E echo "Compiling shaders using PowerShell..."
      COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/scripts/compile_shaders.ps1"
              -InputDir "${SHADER_INPUT_DIR}" -OutputDir "${TARGET_SHADER_DIR}"
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      COMMENT "Compiling Vulkan shaders with PowerShell directly to target directory"
    )
  elseif(CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_custom_command(TARGET testbed POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E echo "Compiling shaders using Bash..."
      COMMAND "${CMAKE_SOURCE_DIR}/scripts/compile_shaders.sh"
              "${SHADER_INPUT_DIR}" "${TARGET_SHADER_DIR}"
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      COMMENT "Compiling Vulkan shaders with Bash directly to target directory"
    )
  endif()
endif()

# ----------------------------------------
# Copy Runtime DLLs (for Windows)
# ----------------------------------------

add_custom_command(TARGET testbed POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
          $<TARGET_RUNTIME_DLLS:testbed>
          $<TARGET_FILE_DIR:testbed>
  COMMAND_EXPAND_LISTS
)
