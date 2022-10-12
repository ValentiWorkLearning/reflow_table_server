#pragma once
#include <cstdint>

namespace ModbusProxyNs::Address
{
constexpr inline std::uint16_t kSurroundingTempAddr = 0x200F;
constexpr inline std::uint16_t kThermocoupleAddr = 0x2010;

constexpr inline std::uint16_t kFactorAddr = 0x2011;
constexpr inline std::uint16_t kHysteresisAddr = 0x2012;
constexpr inline std::uint16_t kTargetTemperatureAddr = 0x2013;

constexpr inline std::uint16_t kModbusRegistersCount = 5;

constexpr inline std::uint16_t kRegulatorParamLength = 2;
}
