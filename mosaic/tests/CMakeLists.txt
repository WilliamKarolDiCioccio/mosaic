add_executable(mosaic_tests "test_mosaic.cpp")

target_include_directories(mosaic_tests PRIVATE "${CMAKE_SOURCE_DIR}/mosaic/include")

target_link_libraries(mosaic_tests PRIVATE mosaic GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(NAME mosaic_tests COMMAND test_engine)

add_custom_command(TARGET mosaic_tests POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:mosaic_tests> $<TARGET_FILE_DIR:mosaic_tests>
	COMMAND_EXPAND_LISTS
)
