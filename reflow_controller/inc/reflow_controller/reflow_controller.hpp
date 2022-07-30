#pragma once

#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <commands/commands_list.hpp>
#include <modbus_proxy/ih_modbus_proxy.hpp>
#include <presets/presets_holder.hpp>
#include <executors/ih_executor.hpp>

#include <boost/signals2.hpp>
#include <optional>

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
            ModbusProxyNs::IModbusProxy::Ptr modbusProxyPtr,
            ExecutorNs::ITimedExecutor::Ptr executorPtr
    );
    ~ReflowProcessController();

public:
    void postInitCall();

    void postCommand(Reflow::Commands::TCommandVariant contextVariant);
    bool isRunning() const;

    std::int32_t getTableTemperature()const noexcept;
    std::int32_t getSurroundingTemperature()const noexcept;

    void setRegulatorParams(const RegulatorParams& regulatorParams);
    RegulatorParams getRegulatorParams() const;

    std::chrono::milliseconds getSystickTime() const;
    std::optional<std::size_t> getActiveReflowPresetId() const;

public:

    struct RegulatorStageContext
    {
        std::uint32_t dt;
        float k;
        float currentSignalValue;
        bool isUnderHysteresis;
        bool isOverHysteresis;
    };

    using TObservableCallback = std::function<void()>;
    using TRegulatorObserverCallback = std::function<void(const RegulatorStageContext&)>;

    boost::signals2::connection subscribeOnReflowProcessStarted(TObservableCallback observerCallback);
    boost::signals2::connection subscribeOnReflowStageCompleted(TObservableCallback observerCallback);
    boost::signals2::connection subscribeOnReflowProcessCompleted(TObservableCallback observerCallback);
    boost::signals2::connection subscribeOnRegulatorProcessing(TRegulatorObserverCallback oberverCallback);
public:
    using Ptr = boost::intrusive_ptr<ReflowProcessController>;

private:
    class ReflowProcessControllerImpl;
    std::shared_ptr<ReflowProcessControllerImpl> m_pImpl;
};
} // namespace Reflow::Controller
