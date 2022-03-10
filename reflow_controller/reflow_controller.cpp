#include <commands/commands_list.hpp>
#include <common/overloaded.hpp>
#include <reflow_controller/reflow_controller.hpp>

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
        Reflow::Devices::Thermocouple::ThermocoupleDataProvider::Ptr pThermocouple,
        Reflow::Devices::Relay::RelayController::Ptr pRelayController)
        : m_pPresetsHolder{pPresetsHolder}
        , m_pTemperatureSensor{pThermocouple}
        , m_pRelayController{pRelayController}
    {
        m_reflowProcessData.regulatorData.dt = kSystickResolution.count();
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

    void setRegulatorParams(const RegulatorParams& regulatorParams)
    {
        m_reflowProcessData.regulatorData.k = regulatorParams.k;
        m_reflowProcessData.regulatorData.hysteresis = regulatorParams.hysteresis;
    }

    RegulatorParams getRegulatorParams() const
    {
        return RegulatorParams{
            .k = m_reflowProcessData.regulatorData.k,
            .hysteresis = m_reflowProcessData.regulatorData.hysteresis};
    }

public:
    void runnable(const std::stop_token& stoken)
    {
        while (!stoken.stop_requested())
        {
            dispatchCommandsQueue(stoken);
            processReflowStep();
            std::this_thread::sleep_for(kSystickResolution);
            m_sysTickTime += kSystickResolution;
        }
    }

private:
    static constexpr inline auto kSystickResolution = 10ms;

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

        struct RelayRegulatorData
        {
            float previousSignalValue;
            std::uint32_t dt;
            float k;
            std::uint32_t hysteresis;
            std::chrono::milliseconds previousTimepoint;
        };

        RelayRegulatorData regulatorData;
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
        spdlog::info("Reflow process started");
    }

    void handleReflowEnd()
    {
        if (!m_isReflowRunning)
            return;
        disableHeating();

        m_pActivePreset.reset();
        m_isReflowRunning = false;
        spdlog::info("Reflow process canceled");
    }

    void handlePresetSelection(Reflow::Commands::SelectPreset choice)
    {
        if (m_isReflowRunning)
            return;
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
                m_isReflowRunning = false;
                disableHeating();
                return;
            }
            else
            {
                m_reflowProcessData.reflowStep = ReflowStage::kInitStep;
            }
            break;
        }
        processRelayRegulatorStep();
    }

    void processRelayRegulatorStep()
    {
        if (!m_reflowProcessData.activePresetState)
        {
            m_pRelayController->disable();
            return;
        }

        auto currentTemperature = m_pTemperatureSensor->getRawData();
        auto k = m_reflowProcessData.regulatorData.k;
        auto dt = m_reflowProcessData.regulatorData.dt;
        auto hysteresis = m_reflowProcessData.regulatorData.hysteresis;

        float currentSignalValue{};
        if (dt != 0 && k != 0)
        {
            currentSignalValue =
                currentTemperature +
                (currentTemperature - m_reflowProcessData.regulatorData.previousSignalValue) * k /
                    dt;
            m_reflowProcessData.regulatorData.previousSignalValue = currentSignalValue;
        }
        else
        {
            currentSignalValue = currentTemperature;
        }

        auto presetState = m_reflowProcessData.activePresetState;
        const bool isUnderHysteresis =
            currentSignalValue < (presetState->temperatureStep - hysteresis) / 2;

        const bool isOverHysteresis =
            currentSignalValue > (presetState->temperatureStep + hysteresis) / 2;

        if (isUnderHysteresis)
            m_pRelayController->enable();
        else if (isOverHysteresis)
            m_pRelayController->disable();
    }

    void disableHeating()
    {
        m_pRelayController->disable();
    }

    void handleInitStep()
    {
        m_reflowProcessData.activePresetState =
            &m_pActivePreset->getStageItem(m_reflowProcessData.activeStageIndex);

        m_reflowProcessData.reflowStep = ReflowStage::kPreheat;
    }
    void handlePreheatState()
    {
        const bool canCompletePreheatStage =
            m_reflowProcessData.activePresetState->temperatureStep ==
            m_pTemperatureSensor->getRawData();

        if (canCompletePreheatStage)
        {
            m_reflowProcessData.stageCompletionTimepoint =
                m_sysTickTime + m_reflowProcessData.activePresetState->stageDuration;
            m_reflowProcessData.reflowStep = ReflowStage::kInProgress;
        }
    }

    void handleInProgressState()
    {
        if (!m_reflowProcessData.activePresetState)
            return;

        const bool canCompleteInProgressStage =
            m_sysTickTime >= m_reflowProcessData.stageCompletionTimepoint;
        if (canCompleteInProgressStage)
        {
            m_reflowProcessData.reflowStep = ReflowStage::kStageCompleted;
        }
    }

private:
    std::unique_ptr<std::jthread> m_workerThread;

    std::atomic_bool m_isReflowRunning{false};
    RequestsQueue m_requestsQueue;

    Reflow::Presets::PresetsHolder::Ptr m_pPresetsHolder;
    Reflow::Presets::Preset::Ptr m_pActivePreset;
    Reflow::Devices::Thermocouple::ThermocoupleDataProvider::Ptr m_pTemperatureSensor;
    Reflow::Devices::Relay::RelayController::Ptr m_pRelayController;

    ReflowProcessData m_reflowProcessData;
    std::chrono::milliseconds m_sysTickTime{};
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

ReflowProcessController::ReflowProcessController(
    Reflow::Presets::PresetsHolder::Ptr pPresetsHolder,
    Reflow::Devices::Thermocouple::ThermocoupleDataProvider::Ptr pThermocoupleData,
    Reflow::Devices::Relay::RelayController::Ptr pRelayController)
    : m_pImpl{std::make_shared<ReflowProcessControllerImpl>(
          pPresetsHolder,
          pThermocoupleData,
          pRelayController)}
{
}
ReflowProcessController::~ReflowProcessController() = default;
} // namespace Reflow::Controller