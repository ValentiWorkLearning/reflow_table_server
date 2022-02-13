add_library(devices_layer STATIC empty_target.cpp)

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

    target_link_libraries(
        devices_layer
        PUBLIC
        platform
    )
endif()