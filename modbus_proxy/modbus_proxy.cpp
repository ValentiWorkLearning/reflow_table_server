#include <modbus-rtu.h>
#include <modbus_proxy/modbus_proxy.hpp>
#include <spdlog/spdlog.h>

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
            pActualConfig->modbusConfig.serialDevicePath.c_str(),
            pActualConfig->modbusConfig.modbusBaudrate,
            'N',
            8,
            1));
        if (!m_pModbusHandle)
        {
            spdlog::error(
                "[modbus] Can't create RTU instance for {} interface",
                pActualConfig->modbusConfig.serialDevicePath);
            return;
        }
        spdlog::info("[modbus]Created RTU modbus with settings");
        spdlog::info("[modbus]Serial device path:{}", pActualConfig->modbusConfig.serialDevicePath);
        spdlog::info("[modbus]Serial baudrate:{}", pActualConfig->modbusConfig.modbusBaudrate);
        spdlog::info("[modbus]Modbus slave id:{}", pActualConfig->modbusConfig.modbusSlaveAddress);

        auto errCode = modbus_set_response_timeout(m_pModbusHandle.get(), kSecondsTimeout, 0);
        if (isFailed(errCode))
        {
            spdlog::error(
                "[modbus] Failed to set response timeout. Error is:{}", modbus_strerror(errCode));
            return;
        }

        if (pActualConfig->modbusConfig.isDebugMode)
        {
            modbus_set_debug(m_pModbusHandle.get(), TRUE);
        }

        modbus_set_error_recovery(
            m_pModbusHandle.get(),
            static_cast<modbus_error_recovery_mode>(
                MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL));

        errCode =
            modbus_set_slave(m_pModbusHandle.get(), pActualConfig->modbusConfig.modbusSlaveAddress);
        if (isFailed(errCode))
        {
            spdlog::error(
                "[modbus] Failed to set slave address. Error is:{}", modbus_strerror(errCode));
            return;
        }

        errCode = modbus_connect(m_pModbusHandle.get());
        if (isFailed(errCode))
        {
            spdlog::error(
                "[modbus] Failed to establish connection. Error is:{}", modbus_strerror(errCode));
            return;
        }

        spdlog::info("[modbus] Modbus client has connected successfully");
    }

public:
    std::optional<std::int16_t> readRegister(std::uint16_t registerAddress)
    {
        std::uint16_t registerData{};
        constexpr int kNumRegisters = 1;
        auto errCode = modbus_read_registers(
            m_pModbusHandle.get(), registerAddress, kNumRegisters, &registerData);
        if (isFailed(errCode))
        {
            spdlog::error("[modbus] readRegister failed. Error is:{}", modbus_strerror(errCode));
            return std::nullopt;
        }
        return registerData;
    }

    std::optional<std::vector<std::uint16_t>> readRegisters(
        std::uint16_t registerAddress,
        std::uint16_t registersCount)
    {
        std::vector<std::uint16_t> registersData;
        registersData.resize(registersCount);
        auto errCode = modbus_read_registers(
            m_pModbusHandle.get(), registerAddress, registersCount, registersData.data());
        if (isFailed(errCode))
        {
            spdlog::error("[modbus] readRegisters failed. Error is:{}", modbus_strerror(errCode));
            return std::nullopt;
        }

        return registersData;
    }


    void scheduleRegistersWrite(std::uint16_t registerAddress,const std::vector<std::uint16_t>& registers)
    {
        std::uint16_t numAttempts{kModbusAttempts};

        auto errCode  = modbus_write_registers(m_pModbusHandle.get(), registerAddress, registers.size(),registers.data());
        const bool isFailedWrite{isFailed(errCode)};
        while( isFailedWrite && (numAttempts!=0))
        {
            errCode  = modbus_write_registers(m_pModbusHandle.get(), registerAddress,registers.size(), registers.data());
            if(!isFailed(errCode))
                return;

            --numAttempts;
        }

        spdlog::error("[modbus] can't execute registers write. Error is:{}", modbus_strerror(errCode));
    }
private:
    bool isFailed(int errorCode) const noexcept
    {
        return errorCode < 0;
    }

private:
    static constexpr inline std::uint16_t kModbusAttempts = 3;
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

std::optional<std::int16_t> ModbusRequestsProxy::readRegister(std::uint16_t registerAddress)
{
    return m_pImpl->readRegister(registerAddress);
}

void ModbusRequestsProxy::scheduleRegistersWrite(std::uint16_t registerAddress,const std::vector<std::uint16_t>& registers)
{
    return m_pImpl->scheduleRegistersWrite(registerAddress,registers);
}


std::optional<std::vector<std::uint16_t>> ModbusRequestsProxy::readRegisters(
    std::uint16_t registerAddress,
    std::uint16_t registersCount)
{
    return m_pImpl->readRegisters(registerAddress, registersCount);
}

} // namespace ModbusProxyNs
