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
                      GENERATORS cmake_find_package
                      IMPORTS "bin, *.dll -> ${CMAKE_CURRENT_BINARY_DIR}"
                      IMPORTS "lib, *.dylib* -> ${CMAKE_CURRENT_BINARY_DIR}"
                      )

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

find_package(Jsoncpp REQUIRED)
find_package(zlib REQUIRED)
add_subdirectory(drogon)

add_executable(drogon_test_app main.cpp)
target_link_libraries(drogon_test_app PUBLIC drogon)