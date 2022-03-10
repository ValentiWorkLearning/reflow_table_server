#include <platform_devices/platform_device_usings.hpp>

namespace Reflow::Devices::Platform
{
Reflow::Devices::Relay::RelayController::Ptr getPlatformRelayController()
{
    return new Reflow::Devices::Relay::RelayController();
}
Reflow::Devices::Thermocouple::ThermocoupleDataProvider::Ptr getPlatformThermocoupleSensor()
{
    return new Reflow::Devices::Thermocouple::ThermocoupleDataProvider();
}
} // namespace Reflow::Devices::Platform