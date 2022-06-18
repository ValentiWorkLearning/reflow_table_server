add_library(devices_layer STATIC platform_devices/platform_device_usings.cpp)

target_include_directories(
    devices_layer
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_compile_features(devices_layer PUBLIC cxx_std_20)

if(CROSS_BUILD_FOR_ARM)
    target_compile_definitions(
        devices_layer
        PUBLIC
        USE_ARM_TARGET_IMPLEMENTATION
    )
    target_sources(
        devices_layer
        PRIVATE
        implementations/arm_target/relay_controller_arm_target.cpp
        implementations/arm_target/thermocouple_sysfs_impl.cpp
        implementations/arm_target/surrounding_sensor_arm_target.cpp
    )

    target_link_libraries(
        devices_layer
        PUBLIC
        platform
        PUBLIC
        Boost::boost
        spdlog::spdlog
        libgpiod::libgpiod
        fmt::fmt
    )
else()
    target_sources(
        devices_layer
        PRIVATE
        implementations/stubs/relay_controller_stub.cpp
        implementations/stubs/random_thermocouple_data_provider.cpp
    )

    target_link_libraries(
        devices_layer
        PUBLIC
        spdlog::spdlog
        Boost::boost
    )
endif()
