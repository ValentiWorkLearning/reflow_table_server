project(reflow_table_server_testing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(GoogleTest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

enable_testing()

find_package(GTest REQUIRED)
find_package(Boost REQUIRED)

add_executable(
    ${PROJECT_NAME}
    presets_holder_tests/presets_test_suite.cpp
    iio_parser_tests/iio_parser_test.cpp
    commands_parser_test/commands_parser_tests.cpp
    reflow_controller_tests/reflow_controller_test.cpp
)

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/testing_common
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    presets_manager
    platform
    commands_parser
    reflow_controller
    utils
    PRIVATE
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
    GTest::gmock_main
    Boost::boost
    tl::expected
)

target_compile_features(
    ${PROJECT_NAME}
    PUBLIC
    cxx_std_20
)
if(NOT CROSS_BUILD_FOR_ARM)
    gtest_discover_tests(${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
