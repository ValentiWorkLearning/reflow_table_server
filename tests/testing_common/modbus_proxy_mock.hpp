#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <modbus_proxy/ih_modbus_proxy.hpp>

namespace ModbusProxyNs
{
class ModbusProxyMock : public IModbusProxy
{
public:
    virtual ~ModbusProxyMock() = default;
public:

    MOCK_METHOD(std::optional<std::int16_t>, readRegister, (std::uint16_t), (override));
    MOCK_METHOD(std::optional<std::vector<std::uint16_t>>, readRegisters, (std::uint16_t , std::uint16_t), (override));
    MOCK_METHOD(void, scheduleRegistersWrite, (std::uint16_t , const std::vector<std::uint16_t>&), (override));
};
}

