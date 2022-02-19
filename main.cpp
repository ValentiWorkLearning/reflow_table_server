#include <http_requests_handler/soldering_table_responses_handler.hpp>
#include <drogon/drogon.h>
#include <memory>

using namespace drogon;

void dumpAvailableEndpoints()
{
    auto hdls = drogon::app().getHandlersInfo();
    for (const auto& hdl : hdls)
        std::cout << std::get<0>(hdl) << std::endl;
}

int main()
{
    dumpAvailableEndpoints();

    auto reflowRequestsHandler = std::make_shared<api::v1::ReflowController>();
    app().registerController(reflowRequestsHandler);
    app().loadConfigFile("./config.json").run();

    return 0;
}
