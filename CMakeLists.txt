cmake_minimum_required(VERSION 3.22)
project (drogon_test C CXX ASM)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CROSS_BUILD_FOR_ARM ON)
option(BUILD_WITH_IIO_SUPPORT OFF)

if(CROSS_BUILD_FOR_ARM)
    set(CMAKE_CROSSCOMPILING TRUE)
    if(BUILD_WITH_IIO_SUPPORT)
        #LIBIIO OPTIONS
        set(WITH_AIO CACHE BOOL OFF)
        set(HAVE_DNS_SD CACHE BOOL OFF)
        set(WITH_USB_BACKEND CACHE BOOL OFF)
        set(WITH_XML_BACKEND CACHE BOOL OFF)
        set(WITH_SERIAL_BACKEND CACHE BOOL OFF)
        set(WITH_NETWORK_BACKEND CACHE BOOL OFF)
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


set(CONAN_REQUIRED_LIBRARIES_LIST
    drogon/1.7.5
    fmt/8.0.1
    spdlog/1.9.2
    gtest/1.11.0
    boost/1.79.0
    nlohmann_json/3.10.5
    tl-expected/20190710
    libmodbus/3.1.6
)

set(CONAN_PACKAGES_OPTIONS
    fmt:header_only=True
    spdlog:header_only=True
    boost:header_only=True
    drogon:with_orm=False
)

if(CROSS_BUILD_FOR_ARM)
    list(APPEND CONAN_REQUIRED_LIBRARIES_LIST libgpiod/1.6.3)
    list(APPEND CONAN_PACKAGES_OPTIONS libgpiod:enable_bindings_cxx=True)
endif()

conan_cmake_configure(REQUIRES
                      ${CONAN_REQUIRED_LIBRARIES_LIST}
                      GENERATORS cmake_find_package
                      IMPORTS "bin, *.dll -> ${CMAKE_CURRENT_BINARY_DIR}"
                      IMPORTS "lib, *.dylib* -> ${CMAKE_CURRENT_BINARY_DIR}"
                      OPTIONS ${CONAN_PACKAGES_OPTIONS}
                      )

conan_cmake_autodetect(settings)

#issue with pkg-conf
#https://github.com/conan-io/conan-center-index/issues/3026
#https://github.com/conan-io/conan-center-index/issues/9005#issuecomment-1018441082
#https://github.com/conan-io/conan/issues/8081

if(CROSS_BUILD_FOR_ARM)
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    PROFILE_BUILD default PROFILE_HOST ${CMAKE_CURRENT_LIST_DIR}/conan_profiles/orange_zero_profile
                    REMOTE conancenter
                    SETTINGS ${settings})
else()
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    PROFILE default
                    SETTINGS ${settings})
endif()


find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(tl-expected REQUIRED)
find_package(libmodbus REQUIRED)
find_package(Drogon REQUIRED)

if(CROSS_BUILD_FOR_ARM)
    find_package(libgpiod REQUIRED)
endif()

add_subdirectory(platform)
add_subdirectory(modbus_proxy)
add_subdirectory(http_requests_handler)
add_subdirectory(presets)
add_subdirectory(commands_parser)
add_subdirectory(utils)
add_subdirectory(reflow_controller)
add_subdirectory(application)

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
target_link_libraries(app_test_drogon_server PRIVATE reflow_requests_handler Drogon::Drogon utils)

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

install(TARGETS app_test_drogon_server DESTINATION bin)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/configs/server_config.json"
              "${CMAKE_CURRENT_LIST_DIR}/configs/application_config.json"
  DESTINATION bin
  )
