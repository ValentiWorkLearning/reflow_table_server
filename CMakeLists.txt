cmake_minimum_required(VERSION 3.22)
project (drogon_test C CXX ASM)

set(CMAKE_BUILD_TYPE Debug)

# DROGON OPTIONS
set(BUILD_POSTGRESQL OFF)
set(LIBPQ_BATCH_MODE OFF)
set(BUILD_MYSQL OFF)
set(BUILD_SQLITE OFF)
set(BUILD_REDIS OFF)
set(BUILD_BROTLI OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CROSS_BUILD_FOR_ARM OFF)
option(BUILD_WITH_IIO_SUPPORT OFF)

if(CROSS_BUILD_FOR_ARM)
    set(CMAKE_CROSSCOMPILING TRUE)
    if(BUILD_WITH_IIO_SUPPORT)
        #LIBIIO OPTIONS
        set(WITH_AIO OFF)
        set(HAVE_DNS_SD OFF)
        set(WITH_USB_BACKEND OFF)
        set(WITH_XML_BACKEND OFF)
        set(WITH_SERIAL_BACKEND OFF)
        set(WITH_NETWORK_BACKEND OFF)
    endif()
endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.17/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=3bef79da16c2e031dc429e1dac87a08b9226418b300ce004cc125a82687baeef
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
                      gtest/1.11.0
                      boost/1.78.0
                      GENERATORS cmake_find_package
                      IMPORTS "bin, *.dll -> ${CMAKE_CURRENT_BINARY_DIR}"
                      IMPORTS "lib, *.dylib* -> ${CMAKE_CURRENT_BINARY_DIR}"
                      OPTIONS fmt:header_only=True
                              spdlog:header_only=True
                              boost:header_only=True
                      )

conan_cmake_autodetect(settings)

if(CROSS_BUILD_FOR_ARM)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    PROFILE ${CMAKE_CURRENT_LIST_DIR}/orange_zero_profile
                    REMOTE conancenter
                    SETTINGS ${settings})
else()
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})
endif()

find_package(jsoncpp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)

add_subdirectory(drogon)
add_subdirectory(platform)
add_subdirectory(devices_layer)
add_subdirectory(http_requests_handler)
add_subdirectory(presets)
add_subdirectory(commands_parser)
add_subdirectory(reflow_controller)

if(CROSS_BUILD_FOR_ARM AND BUILD_WITH_IIO_SUPPORT)
    add_subdirectory(libiio)
endif()

add_executable(app_test_drogon_server main.cpp)

target_include_directories(
  app_test_drogon_server
  PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/devices_layer
)
target_link_libraries(app_test_drogon_server PRIVATE reflow_requests_handler drogon)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR  CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options( app_test_drogon_server PUBLIC -g2 )
endif()

add_custom_command(
    TARGET app_test_drogon_server 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_LIST_DIR}/configs/config.json
    ${CMAKE_CURRENT_BINARY_DIR}/config.json
)

enable_testing()
add_subdirectory(tests)
