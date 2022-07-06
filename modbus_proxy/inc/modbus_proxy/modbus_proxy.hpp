#pragma once
#include <memory>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <string_view>
#include <tl/expected.hpp>
#include <application/application_config.hpp>

namespace ModbusProxyNs
{

class ModbusRequestsProxy
         :public boost::intrusive_ref_counter<ModbusRequestsProxy>

{
public:
    using Ptr = boost::intrusive_ptr<ModbusRequestsProxy>;
public:
    ModbusRequestsProxy(const Application::ConfigNs::ConfigHolder::Ptr& pApplicationConfig);
    ~ModbusRequestsProxy();

public:

    tl::expected<std::int16_t,std::string_view> readRegister(std::uint16_t registerAddress);
    tl::expected<std::vector<std::int16_t>,std::string_view> readRegisters(std::uint16_t registerAddress,std::uint16_t registersCount);
private:
    class ModbusRequestsProxyImpl;
    std::unique_ptr<ModbusRequestsProxyImpl> m_pImpl;
};

} // namespace ModbusProxyNs
