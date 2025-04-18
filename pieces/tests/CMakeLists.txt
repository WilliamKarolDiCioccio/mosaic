add_executable(pieces_tests "src/test_pieces.cpp")

target_link_libraries(pieces_tests PRIVATE pieces GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(NAME pieces_tests COMMAND test_stl)
