add_library(
        modbus_proxy
        STATIC
        modbus_proxy.cpp
)

target_link_libraries(
        modbus_proxy
        PRIVATE
        Boost::boost
        libmodbus::libmodbus
)

target_include_directories(
    modbus_proxy
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc/
)
target_compile_features(modbus_proxy PUBLIC cxx_std_20)
