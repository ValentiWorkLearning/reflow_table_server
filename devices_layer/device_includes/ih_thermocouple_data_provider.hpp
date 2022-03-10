#pragma once
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <cstdint>
#include <memory>

namespace Reflow::Devices::Thermocouple
{

class ThermocoupleDataProvider : public boost::intrusive_ref_counter<ThermocoupleDataProvider>
{
public:
    ThermocoupleDataProvider();
    ~ThermocoupleDataProvider();

    using Ptr = boost::intrusive_ptr<ThermocoupleDataProvider>;

public:
    std::int32_t getRawData();

private:
    class ThermocoupleDataProviderImpl;
    std::unique_ptr<ThermocoupleDataProviderImpl> m_pImpl;
};

}; // namespace Reflow::Devices::Thermocouple