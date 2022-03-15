#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <commands/commands_list.hpp>
#include <platform_devices/platform_device_usings.hpp>
#include <presets/presets_holder.hpp>

namespace Reflow::Controller
{

struct RegulatorParams
{
    float k;
    std::uint32_t hysteresis;
};

class ReflowProcessController : public boost::intrusive_ref_counter<ReflowProcessController>
{

public:
    ReflowProcessController(
        Reflow::Presets::PresetsHolder::Ptr presetsHolder,
        Reflow::Devices::Thermocouple::ThermocoupleDataProvider::Ptr pThermocouple,
        Reflow::Devices::Relay::RelayController::Ptr pRelayController);
    ~ReflowProcessController();

public:
    void postInitCall();

    void postCommand(Reflow::Commands::TCommandVariant contextVariant);
    bool isRunning() const;
    void setRegulatorParams(const RegulatorParams& regulatorParams);
    RegulatorParams getRegulatorParams() const;
    std::chrono::milliseconds getSystickTime() const;

public:
    using Ptr = boost::intrusive_ptr<ReflowProcessController>;

private:
    class ReflowProcessControllerImpl;
    std::shared_ptr<ReflowProcessControllerImpl> m_pImpl;
};
} // namespace Reflow::Controller