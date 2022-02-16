project(reflow_table_server_testing LANGUAGES CXX)

include(GoogleTest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

enable_testing()
find_package(GTest REQUIRED)

add_executable(
    ${PROJECT_NAME}
    presets_holder_tests/presets_test_suite.cpp
)

mark_as_advanced(
    BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
    gmock_build_tests gtest_build_samples gtest_build_tests
    gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

target_link_libraries(
    ${PROJECT_NAME}
    GTest::gtest
    GTest::gmock
    GTest::gtest_main
    GTest::gmock_main

    presets_manager
)

target_compile_features(
    ${PROJECT_NAME}
    PUBLIC
    cxx_std_20
)

gtest_discover_tests(${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})