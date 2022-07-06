#include <application/application_config.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

#include <nlohmann/json.hpp>

namespace Application::ConfigNs
{

class ConfigHolder::ConfigHolderImpl
{

public:
    ConfigHolderImpl(const std::filesystem::path& configPath)
    {
        if (!std::filesystem::exists(configPath))
            m_pActualConfig = createDefaultConfig();
        else
        {
            m_pActualConfig = parseConfigFile(configPath);
        }
    }

public:
    std::shared_ptr<ConfigData> getActualConfig() const
    {
        return m_pActualConfig;
    }

private:
    std::shared_ptr<ConfigData> parseConfigFile(const std::filesystem::path& configPath)
    {
        std::ifstream configFile(configPath);
        auto jsonFile = nlohmann::json::parse(configFile);
        if (jsonFile.is_discarded())
            return createDefaultConfig();

        auto pConfig = std::make_shared<ConfigData>();
        pConfig->serialDevicePath = jsonFile["serial"]["path"].get<std::string>();
        pConfig->modbusBaudrate = jsonFile["serial"]["baudrate"].get<std::uint32_t>();
        pConfig->modbusSlaveAddress =
            jsonFile["serial"]["modbus_slave_address"].get<std::uint8_t>();
        return pConfig;
    }

    std::shared_ptr<ConfigData> createDefaultConfig()
    {
        auto pConfig = std::make_shared<ConfigData>();
        pConfig->serialDevicePath = "/dev/ttyUSB0";
        pConfig->modbusBaudrate = 9600;
        pConfig->modbusSlaveAddress = 0;

        return pConfig;
    }

private:
    std::shared_ptr<ConfigData> m_pActualConfig;
};

std::shared_ptr<ConfigData> ConfigHolder::getActualConfig() const
{
    return m_pImpl->getActualConfig();
}

ConfigHolder::ConfigHolder(const std::filesystem::path& configPath)
    : m_pImpl{std::make_unique<ConfigHolderImpl>(configPath)}
{
}

ConfigHolder::~ConfigHolder() = default;

} // namespace Application::ConfigNs
