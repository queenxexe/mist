cmake_minimum_required (VERSION 3.31.3)
project(test_mist)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(GTest CONFIG REQUIRED)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

add_executable(test_mist test_mist.cc)

set_target_properties(test_mist PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test_mist/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test_mist/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test_mist/lib"
) 

target_link_libraries(test_mist 
	PRIVATE mist
	PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)

target_include_directories(test_mist
	PRIVATE ../src
)

include (GoogleTest)
gtest_discover_tests(test_mist)

add_custom_command(TARGET test_mist POST_BUILD
  	COMMAND ${CMAKE_BINARY_DIR}/test_mist/bin/test_mist
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	COMMENT "Running mist tests..."
)

target_compile_definitions(test_mist
    PRIVATE $<$<CONFIG:Debug>:DEBUG>
)