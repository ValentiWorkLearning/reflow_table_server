#include <drogon/drogon.h>
using namespace drogon;

#include <drogon/HttpSimpleController.h>
#include "thermocouple_sysfs_impl.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>

class JsonCtrl : public drogon::HttpSimpleController<JsonCtrl>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) override
    {
        Json::Value ret;
        ret["message"] = "Hello, World!";
        ret["thermocouple_data"] = m_dataProvider.getRawData();
        auto resp = HttpResponse::newHttpJsonResponse(ret);
        spdlog::info("Handlded get request to json!");
        callback(resp);
    }
    PATH_LIST_BEGIN
    //list path definitions here;
    PATH_ADD("/json", Get);
    PATH_LIST_END

    private:
    Devices::Thermocouple::SysFsDataProvider m_dataProvider;
};

int main()
{
    app().setLogPath("./")
         .setLogLevel(trantor::Logger::kWarn)
         .addListener("192.168.0.116", 8080)
         .setThreadNum(16)
         .run();
}
