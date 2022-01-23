#pragma once

#include "ih_thermocouple_data_provider.hpp"
#include <file_raii_guard.hpp>
#include <string>
#include <charconv>
#include <exception>
#include <fmt/format.h>

namespace Devices::Thermocouple
{
class SysFsDataProvider: public BaseThermocoupleDataProvider<SysFsDataProvider>
{
public:

    SysFsDataProvider()
        :   m_fileGuard{kThermocouplePathRaw}
    {
    }

    std::int32_t getRawData()
    {
        std::array<char,10> kThermoCoupleData;
        auto dataSize = m_fileGuard.readFrom(std::span(kThermoCoupleData.data(),kThermoCoupleData.size()));
        kThermoCoupleData[dataSize+1] = '\0';
        auto readFrom = std::string_view(kThermoCoupleData.data());

        std::int32_t result;
        auto [ptr,error] {std::from_chars(readFrom.data(), readFrom.data() + readFrom.size(),result)};

        if(error != std::errc())
            throw std::runtime_error(fmt::format("std::from_chars failed with:{}", error));

        return result; 
    }
private:
    static constexpr inline std::string_view kThermocouplePathRaw = "/sys/bus/iio/devices/iio:device0/in_temp_raw";
private:

    FileRaiiGuard m_fileGuard;
};

}