#include "reflow_controller_test.hpp"

#include <modbus_proxy/modbus_address_constants.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <ranges>

using ::testing::Return;

TEST_F(ReflowControllerTesting, CreateEmptySuite)
{
    EXPECT_EQ(m_pReflowController->getActiveReflowPresetId(), std::nullopt);
}

TEST_F(ReflowControllerTesting, SelectActivePreset)
{
    m_pReflowController->postCommand(Reflow::Commands::SelectPreset{.presetId = m_activePresetId});

    m_pExecutor->toggleExecutable();

    EXPECT_EQ(m_pReflowController->getActiveReflowPresetId().value(), m_activePresetId);
}

TEST_F(ReflowControllerTesting, StartReflowProcessWithout)
{
    EXPECT_FALSE(m_pReflowController->isRunning());

    m_pReflowController->postCommand(Reflow::Commands::StartReflow{});
    m_pExecutor->toggleExecutable();

    EXPECT_FALSE(m_pReflowController->isRunning());
}

TEST_F(ReflowControllerTesting, StartReflowProcess)
{
    EXPECT_FALSE(m_pReflowController->isRunning());
    m_pReflowController->postCommand(Reflow::Commands::SelectPreset{.presetId = m_activePresetId});

    m_pReflowController->postCommand(Reflow::Commands::StartReflow{});
    m_pExecutor->toggleExecutable();

    EXPECT_TRUE(m_pReflowController->isRunning());
}

TEST_F(ReflowControllerTesting, SimulateReflowProcess)
{
    EXPECT_FALSE(m_pReflowController->isRunning());

    struct ModbusReturnDescriptor
    {
        std::uint16_t repitionsCount;
        std::vector<std::uint16_t> modbusData;
    };

    constexpr std::uint16_t kSurroundingTemperature = 0x19;
    using TModbusReturnContainer = std::vector<ModbusReturnDescriptor>;
    const TModbusReturnContainer returnContainer{
        ModbusReturnDescriptor{
            .repitionsCount = 6, .modbusData = {kSurroundingTemperature, 60, 0x05, 0x05}},
        ModbusReturnDescriptor{
            .repitionsCount = 22, .modbusData = {kSurroundingTemperature, 180, 0x05, 0x05}},
        ModbusReturnDescriptor{
            .repitionsCount = 1, .modbusData = {kSurroundingTemperature, 180, 0x05, 0x05}},
        ModbusReturnDescriptor{
            .repitionsCount = 3, .modbusData = {kSurroundingTemperature, 132, 0x05, 0x05}},

        ModbusReturnDescriptor{
            .repitionsCount = 3, .modbusData = {kSurroundingTemperature, 80, 0x05, 0x05}},

        ModbusReturnDescriptor{
            .repitionsCount = 3, .modbusData = {kSurroundingTemperature, 27, 0x05, 0x05}},

    };

    std::uint16_t invokeCount{};
    std::uint16_t concreteRepitions{};
    const auto elementsCount{returnContainer.size()};

    ON_CALL(*m_pModbusProxy, readRegisters)
        .WillByDefault(::testing::Invoke(
            [&invokeCount, &concreteRepitions, &returnContainer, elementsCount]()
                -> std::optional<std::vector<std::uint16_t>> {
                const auto kLastIdx{elementsCount};

                const bool hasToReturnNullopt{returnContainer.empty() || invokeCount == kLastIdx};
                if (hasToReturnNullopt)
                    return std::nullopt;

                const auto& itemNode{returnContainer[invokeCount]};
                if (concreteRepitions == itemNode.repitionsCount)
                {
                    ++invokeCount;
                    concreteRepitions = 0;

                    const bool isLast{invokeCount == kLastIdx};

                    return isLast ? std::nullopt
                                  : std::make_optional(returnContainer[invokeCount].modbusData);
                }
                else
                {
                    ++concreteRepitions;
                }

                return itemNode.modbusData;
            }));

    m_pReflowController->postCommand(Reflow::Commands::SelectPreset{.presetId = m_activePresetId});

    m_pReflowController->postCommand(Reflow::Commands::StartReflow{});

    m_pExecutor->toggleExecutable();

    EXPECT_TRUE(m_pReflowController->isRunning());

    EXPECT_CALL(
        *m_pModbusProxy,
        scheduleRegistersWrite(
            ModbusProxyNs::Address::kTargetTemperatureAddr, std::vector<std::uint16_t>{60}))
        .Times(1);

    EXPECT_CALL(
        *m_pModbusProxy,
        scheduleRegistersWrite(
            ModbusProxyNs::Address::kTargetTemperatureAddr, std::vector<std::uint16_t>{180}))
        .Times(2);

    EXPECT_CALL(
        *m_pModbusProxy,
        scheduleRegistersWrite(
            ModbusProxyNs::Address::kTargetTemperatureAddr, std::vector<std::uint16_t>{0}))
        .Times(2);

    constexpr auto kIterationsCount{50};
    for (auto itCount : std::ranges::iota_view{0, kIterationsCount})
    {
        m_pExecutor->toggleExecutable();
    }
    EXPECT_FALSE(m_pReflowController->isRunning());
}