add_library(reflow_controller STATIC reflow_controller.cpp)

target_include_directories(reflow_controller PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

target_link_libraries(
	reflow_controller
        PUBLIC
        modbus_proxy
        commands_parser
        presets_manager
        executors
        utils
	PUBLIC
	fmt::fmt
	Boost::boost
	spdlog::spdlog
)

target_compile_features(reflow_controller PUBLIC cxx_std_20)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(reflow_controller PUBLIC "-latomic")
endif()
