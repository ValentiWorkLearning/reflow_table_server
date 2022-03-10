#pragma once

#include <device_includes/ih_relay_controller.hpp>
#include <device_includes/ih_thermocouple_data_provider.hpp>

namespace Reflow::Devices::Platform
{
Reflow::Devices::Relay::RelayController::Ptr getPlatformRelayController();
Reflow::Devices::Thermocouple::ThermocoupleDataProvider::Ptr getPlatformThermocoupleSensor();
}; // namespace Reflow::Devices::Platform