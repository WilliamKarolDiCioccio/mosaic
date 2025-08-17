
set(CODE_CLI_SOURCES)

add_executable(codex_cli ${CODE_CLI_SOURCES} "src/main.cpp")

target_link_libraries(codex_cli PRIVATE codex)

include("${CMAKE_SOURCE_DIR}/cmake/ConfigureForBuildType.cmake")

configure_for_build_type(codex_cli)
