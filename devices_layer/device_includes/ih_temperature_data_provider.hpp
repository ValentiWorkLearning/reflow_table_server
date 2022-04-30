#pragma once
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <cstdint>
#include <memory>

namespace Reflow::Devices::Temperature
{

class ITemperatureDataProvider : public boost::intrusive_ref_counter<ITemperatureDataProvider>
{
public:
    using Ptr = boost::intrusive_ptr<ITemperatureDataProvider>;

public:
    virtual std::int32_t getRawData() = 0;
};

}; // namespace Reflow::Devices::Thermocouple
