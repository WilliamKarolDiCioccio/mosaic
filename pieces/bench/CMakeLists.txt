set (BENCH_SOURCES
	"sparse_vs_map_bench.cpp"
)

add_executable(pieces_benchmark ${BENCH_SOURCES} "main.cpp")

target_link_libraries(pieces_benchmark PRIVATE pieces benchmark::benchmark)

include ("${CMAKE_SOURCE_DIR}/cmake/EnableSIMD.cmake")

enable_simd(pieces_benchmark)