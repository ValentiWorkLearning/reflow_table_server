#pragma once
#include <application/application_config.hpp>
#include <modbus_proxy/ih_modbus_proxy.hpp>
#include <memory>
#include <optional>
#include <string_view>

namespace ModbusProxyNs
{

class ModbusRequestsProxy : public IModbusProxy

{

public:
    ModbusRequestsProxy(const Application::ConfigNs::ConfigHolder::Ptr& pApplicationConfig);
    virtual ~ModbusRequestsProxy();

public:
    std::optional<std::int16_t> readRegister(std::uint16_t registerAddress);
    std::optional<std::vector<std::uint16_t>> readRegisters(
        std::uint16_t registerAddress,
        std::uint16_t registersCount);

    void scheduleRegistersWrite(std::uint16_t registerAddress,const std::vector<std::uint16_t>& registers);

private:
    class ModbusRequestsProxyImpl;
    std::unique_ptr<ModbusRequestsProxyImpl> m_pImpl;
};

} // namespace ModbusProxyNs
