# ----------------------------------------
# Source Files
# ----------------------------------------

set(TESTBED_SRC
  src/main.cpp
)

# ----------------------------------------
# Executable Target
# ----------------------------------------

add_executable(testbed ${TESTBED_SRC})
set_target_properties(testbed PROPERTIES LINKER_LANGUAGE CXX)

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

if(DEFINED EMSCRIPTEN)
  set_target_properties(testbed PROPERTIES SUFFIX ".html")
endif()

# ----------------------------------------
# Shader Compilation Setup
# ----------------------------------------

set(SHADER_INPUT_DIR "${CMAKE_SOURCE_DIR}/assets/vulkan/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/assets/vulkan/shaders/bin")
set(TARGET_SHADER_DIR "$<TARGET_FILE_DIR:testbed>/assets/shaders/bin")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  add_custom_command(TARGET testbed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling shaders using PowerShell..."
    COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/scripts/compile_shaders.ps1"
            -InputDir "${SHADER_INPUT_DIR}" -OutputDir "${SHADER_OUTPUT_DIR}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Compiling Vulkan shaders with PowerShell"
    COMMAND ${CMAKE_COMMAND} -E echo "Copying compiled shaders to build directory..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADER_OUTPUT_DIR}" "${TARGET_SHADER_DIR}"
    COMMENT "Copying compiled shaders to testbed output directory"
  )
else()
  add_custom_command(TARGET testbed POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling shaders using Bash..."
    COMMAND "${CMAKE_SOURCE_DIR}/scripts/compile_shaders.sh"
            "${SHADER_INPUT_DIR}" "${SHADER_OUTPUT_DIR}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Compiling Vulkan shaders with Bash"
    COMMAND ${CMAKE_COMMAND} -E echo "Copying compiled shaders to build directory..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADER_OUTPUT_DIR}" "${TARGET_SHADER_DIR}"
    COMMENT "Copying compiled shaders to testbed output directory"
  )
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
