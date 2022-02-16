#include "soldering_table_responses_handler.hpp"
#include <fmt/format.h>
#include <platform_devices/platform_device_usings.hpp>
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
    }

    void GetPreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
    {
    }

    void UpdatePreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback)
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
};

ReflowController::ReflowController() : m_pControllerImpl{std::make_unique<ReflowControllerImpl>()}
{
}

ReflowController::~ReflowController() = default;

void ReflowController::GetPreset(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    m_pControllerImpl->GetPreset(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback));
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
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    m_pControllerImpl->UpdatePreset(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback));
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