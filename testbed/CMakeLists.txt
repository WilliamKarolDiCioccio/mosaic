set (TESTBED_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
)

add_executable(testbed ${TESTBED_SRC})

set_target_properties(testbed PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(testbed PRIVATE mosaic)

add_custom_command(TARGET testbed POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:testbed> $<TARGET_FILE_DIR:testbed>
	COMMAND_EXPAND_LISTS
) 

target_link_libraries(testbed PRIVATE fmt::fmt)
