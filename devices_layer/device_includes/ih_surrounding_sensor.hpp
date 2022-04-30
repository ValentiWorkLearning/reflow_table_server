#pragma once

#include "ih_temperature_data_provider.hpp"

namespace Reflow::Devices::Temperature
{

class SurroundingTemperatureSensor : public ITemperatureDataProvider
{
public:
    SurroundingTemperatureSensor();
    ~SurroundingTemperatureSensor();

public:
    std::int32_t getRawData()override;

private:
    class SurroundingTemperatureSensorImpl;
    std::unique_ptr<SurroundingTemperatureSensorImpl> m_pImpl;
};

}; // namespace Reflow::Devices::Temperature
