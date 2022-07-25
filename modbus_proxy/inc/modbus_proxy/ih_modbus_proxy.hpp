#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace ModbusProxyNs
{
class IModbusProxy : public boost::intrusive_ref_counter<IModbusProxy>{
public:


    using Ptr = boost::intrusive_ptr<IModbusProxy>;


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
