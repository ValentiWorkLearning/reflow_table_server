#pragma once
#include <application/application_config.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <memory>
#include <optional>
#include <string_view>

namespace ModbusProxyNs
{

class ModbusRequestsProxy : public boost::intrusive_ref_counter<ModbusRequestsProxy>

{
public:
    using Ptr = boost::intrusive_ptr<ModbusRequestsProxy>;

public:
    ModbusRequestsProxy(const Application::ConfigNs::ConfigHolder::Ptr& pApplicationConfig);
    ~ModbusRequestsProxy();

public:
    std::optional<std::int16_t> readRegister(std::uint16_t registerAddress);
    std::optional<std::vector<std::uint16_t>> readRegisters(
        std::uint16_t registerAddress,
        std::uint16_t registersCount);

private:
    class ModbusRequestsProxyImpl;
    std::unique_ptr<ModbusRequestsProxyImpl> m_pImpl;
};

} // namespace ModbusProxyNs
