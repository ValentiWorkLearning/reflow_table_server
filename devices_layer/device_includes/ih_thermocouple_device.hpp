#pragma once

#include "ih_temperature_data_provider.hpp"

namespace Reflow::Devices::Temperature
{

class ThermocoupleDataProvider : public ITemperatureDataProvider
{
public:
    ThermocoupleDataProvider();
    ~ThermocoupleDataProvider();

public:
    std::int32_t getRawData()override;

private:
    class ThermocoupleDataProviderImpl;
    std::unique_ptr<ThermocoupleDataProviderImpl> m_pImpl;
};

}; // namespace Reflow::Devices::Temperature
