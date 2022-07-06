add_library(application STATIC application_config.cpp)


target_include_directories(application PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

target_link_libraries(
        application
        PUBLIC
        utils
        nlohmann_json::nlohmann_json
)

target_compile_features(application PUBLIC cxx_std_20)
