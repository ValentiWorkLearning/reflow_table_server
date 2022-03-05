#pragma once
#include <drogon/drogon.h>
using namespace drogon;

#include <drogon/HttpSimpleController.h>

namespace api::v1
{
class ReflowController : public drogon::HttpController<ReflowController, false>
{

public:
    ReflowController();
    ~ReflowController();

public:
    METHOD_LIST_BEGIN
    METHOD_ADD(ReflowController::CreatePreset, "/preset/{preset_name}", Post);
    METHOD_ADD(ReflowController::GetPreset, "/preset/{id}", Get);
    METHOD_ADD(ReflowController::UpdatePreset, "/preset/{id}", Put);
    METHOD_ADD(ReflowController::GetStats, "/telemetry", Get);
    METHOD_ADD(ReflowController::PushCommand, "/command", Post);
    METHOD_ADD(ReflowController::PingPong, "/ping-pong", Get);
    METHOD_LIST_END

protected:
    using THttpResponseCallback = std::function<void(const HttpResponsePtr&)>;
    void CreatePreset(
        const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void GetPreset(
        const HttpRequestPtr& req,
        THttpResponseCallback&& callback,
        const std::string& presetId);

    void UpdatePreset(
        const HttpRequestPtr& req,
        THttpResponseCallback&& callback,
        const std::string& presetId);

    void GetStats(
        const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void PushCommand(
        const HttpRequestPtr& req, THttpResponseCallback&& callback);

    void PingPong(
        const HttpRequestPtr& req, THttpResponseCallback&& callback);

private:
    class ReflowControllerImpl;
    std::unique_ptr<ReflowControllerImpl> m_pControllerImpl;
};
} // namespace api::v1