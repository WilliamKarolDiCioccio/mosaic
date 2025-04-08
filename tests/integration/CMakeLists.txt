add_executable(integration_tests "src/test_integration.cpp")

target_include_directories(integration_tests PRIVATE "${CMAKE_SOURCE_DIR}/mosaic/include")

target_link_libraries(integration_tests PRIVATE mosaic GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(NAME integration_tests COMMAND integration_test)

add_custom_command(TARGET integration_tests POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:integration_tests> $<TARGET_FILE_DIR:integration_tests>
	COMMAND_EXPAND_LISTS
)
