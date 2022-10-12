#pragma once

#include <filesystem>
#include <memory>

namespace Application::ConfigNs
{

struct ConfigData
{
    struct ModbusConfigData
    {
        bool isDebugMode;
        std::uint8_t modbusSlaveAddress;
        std::string serialDevicePath;
        std::uint32_t modbusBaudrate;
    };
    ModbusConfigData modbusConfig;
};

class ConfigHolder
{
public:
    ConfigHolder(const std::filesystem::path& configPath);

    ~ConfigHolder();

    using Ptr = std::shared_ptr<ConfigHolder>;

public:
    std::shared_ptr<ConfigData> getActualConfig() const;

private:
    class ConfigHolderImpl;
    std::unique_ptr<ConfigHolderImpl> m_pImpl;
};

} // namespace Application::ConfigNs
