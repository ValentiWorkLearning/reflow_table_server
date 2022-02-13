#pragma once
#include <cstdint>
#include <device_includes/ih_thermocouple_data_provider.hpp>
#include <random>

namespace Devices::Thermocouple
{
class RandomDataProvider : public BaseThermocoupleDataProvider<RandomDataProvider>
{

public:
    RandomDataProvider()
        : m_randomGenerator{std::random_device{}()}
        , m_randomDistribution{kMinTempValue, kMaxTempValue}
    {
    }

    std::int32_t getRawData()
    {
        return m_randomDistribution(m_randomGenerator);
    }

private:
    static constexpr inline std::uint32_t kMinTempValue = 35;
    static constexpr inline std::uint32_t kMaxTempValue = 340;

private:
    std::mt19937 m_randomGenerator;

    using TDistribution = std::uniform_int_distribution<>;
    TDistribution m_randomDistribution;
};
} // namespace Devices::Thermocouple