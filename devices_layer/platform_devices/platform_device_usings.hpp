#pragma once

#include <device_includes/ih_relay_controller.hpp>
#include <device_includes/ih_temperature_data_provider.hpp>

namespace Reflow::Devices::Platform
{
Reflow::Devices::Relay::RelayController::Ptr getPlatformRelayController();
Reflow::Devices::Temperature::ITemperatureDataProvider::Ptr getPlatformThermocoupleSensor();
Reflow::Devices::Temperature::ITemperatureDataProvider::Ptr getPlatformSurroundingTemperatureSensor();

}; // namespace Reflow::Devices::Platform
