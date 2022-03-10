#include "soldering_table_responses_handler.hpp"

#include "request_utils.hpp"
#include <commands/commands_parser.hpp>
#include <platform_devices/platform_device_usings.hpp>
#include <presets/presets_holder.hpp>
#include <reflow_controller/reflow_controller.hpp>

#include <boost/range/adaptor/indexed.hpp>
#include <boost/scope_exit.hpp>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <common/overloaded.hpp>

namespace api::v1
{

class ReflowController::ReflowControllerImpl
{
public:
    ReflowControllerImpl()
        : m_thermocoupleDataProvider{Reflow::Devices::Platform::getPlatformThermocoupleSensor()}
        , m_presetsHolder{new Reflow::Presets::PresetsHolder()}
        , m_commandsParser{new Reflow::Commands::CommandsParser()}
        , m_reflowController{new Reflow::Controller::ReflowProcessController(
              m_presetsHolder,
              m_thermocoupleDataProvider,
              Reflow::Devices::Platform::getPlatformRelayController())}
    {
    }

public:
    void postInitCall()
    {
        m_reflowController->postInitCall();
    }

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
        auto presetName = RequestUtils::parsePresetName(utils::urlDecode(req.get()->getBody()));
        if (!presetName)
        {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        Json::Value ret;
        ret["preset-id"] = m_presetsHolder->addNewPreset(presetName.value());
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void GetPreset(
        const HttpRequestPtr& req,
        THttpResponseCallback&& callback,
        const std::string& presetId)
    {
        if (auto presetPtr = m_presetsHolder->getPresetById(std::stoull(presetId)); presetPtr)
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
        if (auto presetPtr = m_presetsHolder->getPresetById(std::atoll(presetId.c_str()));
            presetPtr)
        {
            auto requestBody = req.get()->body();
            auto parsedState = RequestUtils::parsePresetUpdateRequest(requestBody);

            auto parseResultVisitor = Overload{
                [&presetPtr](const std::string& itemName) { presetPtr->setName(itemName); },
                [&presetPtr](RequestUtils::TStagesContainer& stagesContainer) {
                    for (auto stageItem : stagesContainer | boost::adaptors::indexed())
                    {
                        presetPtr->replaceStageItem(
                            stageItem.index(), std::move(stageItem.value()));
                    }
                },
                [](std::monostate mono) {}};
            std::visit(parseResultVisitor, parsedState);

            auto resp = HttpResponse::newHttpResponse();
            callback(resp);
        }
    }

    void GetStats(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        Json::Value ret;
        ret["temperature_data"] = m_thermocoupleDataProvider->getRawData();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void PushCommand(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        auto commandBody = req->getBody();
        auto parseResult = m_commandsParser->parseCommandContext(commandBody);

        auto resp = HttpResponse::newHttpResponse();
        if (!parseResult)
        {
            resp->setStatusCode(k400BadRequest);
            resp->setBody(parseResult.error().data());
            callback(resp);
        }
        else
        {
            m_reflowController->postCommand(parseResult.value());
            callback(resp);
        }
    }

    void GetRegulatorParams(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        Json::Value ret;
        const auto& regulatorParams{m_reflowController->getRegulatorParams()};

        ret["hysteresis"] = regulatorParams.hysteresis;
        ret["k"] = regulatorParams.k;

        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void SetRegulatorParams(const HttpRequestPtr& req, THttpResponseCallback&& callback)
    {
        auto resp = HttpResponse::newHttpResponse();
        if (m_reflowController->isRunning())
        {
            resp->setStatusCode(k400BadRequest);
            resp->setBody("Reflow process is running. Can't change the regulator settings");
        }
        else
        {
            auto regulatorData = RequestUtils::parseRegulatorParams(req->body());
            if (!regulatorData)
            {
                resp->setStatusCode(k400BadRequest);
                resp->setBody(std::string(regulatorData.error()));
            }
            else
            {
                m_reflowController->setRegulatorParams(regulatorData.value());
            }
        }
        callback(resp);
    }

private:
    Reflow::Devices::Thermocouple::ThermocoupleDataProvider::Ptr m_thermocoupleDataProvider;
    Reflow::Presets::PresetsHolder::Ptr m_presetsHolder;
    Reflow::Commands::CommandsParser::Ptr m_commandsParser;
    Reflow::Controller::ReflowProcessController::Ptr m_reflowController;
};

ReflowController::ReflowController() : m_pControllerImpl{std::make_unique<ReflowControllerImpl>()}
{
}

ReflowController::~ReflowController() = default;

void ReflowController::postInitCall()
{
    m_pControllerImpl->postInitCall();
}

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

void ReflowController::GetRegulatorParams(
    const HttpRequestPtr& req,
    THttpResponseCallback&& callback)
{
    m_pControllerImpl->GetRegulatorParams(req, std::forward<THttpResponseCallback>(callback));
}

void ReflowController::SetRegulatorParams(
    const HttpRequestPtr& req,
    THttpResponseCallback&& callback)
{
    m_pControllerImpl->SetRegulatorParams(req, std::forward<THttpResponseCallback>(callback));
}

} // namespace api::v1
