#include <device_includes/ih_surrounding_sensor.hpp>
#include <device_includes/ih_thermocouple_device.hpp>
#include <platform_devices/platform_device_usings.hpp>

namespace Reflow::Devices::Platform
{
Reflow::Devices::Relay::RelayController::Ptr getPlatformRelayController()
{
    return new Reflow::Devices::Relay::RelayController();
}
Reflow::Devices::Temperature::ITemperatureDataProvider::Ptr getPlatformThermocoupleSensor()
{
    return new Reflow::Devices::Temperature::ThermocoupleDataProvider();
}

Reflow::Devices::Temperature::ITemperatureDataProvider::Ptr
getPlatformSurroundingTemperatureSensor()
{
    return new Reflow::Devices::Temperature::SurroundingTemperatureSensor();
}

} // namespace Reflow::Devices::Platform
