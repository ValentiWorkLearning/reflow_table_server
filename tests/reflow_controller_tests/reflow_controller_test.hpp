#pragma once

#include <gtest/gtest.h>
#include <presets/presets_holder.hpp>
#include <reflow_controller/reflow_controller.hpp>
#include "modbus_proxy_mock.hpp"

class ReflowControllerTesting : public ::testing::Test
{

protected:
    void SetUp() override
    {
        m_pPresetsHolder = new Reflow::Presets::PresetsHolder();
        m_pModbusProxy = new ModbusProxyNs::ModbusProxyMock();
        m_pReflowController = new Reflow::Controller::ReflowProcessController(m_pPresetsHolder,m_pModbusProxy);
        m_pReflowController->postInitCall();

        setupInitialPreset();
    }
private:
    static constexpr inline std::string_view kPresetName = "DefaultPreset";
private:

    void setupInitialPreset()
    {
        using namespace std::chrono_literals;

        auto presetId = m_pPresetsHolder->addNewPreset(kPresetName);
        auto pPreset = m_pPresetsHolder->getPresetById(presetId);

        pPreset->addStageItem(
                    Reflow::Presets::Preset::StageItem{.temperatureStep = 0, .stageDuration = 10s});
        pPreset->addStageItem(
                    Reflow::Presets::Preset::StageItem{.temperatureStep = 60, .stageDuration = 120s}
                    );

        pPreset->addStageItem(
                    Reflow::Presets::Preset::StageItem{.temperatureStep = 180, .stageDuration = 10s}
                    );
        pPreset->addStageItem(
                    Reflow::Presets::Preset::StageItem{.temperatureStep = 180, .stageDuration = 100s}
                    );

        pPreset->addStageItem(
                    Reflow::Presets::Preset::StageItem{.temperatureStep = 0, .stageDuration = 100s}
                    );

        m_activePresetId = presetId;
    }
protected:
    std::size_t m_activePresetId;
    Reflow::Presets::PresetsHolder::Ptr m_pPresetsHolder;
    Reflow::Controller::ReflowProcessController::Ptr m_pReflowController;
    ModbusProxyNs::IModbusProxy::Ptr m_pModbusProxy;
};
