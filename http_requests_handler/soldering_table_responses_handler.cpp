#include "soldering_table_responses_handler.hpp"

#include <commands/commands_parser.hpp>
#include <fmt/format.h>
#include <platform_devices/platform_device_usings.hpp>
#include <presets/presets_holder.hpp>
#include <reflow_controller/reflow_controller.hpp>
#include <spdlog/spdlog.h>

namespace api::v1
{

class ReflowController::ReflowControllerImpl
{
public:
    void PingPong(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
    {
        Json::Value ret;
        ret["message"] = "Pong";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        spdlog::info("Handlded get request to json!");
        callback(resp);
    }

    void CreatePreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
        Json::Value ret;
        ret["preset-id"] = m_presetsHolder->addNewPreset(req.get()->body());
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void GetPreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback,
        const std::string& presetId)
    {
        if (auto presetPtr = m_presetsHolder->getPresetById(std::hash<std::string>()(presetId));
            presetPtr)
        {
            Json::Value ret;
            ret["preset-name"] = presetPtr->presetName();

            Json::Value stagesArray{Json::arrayValue};
            presetPtr->forEachStage([&stagesArray](const auto& presetStage) {
                Json::Value temperatureStamp;
                temperatureStamp["temperature"] = presetStage.tempereatureStep;
                temperatureStamp["stage-duration"] = fmt::format("{}", presetStage.stageDuration);
                stagesArray.append(temperatureStamp);
            });
            ret["stages"] = stagesArray;

            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
    }

    void UpdatePreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback,
        const std::string& presetId)
    {
    }

    void GetStats(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
    {
        Json::Value ret;
        ret["temperature_data"] = m_thermocoupleDataProvider.getRawData();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void PushCommand(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
    }

private:
    Platform::ThermocoupleDataProvider m_thermocoupleDataProvider;
    Reflow::Presets::PresetsHolder::Ptr m_presetsHolder;
    Reflow::Commands::CommandsParser::Ptr m_commandsParser;
    Reflow::Controller::ReflowProcessController::Ptr m_reflowController;
};

ReflowController::ReflowController() : m_pControllerImpl{std::make_unique<ReflowControllerImpl>()}
{
}

ReflowController::~ReflowController() = default;

void ReflowController::GetPreset(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback,
    const std::string& presetId)
{
    m_pControllerImpl->GetPreset(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback), presetId);
}

void ReflowController::CreatePreset(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    m_pControllerImpl->CreatePreset(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback));
}

void ReflowController::UpdatePreset(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback,
    const std::string& presetId)
{
    m_pControllerImpl->UpdatePreset(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback), presetId);
}
void ReflowController::GetStats(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    m_pControllerImpl->GetStats(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback));
}
void ReflowController::PushCommand(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    m_pControllerImpl->PushCommand(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback));
}
void ReflowController::PingPong(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    m_pControllerImpl->PingPong(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback));
}

} // namespace api::v1