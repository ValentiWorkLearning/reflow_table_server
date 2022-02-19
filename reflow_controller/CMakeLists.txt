add_library(reflow_controller STATIC reflow_controller.cpp)

target_include_directories(reflow_controller PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)

target_link_libraries(reflow_controller PUBLIC fmt::fmt Boost::boost)

target_compile_features(reflow_controller PUBLIC cxx_std_20)