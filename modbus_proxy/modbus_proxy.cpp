#include <modbus-rtu.h>
#include <modbus_proxy/modbus_proxy.hpp>

template <auto DeleterFunction>
using CustomDeleter = std::integral_constant<decltype(DeleterFunction), DeleterFunction>;

template <typename ManagedType, auto Functor>
using PointerWrapper = std::unique_ptr<ManagedType, CustomDeleter<Functor>>;

static void modbusHandleDeleter(modbus_t* modbusContext)
{
    modbus_close(modbusContext);
    modbus_free(modbusContext);
}

using TModbusPtr = PointerWrapper<modbus_t, modbusHandleDeleter>;

namespace ModbusProxyNs
{

class ModbusRequestsProxy::ModbusRequestsProxyImpl
{
public:
    ModbusRequestsProxyImpl(const Application::ConfigNs::ConfigHolder::Ptr& pApplicationConfig)
    {
        auto pActualConfig = pApplicationConfig->getActualConfig();
        m_pModbusHandle.reset(modbus_new_rtu(
            pActualConfig->serialDevicePath.c_str(), pActualConfig->modbusBaudrate, 'N', 8, 1));
        modbus_set_response_timeout(m_pModbusHandle.get(), kSecondsTimeout, 0);
        modbus_set_slave(m_pModbusHandle.get(), pActualConfig->modbusSlaveAddress);
    }

public:
    tl::expected<std::int16_t, std::string_view> readRegister(std::uint16_t registerAddress)
    {
        return (std::int16_t{});
    }

    tl::expected<std::vector<std::int16_t>, std::string_view> readRegisters(
        std::uint16_t registerAddress,
        std::uint16_t registersCount)
    {
        return std::vector<std::int16_t>{};
    }

private:
    static constexpr inline std::int32_t kSecondsTimeout = 5;

private:
    TModbusPtr m_pModbusHandle;
};

ModbusRequestsProxy::ModbusRequestsProxy(
    const Application::ConfigNs::ConfigHolder::Ptr& pApplicationConfig)
    : m_pImpl{std::make_unique<ModbusRequestsProxy::ModbusRequestsProxyImpl>(pApplicationConfig)}
{
}

ModbusRequestsProxy::~ModbusRequestsProxy() = default;

tl::expected<std::int16_t, std::string_view> ModbusRequestsProxy::readRegister(
    std::uint16_t registerAddress)
{
    return m_pImpl->readRegister(registerAddress);
}

tl::expected<std::vector<std::int16_t>, std::string_view> ModbusRequestsProxy::readRegisters(
    std::uint16_t registerAddress,
    std::uint16_t registersCount)
{
    return m_pImpl->readRegisters(registerAddress, registersCount);
}

} // namespace ModbusProxyNs
