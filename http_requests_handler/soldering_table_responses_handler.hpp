#pragma once
#include <drogon/drogon.h>
using namespace drogon;

#include <drogon/HttpSimpleController.h>

namespace api::v1
{
class ReflowController : public drogon::HttpController<ReflowController,false>
{

public:
    ReflowController();
    ~ReflowController();

public:
    METHOD_LIST_BEGIN
    METHOD_ADD(ReflowController::CreatePreset, "/preset", Post);
    METHOD_ADD(ReflowController::GetPreset, "/preset/{id}", Get);
    METHOD_ADD(ReflowController::UpdatePreset, "/preset/{id}", Put);
    METHOD_ADD(ReflowController::GetStats, "/telemetry", Get);
    METHOD_ADD(ReflowController::PushCommand, "/command", Post);
    METHOD_ADD(ReflowController::PingPong, "/ping-pong", Get);
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