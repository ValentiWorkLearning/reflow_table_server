#include "soldering_table_responses_handler.hpp"
#include <fmt/format.h>
#include <platform_devices/platform_device_usings.hpp>
#include <spdlog/spdlog.h>

namespace api::v1
{

class RewflowController::ReflowControllerImpl
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

private:
    Platform::ThermocoupleDataProvider m_thermocoupleDataProvider;
};

RewflowController::RewflowController() : m_pControllerImpl{std::make_unique<ReflowControllerImpl>()}
{
}

RewflowController::~RewflowController() = default;

void RewflowController::GetPreset(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
}

void RewflowController::CreatePreset(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
}

void RewflowController::UpdatePreset(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
}
void RewflowController::GetStats(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
}
void RewflowController::PushCommand(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
}
void RewflowController::PingPong(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    m_pControllerImpl->PingPong(
        req, std::forward<std::function<void(const HttpResponsePtr&)>>(callback));
}

} // namespace api::v1