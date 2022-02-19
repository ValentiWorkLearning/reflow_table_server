#include "soldering_table_responses_handler.hpp"
#include <fmt/format.h>
#include <platform_devices/platform_device_usings.hpp>
#include <presets/presets_holder.hpp>
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
        std::function<void(const HttpResponsePtr&)>&& callback,
        const std::string& presetName)
    {
        Json::Value ret;
        ret["preset-id"] = m_presetsHolder->addNewPreset(presetName.c_str());
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    }

    void GetPreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback,
        const std::string& presetId)
    {
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
    std::function<void(const HttpResponsePtr&)>&& callback,
    const std::string& presetName)
{
    m_pControllerImpl->CreatePreset(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback), presetName);
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