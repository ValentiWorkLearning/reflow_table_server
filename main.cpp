#include <http_requests_handler/soldering_table_responses_handler.hpp>
#include <drogon/drogon.h>
#include <memory>
#include <common/executable_running_location.hpp>
#include <application/application_config.hpp>

 using namespace drogon;

 void dumpAvailableEndpoints()
{
    auto hdls = drogon::app().getHandlersInfo();
    for (const auto& hdl : hdls)
        std::cout << std::get<0>(hdl) << std::endl;
}

 int main(int argc, char *argv[])
{
    dumpAvailableEndpoints();
    const auto serverConfigPath{Utils::Executable::GetExecutableLocation(argv[0]) / "server_config.json"};
    const auto applicationConfigPath{Utils::Executable::GetExecutableLocation(argv[0]) / "application_config.json"};

    auto pConfigHolder = std::make_shared<Application::ConfigNs::ConfigHolder>(applicationConfigPath);
    auto reflowRequestsHandler = std::make_shared<api::v1::ReflowController>(pConfigHolder);
    reflowRequestsHandler->postInitCall();

    app().registerController(reflowRequestsHandler);
    app().loadConfigFile(serverConfigPath.string()).run();

    return 0;
}
