#pragma once
#include "drogon/drogon.h"
using namespace drogon;

#include "drogon/HttpController.h"
#include <application/application_config.hpp>

namespace api::v1
{
class ReflowController : public drogon::HttpController<ReflowController, false>
{

public:
    ReflowController(std::shared_ptr<Application::ConfigNs::ConfigHolder> pConfigHolder);
    ~ReflowController();

public:
    METHOD_LIST_BEGIN
    METHOD_ADD(ReflowController::CreatePreset, "/preset", Post);
    METHOD_ADD(ReflowController::GetPreset, "/preset/{id}", Get);
    METHOD_ADD(ReflowController::GetPresets, "/presets", Get);
    METHOD_ADD(ReflowController::UpdatePreset, "/preset/{id}", Put);
    METHOD_ADD(ReflowController::GetStats, "/telemetry", Get);
    METHOD_ADD(ReflowController::PushCommand, "/command", Post);
    METHOD_ADD(ReflowController::PingPong, "/ping-pong", Get);
    METHOD_ADD(ReflowController::GetRegulatorParams, "/regulator", Get);
    METHOD_ADD(ReflowController::SetRegulatorParams, "/regulator", Post);
    METHOD_LIST_END
public:
    void postInitCall();

protected:
    using THttpResponseCallback = std::function<void(const HttpResponsePtr&)>;
    void CreatePreset(const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void GetPreset(
        const HttpRequestPtr& req,
        THttpResponseCallback&& callback,
        const std::string& presetId);

    void GetPresets(const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void UpdatePreset(
        const HttpRequestPtr& req,
        THttpResponseCallback&& callback,
        const std::string& presetId);

    void GetStats(const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void PushCommand(const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void PingPong(const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void GetRegulatorParams(const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void SetRegulatorParams(const HttpRequestPtr& req, THttpResponseCallback&& callback);

private:
    class ReflowControllerImpl;
    std::unique_ptr<ReflowControllerImpl> m_pControllerImpl;
};
} // namespace api::v1
