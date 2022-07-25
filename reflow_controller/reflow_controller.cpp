#include <commands/commands_list.hpp>
#include <common/overloaded.hpp>
#include <reflow_controller/reflow_controller.hpp>

#include <modbus_proxy/modbus_address_constants.hpp>
#include <reflow_controller/requests_queue.hpp>

#include <spdlog/spdlog.h>
#include <thread>
#include <variant>

// https://www.codeproject.com/Articles/991641/Revisiting-the-Active-Object-Pattern-with-Cplusplu
using namespace std::chrono_literals;

namespace Reflow::Controller
{
class ReflowProcessController::ReflowProcessControllerImpl
    : public std::enable_shared_from_this<ReflowProcessController::ReflowProcessControllerImpl>
{
public:
    ReflowProcessControllerImpl(
        Reflow::Presets::PresetsHolder::Ptr pPresetsHolder,
        ModbusProxyNs::IModbusProxy::Ptr modbusProxyPtr)
        : m_pPresetsHolder{pPresetsHolder}, m_pModbusProxyPtr{modbusProxyPtr}
    {
    }

public:
    void postCommand(Reflow::Commands::TCommandVariant contextVariant)
    {
        m_requestsQueue.pushOperation(contextVariant);
    }

    bool isRunning() const
    {
        return m_isReflowRunning;
    }

    void postInitCall()
    {
        m_workerThread =
            std::make_unique<std::jthread>([pWeakThis = weak_from_this()](std::stop_token stoken) {
                if (auto pStrongThis = pWeakThis.lock())
                {
                    pStrongThis->runnable(stoken);
                }
            });
    }

    std::int32_t getTableTemperature() const noexcept
    {
        return m_lastModbusData.load().tableTemperature;
    }
    std::int32_t getSurroundingTemperature() const noexcept
    {
        return m_lastModbusData.load().surroundingTemperature;
    }

    void setRegulatorParams(const RegulatorParams& regulatorParams)
    {
        constexpr auto kBeginAddress{ModbusProxyNs::Address::kFactorAddr};

        std::vector<std::uint16_t> paramsData;
        paramsData.resize(ModbusProxyNs::Address::kRegulatorParamLength);

        paramsData[0] = static_cast<std::uint16_t>(regulatorParams.k* 10);
        paramsData[1] = static_cast<std::uint16_t>(regulatorParams.hysteresis);

        m_pModbusProxyPtr->scheduleRegistersWrite(kBeginAddress, paramsData);
    }

    RegulatorParams getRegulatorParams() const
    {
        return m_lastModbusData.load().regulator;
    }

    std::chrono::milliseconds getSystickTime() const
    {
        return m_sysTickTime;
    }

    std::optional<std::size_t> getActiveReflowPresetId() const
    {
        return m_activePresetId.load();
    }

    boost::signals2::connection subscribeOnReflowProcessStarted(
        TObservableCallback observerCallback)
    {
        return m_onReflowProcessStarted.connect(observerCallback);
    }
    boost::signals2::connection subscribeOnReflowStageCompleted(
        TObservableCallback observerCallback)
    {
        return m_onReflowStageCompleted.connect(observerCallback);
    }
    boost::signals2::connection subscribeOnReflowProcessCompleted(
        TObservableCallback observerCallback)
    {
        return m_onReflowProcessCompleted.connect(observerCallback);
    }

    boost::signals2::connection subscribeOnProcessingRegulatorStage(
        TRegulatorObserverCallback observerCallback)
    {
        return m_onRegulatorStageProcessing.connect(observerCallback);
    }

public:
    void runnable(const std::stop_token& stoken)
    {
        while (!stoken.stop_requested())
        {
            dispatchCommandsQueue(stoken);
            requestModbusData();
            processReflowStep();
            std::this_thread::sleep_for(kSystickResolution);
            if (m_isReflowRunning)
                m_sysTickTime.store(m_sysTickTime.load() + kSystickResolution);
        }
    }

private:
    static constexpr inline auto kSystickResolution = 250ms;

private:
    enum class ReflowStage
    {
        kInitStep,
        kPreheat,
        kInProgress,
        kStageCompleted
    };

    struct ReflowProcessData
    {
        std::size_t activeStageIndex{};
        Reflow::Presets::Preset::StageItem* activePresetState{nullptr};
        ReflowStage reflowStep{ReflowStage::kInitStep};
        std::chrono::milliseconds stageCompletionTimepoint;
    };

    struct LastModbusData
    {
        std::int32_t tableTemperature;
        std::int32_t surroundingTemperature;

        RegulatorParams regulator;
    };

private:
    void dispatchCommandsQueue(const std::stop_token& stoken)
    {
        auto commandsVisitor = Overload{
            [this](Reflow::Commands::StartReflow) { handleReflowStart(); },
            [this](Reflow::Commands::StopReflow) { handleReflowEnd(); },
            [this](Reflow::Commands::SelectPreset presetChoice) {
                handlePresetSelection(presetChoice);
            }};

        while (auto operation{m_requestsQueue.getOperation()})
        {
            if (stoken.stop_requested())
                return;

            std::visit(commandsVisitor, operation.value());
        }
    }

    void handleReflowStart()
    {
        if (!m_pActivePreset)
        {
            spdlog::error("Can't get the active soldering preset");
            return;
        }
        m_isReflowRunning = true;

        m_onReflowProcessStarted();
    }

    void handleReflowEnd()
    {
        if (!m_isReflowRunning)
            return;
        disableHeating();
        m_reflowProcessData.activeStageIndex = 0;
        m_pActivePreset.reset();
        m_activePresetId = std::nullopt;
        m_isReflowRunning = false;
        m_sysTickTime.store(1ms);
        m_onReflowProcessCompleted();
    }

    void handlePresetSelection(Reflow::Commands::SelectPreset choice)
    {
        if (m_isReflowRunning)
            return;
        m_activePresetId = choice.presetId;
        m_pActivePreset = m_pPresetsHolder->getPresetById(choice.presetId);
    }

    void processReflowStep()
    {
        if (!m_pActivePreset)
            return;
        if (!m_isReflowRunning)
            return;

        switch (m_reflowProcessData.reflowStep)
        {
        case ReflowStage::kInitStep:
            handleInitStep();
            break;
        case ReflowStage::kPreheat:
            handlePreheatState();
            break;
        case ReflowStage::kInProgress:
            handleInProgressState();
            break;
        case ReflowStage::kStageCompleted:
            ++m_reflowProcessData.activeStageIndex;
            const bool isReflowCompleted =
                m_reflowProcessData.activeStageIndex >= m_pActivePreset->numStages();
            if (isReflowCompleted)
            {
                handleReflowEnd();
                return;
            }
            else
            {
                m_reflowProcessData.reflowStep = ReflowStage::kInitStep;
            }
            break;
        }
    }

    void disableHeating()
    {
    }

    void handleInitStep()
    {
        m_reflowProcessData.activePresetState =
            &m_pActivePreset->getStageItem(m_reflowProcessData.activeStageIndex);

        m_reflowProcessData.reflowStep = ReflowStage::kPreheat;
    }
    void handlePreheatState()
    {

        auto hysteresis = m_lastModbusData.load().regulator.hysteresis;
        auto currentTemperatureOpt =
            m_pModbusProxyPtr->readRegister(ModbusProxyNs::Address::kSurroundingTempAddr);
        if (!currentTemperatureOpt)
            return;
        auto currentTemperature = currentTemperatureOpt.value();

        auto presetExpectedTemperature = m_reflowProcessData.activePresetState->temperatureStep;

        const bool canCompletePreheatStage =
            (presetExpectedTemperature - hysteresis) <= currentTemperature &&
            (presetExpectedTemperature + hysteresis) >= currentTemperature;

        if (canCompletePreheatStage)
        {
            m_reflowProcessData.stageCompletionTimepoint =
                m_sysTickTime.load() + m_reflowProcessData.activePresetState->stageDuration;
            m_reflowProcessData.reflowStep = ReflowStage::kInProgress;
        }
    }

    void handleInProgressState()
    {
        if (!m_reflowProcessData.activePresetState)
            return;

        const bool canCompleteInProgressStage =
            m_sysTickTime.load() >= m_reflowProcessData.stageCompletionTimepoint;
        if (canCompleteInProgressStage)
        {
            m_reflowProcessData.reflowStep = ReflowStage::kStageCompleted;
        }
    }

    void requestModbusData()
    {
        auto modbusData = m_pModbusProxyPtr->readRegisters(
            ModbusProxyNs::Address::kSurroundingTempAddr,
            ModbusProxyNs::Address::kModbusRegistersCount);
        if (!modbusData)
            return;

        const auto& modbusObtainedData = modbusData.value();
        LastModbusData actualData{};
        actualData.surroundingTemperature = modbusObtainedData[0];
        actualData.tableTemperature = modbusObtainedData[1];
        actualData.regulator.hysteresis = modbusObtainedData[2];
        actualData.regulator.k = modbusObtainedData[3];

        m_lastModbusData = actualData;
        spdlog::info(
            "[reflow_controller] Surrounding temperature: {}",
            actualData.surroundingTemperature);
        spdlog::info(
                    "[reflow_controller] Table:{}",actualData.tableTemperature);
        spdlog::info(
                    "[reflow_controller] Regulator k:{}",actualData.regulator.k);

        spdlog::info(
                    "[reflow_controller] Regulator hysteresis:{}",actualData.regulator.hysteresis);
    }

private:
    ModbusProxyNs::IModbusProxy::Ptr m_pModbusProxyPtr;

    using TNotifySignal = boost::signals2::signal<void()>;
    using TRegulatorSignal = boost::signals2::signal<void(const RegulatorStageContext&)>;

    TNotifySignal m_onReflowProcessStarted;
    TNotifySignal m_onReflowProcessCompleted;
    TNotifySignal m_onReflowStageCompleted;
    TRegulatorSignal m_onRegulatorStageProcessing;

    std::unique_ptr<std::jthread> m_workerThread;

    std::atomic_bool m_isReflowRunning{false};
    RequestsQueue m_requestsQueue;

    Reflow::Presets::PresetsHolder::Ptr m_pPresetsHolder;
    Reflow::Presets::Preset::Ptr m_pActivePreset;
    std::atomic<std::optional<std::size_t>> m_activePresetId;

    ReflowProcessData m_reflowProcessData;
    std::atomic<std::chrono::milliseconds> m_sysTickTime{};
    std::atomic<LastModbusData> m_lastModbusData;
};

void ReflowProcessController::postCommand(Reflow::Commands::TCommandVariant commandContext)
{
    m_pImpl->postCommand(commandContext);
}

void ReflowProcessController::postInitCall()
{
    m_pImpl->postInitCall();
}

bool ReflowProcessController::isRunning() const
{
    return m_pImpl->isRunning();
}

void ReflowProcessController::setRegulatorParams(const RegulatorParams& regulatorParams)
{
    return m_pImpl->setRegulatorParams(regulatorParams);
}

RegulatorParams ReflowProcessController::getRegulatorParams() const
{
    return m_pImpl->getRegulatorParams();
}

std::chrono::milliseconds ReflowProcessController::getSystickTime() const
{
    return m_pImpl->getSystickTime();
}

std::optional<std::size_t> ReflowProcessController::getActiveReflowPresetId() const
{
    return m_pImpl->getActiveReflowPresetId();
}

boost::signals2::connection ReflowProcessController::subscribeOnReflowProcessStarted(
    TObservableCallback observerCallback)
{
    return m_pImpl->subscribeOnReflowProcessStarted(observerCallback);
}
boost::signals2::connection ReflowProcessController::subscribeOnReflowStageCompleted(
    TObservableCallback observerCallback)
{
    return m_pImpl->subscribeOnReflowStageCompleted(observerCallback);
}
boost::signals2::connection ReflowProcessController::subscribeOnReflowProcessCompleted(
    TObservableCallback observerCallback)
{
    return m_pImpl->subscribeOnReflowProcessCompleted(observerCallback);
}

boost::signals2::connection ReflowProcessController::subscribeOnRegulatorProcessing(
    TRegulatorObserverCallback observerCallback)
{
    return m_pImpl->subscribeOnProcessingRegulatorStage(observerCallback);
}

std::int32_t ReflowProcessController::getTableTemperature() const noexcept
{
    return m_pImpl->getTableTemperature();
}
std::int32_t ReflowProcessController::getSurroundingTemperature() const noexcept
{
    return m_pImpl->getSurroundingTemperature();
}

ReflowProcessController::ReflowProcessController(
    Reflow::Presets::PresetsHolder::Ptr pPresetsHolder,
    ModbusProxyNs::IModbusProxy::Ptr modbusProxyPtr)
    : m_pImpl{std::make_shared<ReflowProcessControllerImpl>(pPresetsHolder, modbusProxyPtr)}
{
}

ReflowProcessController::~ReflowProcessController() = default;
} // namespace Reflow::Controller
