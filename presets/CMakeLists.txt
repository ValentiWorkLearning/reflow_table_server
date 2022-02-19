add_library(presets_manager STATIC presets_holder.cpp)
target_include_directories(presets_manager PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)
target_link_libraries(presets_manager PUBLIC fmt::fmt Boost::boost)