#include <charconv>
#include <device_includes/ih_surrounding_sensor.hpp>
#include <exception>
#include <file_raii_guard.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <string>

namespace Reflow::Devices::Temperature
{
class SurroundingTemperatureSensor::SurroundingTemperatureSensorImpl
{
public:
    SurroundingTemperatureSensorImpl() : m_fileGuard{kThermocouplePathRaw}
    {
    }

    std::int32_t getRawData()
    {
        std::array<char, 10> kThermoCoupleData;
        auto dataSize =
            m_fileGuard.readFrom(std::span(kThermoCoupleData.data(), kThermoCoupleData.size()));
        kThermoCoupleData[dataSize + 1] = '\0';
        auto readFrom = std::string_view(kThermoCoupleData.data());

        std::int32_t result;
        auto [ptr, error]{
            std::from_chars(readFrom.data(), readFrom.data() + readFrom.size(), result)};

        if (error != std::errc())
            throw std::runtime_error(fmt::format("std::from_chars failed with:{}", error));

        auto correctedResult = result / kDivisionCoeff;

        return correctedResult;
    }

private:
    static constexpr inline std::string_view kThermocouplePathRaw =
        "/sys/bus/w1/devices/28-0416822382ff/temperature";

    static constexpr inline std::uint32_t kDivisionCoeff = 1000;

private:
    FileRaiiGuard m_fileGuard;
};

std::int32_t SurroundingTemperatureSensor::getRawData()
{
    return m_pImpl->getRawData();
}
SurroundingTemperatureSensor::SurroundingTemperatureSensor()
    : m_pImpl{std::make_unique<SurroundingTemperatureSensorImpl>()}
{
}

SurroundingTemperatureSensor::~SurroundingTemperatureSensor() = default;

} // namespace Reflow::Devices::Temperature
