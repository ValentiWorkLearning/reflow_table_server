#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <memory>

namespace ModbusProxyNs
{
class IModbusProxy{
public:


    using Ptr = std::shared_ptr<IModbusProxy>;


public:
    virtual ~IModbusProxy()=default;
public:

    virtual std::optional<std::int16_t> readRegister(std::uint16_t registerAddress) = 0;
    virtual std::optional<std::vector<std::uint16_t>> readRegisters(
        std::uint16_t registerAddress,
        std::uint16_t registersCount) = 0;

    virtual void scheduleRegistersWrite(std::uint16_t registerAddress,const std::vector<std::uint16_t>& registers) = 0;
};

}
