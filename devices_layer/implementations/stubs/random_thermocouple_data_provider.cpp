#pragma once
#include <cstdint>
#include <device_includes/ih_thermocouple_data_provider.hpp>
#include <random>

namespace Reflow::Devices::Thermocouple
{
class ThermocoupleDataProvider::ThermocoupleDataProviderImpl
{

public:
    ThermocoupleDataProviderImpl()
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


std::int32_t ThermocoupleDataProvider::getRawData()
{
    return m_pImpl->getRawData();

}
 ThermocoupleDataProvider::ThermocoupleDataProvider()
    : m_pImpl{std::make_unique<ThermocoupleDataProviderImpl>()}
{
}

ThermocoupleDataProvider::~ThermocoupleDataProvider() = default;

} // namespace Devices::Thermocouple