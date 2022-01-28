cmake_minimum_required(VERSION 3.22)
project (drogon_test)

set(CMAKE_BUILD_TYPE Debug)
set(BUILD_POSTGRESQL OFF)
set(LIBPQ_BATCH_MODE OFF)
set(BUILD_MYSQL OFF)
set(BUILD_SQLITE OFF)
set(BUILD_REDIS OFF)
set(BUILD_BROTLI OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CROSSCOMPILING TRUE)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
                TLS_VERIFY ON)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES
                      jsoncpp/1.9.5
                      zlib/1.2.11
                      fmt/8.0.1
                      spdlog/1.9.2
                      GENERATORS cmake_find_package
                      IMPORTS "bin, *.dll -> ${CMAKE_CURRENT_BINARY_DIR}"
                      IMPORTS "lib, *.dylib* -> ${CMAKE_CURRENT_BINARY_DIR}"
                      OPTIONS fmt:header_only=True
                              spdlog:header_only=True
                      )

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    PROFILE ${CMAKE_CURRENT_LIST_DIR}/orange_zero_profile
                    REMOTE conancenter
                    SETTINGS ${settings})

find_package(jsoncpp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(drogon)

add_executable(app_test_drogon_server main.cpp)

target_include_directories(
  app_test_drogon_server
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/devices_layer
)
target_link_libraries(app_test_drogon_server PRIVATE drogon fmt::fmt spdlog::spdlog)
