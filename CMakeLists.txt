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
find_package(Boost 1.50 REQUIRED COMPONENTS system log_setup log)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# TODO(!): Update name and srcs
add_library(session src/session.cc src/request_parser.cc src/reply.cc src/echo_request_handler.cc src/static_request_handler.cc src/404_request_handler.cc src/bad_request_handler.cc src/crud_request_handler.cc src/health_request_handler.cc src/sleep_request_handler.cc src/markdown_request_handler.cc src/404_handler_factory.cc src/bad_handler_factory.cc src/echo_handler_factory.cc src/static_handler_factory.cc src/crud_handler_factory.cc src/health_handler_factory.cc src/sleep_handler_factory.cc src/markdown_handler_factory.cc)
add_library(server_lib src/server.cc src/session.cc)
add_library(request_parser src/request_parser.cc)
add_library(reply src/reply.cc)
add_library(config_parser_lib src/config_parser.cc)
add_library(echo_request_handler src/echo_request_handler.cc)
add_library(static_request_handler src/static_request_handler.cc)
add_library(404_request_handler src/404_request_handler.cc)
add_library(bad_request_handler src/bad_request_handler.cc)
add_library(crud_request_handler src/crud_request_handler.cc)
add_library(health_request_handler src/health_request_handler.cc)
add_library(sleep_request_handler src/sleep_request_handler.cc)
add_library(markdown_request_handler src/markdown_request_handler.cc)
add_library(filesystem_lib include/filesystem_interface.h src/real_filesystem.cc)

add_library(echo_handler_factory src/echo_handler_factory.cc)
add_library(static_handler_factory src/static_handler_factory.cc)
add_library(bad_handler_factory src/bad_handler_factory.cc)
add_library(404_handler_factory src/404_handler_factory.cc)
add_library(crud_handler_factory src/crud_handler_factory.cc)
add_library(health_handler_factory src/health_handler_factory.cc)
add_library(sleep_handler_factory src/sleep_handler_factory.cc)
add_library(markdown_handler_factory src/markdown_handler_factory.cc)

target_link_libraries(server_lib session config_parser_lib reply request_parser filesystem_lib Boost::log_setup Boost::log)

# TODO(!): Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server session server_lib Boost::system request_parser reply echo_request_handler static_request_handler 404_request_handler bad_request_handler crud_request_handler health_request_handler sleep_request_handler markdown_request_handler config_parser_lib Boost::log_setup Boost::log echo_handler_factory static_handler_factory bad_handler_factory 404_handler_factory crud_handler_factory health_handler_factory sleep_handler_factory markdown_handler_factory filesystem_lib)

# TODO(!): Update test executable name, srcs, and deps
# add_executable(example_lib_test tests/example_lib_test.cc)
# target_link_libraries(example_lib_test example_lib gtest_main)
add_executable(config_parser_test tests/config_parser_test.cc)
target_link_libraries(config_parser_test config_parser_lib gtest_main)
target_link_libraries(config_parser_test Boost::log Boost::log_setup)

add_executable(server_test tests/server_test.cc)
target_link_libraries(server_test server_lib gtest_main)

add_executable(echo_request_handler_test tests/echo_request_handler_test.cc)
target_link_libraries(echo_request_handler_test echo_request_handler gtest_main reply)

add_executable(request_parser_test tests/request_parser_test.cc)
target_link_libraries(request_parser_test request_parser gtest_main)

add_executable(reply_test tests/reply_test.cc)
target_link_libraries(reply_test reply  gtest_main)
target_link_libraries(reply_test session Boost::log Boost::log_setup)

add_executable(session_test tests/session_test.cc)
target_link_libraries(session_test session config_parser_lib filesystem_lib gtest_main)
target_link_libraries(session_test Boost::log Boost::log_setup)

add_executable(static_request_handler_test tests/static_request_handler_test.cc)
target_link_libraries(static_request_handler_test static_request_handler markdown_request_handler gtest_main reply Boost::filesystem Boost::log)

add_executable(404_request_handler_test tests/404_request_handler_test.cc)
target_link_libraries(404_request_handler_test 404_request_handler gtest_main reply)

add_executable(crud_request_handler_test tests/crud_request_handler_test.cc src/mock_filesystem.cc)
target_link_libraries(crud_request_handler_test crud_request_handler gtest_main gmock_main reply Boost::filesystem Boost::log)

add_executable(health_request_handler_test tests/health_request_handler_test.cc)
target_link_libraries(health_request_handler_test health_request_handler gtest_main reply)

add_executable(bad_request_test tests/bad_request_test.cc)
target_link_libraries(bad_request_test bad_request_handler gtest_main reply)

add_executable(sleep_request_handler_test tests/sleep_request_handler_test.cc)
target_link_libraries(sleep_request_handler_test sleep_request_handler gtest_main reply)

add_executable(markdown_request_handler_test tests/markdown_request_handler_test.cc)
target_link_libraries(markdown_request_handler_test markdown_request_handler filesystem_lib gtest_main reply Boost::filesystem Boost::log)

# TODO(!): Update with test binary
# gtest_discover_tests(example_lib_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ../tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(echo_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(static_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(404_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(crud_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(health_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(bad_request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(markdown_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(sleep_request_handler_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Coverage")
    add_test(NAME successful_get_echo_response_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_get_echo_response_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME successful_get_static_response_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_get_static_response_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME successful_get_404_response_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_get_404_response_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME successful_create_log_file_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_create_log_file_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME successful_log_invalid_port_number_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_log_invalid_port_number_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME successful_log_invalid_config_file_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_log_invalid_config_file_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME crud_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/crud_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME successful_get_health_response_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_get_health_response_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    add_test(NAME successful_multithreading_integration_test COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/successful_multithreading_integration_test.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endif()


# TODO(!): Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
# generate_coverage_report(TARGETS example_server example_lib TESTS example_lib_test)
generate_coverage_report(TARGETS session server_lib server echo_request_handler static_request_handler crud_request_handler health_request_handler 404_request_handler bad_request_handler sleep_request_handler markdown_request_handler request_parser reply config_parser_lib TESTS session_test server_test echo_request_handler_test static_request_handler_test crud_request_handler_test health_request_handler_test 404_request_handler_test bad_request_test markdown_request_handler_test sleep_request_handler_test request_parser_test reply_test config_parser_test)

