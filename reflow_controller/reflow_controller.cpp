#include "reflow_controller/reflow_controller.hpp"
#include <common/overloaded.hpp>
#include <mutex>
#include <optional>
#include <queue>
#include <spdlog/spdlog.h>
#include <thread>

// https://www.codeproject.com/Articles/991641/Revisiting-the-Active-Object-Pattern-with-Cplusplu

class RequestsQueue
{
public:
    std::optional<Reflow::Commands::TCommandVariant> getOperation()
    {
        std::unique_lock lock(queueLock);
        if (requests.empty())
            return std::nullopt;
        else
        {
            auto&& result = std::move(requests.front());
            requests.pop();
            return result;
        }
    }

    void pushOperation(Reflow::Commands::TCommandVariant commands)
    {
        std::unique_lock lock(queueLock);
        requests.push(commands);
    }

private:
    std::mutex queueLock;
    std::queue<Reflow::Commands::TCommandVariant> requests;
};

namespace Reflow::Controller
{
class ReflowProcessController::ReflowProcessControllerImpl
    : public std::enable_shared_from_this<ReflowProcessController::ReflowProcessControllerImpl>
{
public:
    ReflowProcessControllerImpl(Reflow::Presets::PresetsHolder::Ptr pPresetsHolder)
        : m_pPresetsHolder{pPresetsHolder}
    {
    }

public:
    void postCommand(Reflow::Commands::TCommandVariant contextVariant)
    {
        m_requestsQueue.pushOperation(contextVariant);
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

public:
    void runnable(const std::stop_token& stoken)
    {
        while (!stoken.stop_requested())
        {
            dispatchCommandsQueue(stoken);
        }
    }

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

private:
    std::unique_ptr<std::jthread> m_workerThread;
    std::atomic_bool m_isReflowRunning;
    RequestsQueue m_requestsQueue;
    Reflow::Presets::PresetsHolder::Ptr m_pPresetsHolder;
    Reflow::Presets::Preset::Ptr m_pActivePreset;
};

void ReflowProcessController::postCommand(Reflow::Commands::TCommandVariant commandContext)
{
    m_pImpl->postCommand(commandContext);
}

void ReflowProcessController::postInitCall()
{
    m_pImpl->postInitCall();
}
ReflowProcessController::ReflowProcessController(Reflow::Presets::PresetsHolder::Ptr pPresetsHolder)
    : m_pImpl{std::make_shared<ReflowProcessControllerImpl>(pPresetsHolder)}
{
}
ReflowProcessController::~ReflowProcessController() = default;
} // namespace Reflow::Controller