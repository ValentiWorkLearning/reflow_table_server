#pragma once

#ifdef USE_ARM_TARGET_IMPLEMENTATION
#include <implementations/thermocouple_sysfs_impl.hpp>
#else
#include <implementations/random_thermocouple_data_provider.hpp>
#endif // USE_ARM_TARGET_IMPLEMENTATION

namespace Platform
{
#ifdef USE_ARM_TARGET_IMPLEMENTATION
    using ThermocoupleDataProvider = Devices::Thermocouple::SysFsDataProvider;
#else
    using ThermocoupleDataProvider = Devices::Thermocouple::RandomDataProvider;
#endif
}