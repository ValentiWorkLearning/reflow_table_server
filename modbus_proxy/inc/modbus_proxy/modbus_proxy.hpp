#pragma once
#include <memory>
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

namespace ModbusProxyNs
{

class ModbusRequestsProxy
         :public boost::intrusive_ref_counter<ModbusRequestsProxy>

{
public:
    using Ptr = boost::intrusive_ptr<ModbusRequestsProxy>;
public:
    ModbusRequestsProxy();
    ~ModbusRequestsProxy();

public:

    std::int16_t readRegister(std::uint16_t registerAddress)noexcept;

private:
    class ModbusRequestsProxyImpl;
    std::unique_ptr<ModbusRequestsProxyImpl> m_pImpl;
};

} // namespace ModbusProxyNs
