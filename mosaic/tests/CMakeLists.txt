add_executable(mosaic_tests "src/test_mosaic.cpp")

target_link_libraries(mosaic_tests PRIVATE mosaic GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(NAME mosaic_tests COMMAND test_engine)

add_custom_command(TARGET mosaic_tests POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:mosaic_tests> $<TARGET_FILE_DIR:mosaic_tests>
	COMMAND_EXPAND_LISTS
)
