#pragma once
#include <cstdint>

namespace Devices::Thermocouple
{

template< typename TImpl>
class BaseThermocoupleDataProvider
{

public:

    std::int32_t getRawData()
    {
        return pOffspring()->getRawData();
    }

private:

    auto pOffspring(){
        return static_cast<TImpl*>(this);
    }
};

};