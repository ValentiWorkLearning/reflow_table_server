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
        pConfig->modbusConfig.serialDevicePath = jsonFile["serial"]["path"].get<std::string>();
        pConfig->modbusConfig.modbusBaudrate = jsonFile["serial"]["baudrate"].get<std::uint32_t>();
        pConfig->modbusConfig.modbusSlaveAddress =
            jsonFile["modbus"]["slave_address"].get<std::uint8_t>();
        pConfig->modbusConfig.isDebugMode = jsonFile["modbus"]["enable_debug_mode"].get<bool>();
        return pConfig;
    }

    std::shared_ptr<ConfigData> createDefaultConfig()
    {
        auto pConfig = std::make_shared<ConfigData>();
        pConfig->modbusConfig.serialDevicePath = "/dev/ttyUSB0";
        pConfig->modbusConfig.modbusBaudrate = 9600;
        pConfig->modbusConfig.modbusSlaveAddress = 0;
        pConfig->modbusConfig.isDebugMode = false;
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
