set(CODEX_SOURCES
    "src/parser.cpp"
)

add_library(codex STATIC ${CODEX_SOURCES})

target_include_directories(
  codex
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

include("${CMAKE_SOURCE_DIR}/cmake/ConfigureForBuildType.cmake")

configure_for_build_type(codex)
