add_library(commands_parser STATIC  commands_parser.cpp)

target_include_directories(commands_parser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

target_link_libraries(commands_parser PUBLIC reflow_controller fmt::fmt Boost::boost nlohmann_json::nlohmann_json tl::expected)

target_compile_features(commands_parser PUBLIC cxx_std_20)