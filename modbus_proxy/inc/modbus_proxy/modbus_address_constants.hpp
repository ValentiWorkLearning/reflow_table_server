#pragma once
#include <cstdint>

namespace ModbusProxyNs::Address
{
constexpr inline std::uint16_t kSurroundingTemperatureAddr = 0x200F;
constexpr inline std::uint16_t kTableTempeatureAddr = 0x2010;
}
