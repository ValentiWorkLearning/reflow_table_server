add_library(platform INTERFACE)

if(CROSS_BUILD_FOR_ARM)
	target_sources(platform INTERFACE file_raii_guard.hpp)
endif()

target_compile_features(platform INTERFACE cxx_std_20)
