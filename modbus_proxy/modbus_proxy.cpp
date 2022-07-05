#include <modbus-rtu.h>
#include <modbus_proxy/modbus_proxy.hpp>

namespace ModbusProxyNs
{

class ModbusRequestsProxy::ModbusRequestsProxyImpl
{

public:
private:
};

ModbusRequestsProxy::ModbusRequestsProxy()
    : m_pImpl{std::make_unique<ModbusRequestsProxy::ModbusRequestsProxyImpl>()}
{
}

ModbusRequestsProxy::~ModbusRequestsProxy() = default;

std::int16_t ModbusRequestsProxy::readRegister(std::uint16_t address)noexcept
{
    return 0;
}

} // namespace ModbusProxyNs
