#pragma once
#include <drogon/drogon.h>
using namespace drogon;

#include <drogon/HttpSimpleController.h>

namespace api::v1
{
class RewflowController : public drogon::HttpController<RewflowController>
{

public:
    RewflowController();
    ~RewflowController();

public:
    METHOD_LIST_BEGIN
    METHOD_ADD(RewflowController::CreatePreset, "/preset", Post);
    METHOD_ADD(RewflowController::GetPreset, "/preset/{id}", Post);
    METHOD_ADD(RewflowController::UpdatePreset, "/preset/{id}", Put);
    METHOD_ADD(RewflowController::GetStats, "/telemetry", Get);
    METHOD_ADD(RewflowController::PushCommand, "/command", Post);
    METHOD_ADD(RewflowController::PingPong, "/ping-pong", Get);
    METHOD_LIST_END

protected:
    void CreatePreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback);

    void GetPreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback);

    void UpdatePreset(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback);

    void GetStats(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback);

    void PushCommand(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback);

    void PingPong(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback);

private:
    class ReflowControllerImpl;
    std::unique_ptr<ReflowControllerImpl> m_pControllerImpl;
};
} // namespace api::v1