add_library(executors STATIC
    sync_executor.cpp
    async_executor.cpp
    executor_creator.cpp
)


target_include_directories(
    executors
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)


target_link_libraries(
    executors
    PUBLIC
    spdlog::spdlog
)

target_compile_features(executors PUBLIC cxx_std_20)
