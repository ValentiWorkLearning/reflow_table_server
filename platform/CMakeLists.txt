add_library(platform INTERFACE)

if(CROSS_BUILD_FOR_ARM)
	target_sources(platform INTERFACE file_raii_guard.hpp)
endif()
target_include_directories(platform INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_compile_features(platform INTERFACE cxx_std_20)
target_link_libraries(platform INTERFACE Boost::boost)
