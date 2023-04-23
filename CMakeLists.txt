cmake_minimum_required(VERSION 3.10.0)
project(bingsoo)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# TODO(!): Update name and srcs
add_library(session src/session.cc src/request_parser.cc src/reply.cc)
add_library(server_lib src/server.cc src/session.cc)
add_library(request_parser src/request_parser.cc)
add_library(reply src/reply.cc)
add_library(config_parser_lib src/config_parser.cc)


# TODO(!): Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server session server_lib Boost::system request_parser reply config_parser_lib)

# TODO(!): Update test executable name, srcs, and deps
# add_executable(example_lib_test tests/example_lib_test.cc)
# target_link_libraries(example_lib_test example_lib gtest_main)
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main)
add_executable(request_parser_test tests/request_parser_test.cc)
target_link_libraries(request_parser_test request_parser gtest_main)
add_executable(reply_test tests/reply_test.cc)
target_link_libraries(reply_test reply gtest_main Boost::system session)

# TODO(!): Update with test binary
# gtest_discover_tests(example_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ../tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)


# TODO(!): Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
# generate_coverage_report(TARGETS example_server example_lib TESTS example_lib_test)
generate_coverage_report(TARGETS server session server_lib Boost::system request_parser reply config_parser_lib TESTS config_parser_test reply_test request_parser_test)