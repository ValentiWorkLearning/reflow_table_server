add_library(
	reflow_requests_handler
	STATIC
	soldering_table_responses_handler.cpp
)

target_link_libraries(
	reflow_requests_handler
	PRIVATE
	devices_layer
	presets_manager
	commands_parser
	reflow_controller
	PRIVATE
	drogon
	fmt::fmt
	spdlog::spdlog
)
target_compile_features(reflow_requests_handler PUBLIC cxx_std_20)
