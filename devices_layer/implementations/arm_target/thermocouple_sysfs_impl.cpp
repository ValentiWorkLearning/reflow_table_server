#include <charconv>
#include <device_includes/ih_thermocouple_data_provider.hpp>
#include <exception>
#include <file_raii_guard.hpp>
#include <fmt/format.h>
#include <string>
#include <iio_format_parser.hpp>
#include <spdlog/spdlog.h>

#include <boost/lexical_cast.hpp>

namespace Reflow::Devices::Thermocouple
{
class ThermocoupleDataProvider::ThermocoupleDataProviderImpl
{
public:
    ThermocoupleDataProviderImpl() : m_fileGuard{kThermocouplePathRaw},
            m_formatParser{new Platform::Iio::FormatParser()}
    {
        m_formatParser->setFormat(kThermocoupleDataFormat);
    }

    std::int32_t getRawData()
    {
        std::array<char, 10> kThermoCoupleData;
        auto dataSize =
            m_fileGuard.readFrom(std::span(kThermoCoupleData.data(), kThermoCoupleData.size()));
        kThermoCoupleData[dataSize + 1] = '\0';
        auto readFrom = std::string_view(kThermoCoupleData.data());
        spdlog::info("Read data from thermocouple:{}",readFrom);

        std::int32_t result;
        auto [ptr, error]{
            std::from_chars(readFrom.data(), readFrom.data() + readFrom.size(), result)};

        if (error != std::errc())
            throw std::runtime_error(fmt::format("std::from_chars failed with:{}", error));

        auto correctedResult = m_formatParser->parseValue(result);

        return boost::lexical_cast<std::int32_t>(correctedResult.value());
    }

private:
    static constexpr inline std::string_view kThermocouplePathRaw =
        "/sys/bus/iio/devices/iio:device0/in_temp_raw";

     static constexpr inline std::string_view kThermocoupleDataFormat = "be:s13/16>>3";
private:
    FileRaiiGuard m_fileGuard;
    Platform::Iio::FormatParser::Ptr m_formatParser;
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

} // namespace Reflow::Devices::Thermocouple
