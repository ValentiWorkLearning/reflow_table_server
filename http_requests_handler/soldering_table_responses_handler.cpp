#include "soldering_table_responses_handler.hpp"

#include "request_utils.hpp"
#include <commands/commands_parser.hpp>
#include <platform_devices/platform_device_usings.hpp>
#include <presets/presets_holder.hpp>
#include <reflow_controller/reflow_controller.hpp>

#include <boost/range/counting_range.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace api::v1
{

class ReflowController::ReflowControllerImpl
{
public:
    void PingPong(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        Json::Value ret;
        ret["message"] = "Pong";
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        spdlog::info("Handlded get request to json!");
        callback(resp);
    }

    void CreatePreset(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        Json::Value ret;
        ret["preset-id"] = m_presetsHolder->addNewPreset(req.get()->body());
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void GetPreset(
        const HttpRequestPtr& req,
        THttpResponseCallback&& callback,
        const std::string& presetId)
    {
        if (auto presetPtr = m_presetsHolder->getPresetById(std::hash<std::string>{}(presetId));
            presetPtr)
        {
            Json::Value ret;
            ret["preset-name"] = presetPtr->presetName();

            Json::Value stagesArray{Json::arrayValue};
            presetPtr->forEachStage([&stagesArray](const auto& presetStage) {
                Json::Value temperatureStamp;
                temperatureStamp["temperature"] = presetStage.temperatureStep;
                temperatureStamp["stage-duration"] = fmt::format("{}", presetStage.stageDuration);
                stagesArray.append(temperatureStamp);
            });
            ret["stages"] = stagesArray;

            auto resp = HttpResponse::newHttpJsonResponse(ret);
            callback(resp);
        }
    }

    void GetPresets(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        Json::Value ret{Json::arrayValue};
        m_presetsHolder->forEachPreset([&ret](auto presetHash, const auto& presetPtr) {
            Json::Value presetObj;
            presetObj["preset-id"] = presetHash;
            presetObj["preset-name"] = presetPtr->presetName();
            ret.append(presetObj);
        });
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void UpdatePreset(
        const HttpRequestPtr& req,
        THttpResponseCallback&& callback,
        const std::string& presetId)
    {
        if (auto presetPtr = m_presetsHolder->getPresetById(std::hash<std::string>{}(presetId));
            presetPtr)
        {
            auto requestBody = req.get()->body();
            auto parsedState = RequestUtils::parsePresetUpdateRequest(requestBody);

            auto parseResult = Overload{
                [&presetPtr](const std::string& itemName) { presetPtr->setName(itemName); },
                [&presetPtr](RequestUtils::TStagesContainer& stagesContainer) {
                    auto countedRange{
                        boost::counting_range(std::size_t{}, stagesContainer.size() - 1)};

                    for (auto stageItem : countedRange)
                    {
                        presetPtr->replaceStageItem(
                            stageItem, std::move(stagesContainer.at(stageItem)));
                    }
                },
                [&presetPtr](std::monostate mono) {}};

            auto resp = HttpResponse::newHttpResponse();
            callback(resp);
        }
    }

    void GetStats(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        Json::Value ret;
        ret["temperature_data"] = m_thermocoupleDataProvider.getRawData();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void PushCommand(const HttpRequestPtr& req, THttpResponseCallback&& callback)
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
    THttpResponseCallback&& callback,
    const std::string& presetId)
{
    m_pControllerImpl->GetPreset(req, std::forward<THttpResponseCallback>(callback), presetId);
}

void ReflowController::GetPresets(const HttpRequestPtr& req, THttpResponseCallback&& callback)
{
    m_pControllerImpl->GetPresets(req, std::forward<THttpResponseCallback>(callback));
}

void ReflowController::CreatePreset(const HttpRequestPtr& req, THttpResponseCallback&& callback)
{
    m_pControllerImpl->CreatePreset(req, std::forward<THttpResponseCallback>(callback));
}

void ReflowController::UpdatePreset(
    const HttpRequestPtr& req,
    THttpResponseCallback&& callback,
    const std::string& presetId)
{
    m_pControllerImpl->UpdatePreset(req, std::forward<THttpResponseCallback>(callback), presetId);
}
void ReflowController::GetStats(const HttpRequestPtr& req, THttpResponseCallback&& callback)
{
    m_pControllerImpl->GetStats(req, std::forward<THttpResponseCallback>(callback));
}
void ReflowController::PushCommand(const HttpRequestPtr& req, THttpResponseCallback&& callback)
{
    m_pControllerImpl->PushCommand(req, std::forward<THttpResponseCallback>(callback));
}
void ReflowController::PingPong(const HttpRequestPtr& req, THttpResponseCallback&& callback)
{
    m_pControllerImpl->PingPong(req, std::forward<THttpResponseCallback>(callback));
}

} // namespace api::v1
