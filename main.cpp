#include <http_requests_handler/soldering_table_responses_handler.hpp>
#include <drogon/drogon.h>
using namespace drogon;

int main()
{
    app().setLogPath("./")
         .setLogLevel(trantor::Logger::kWarn)
         .addListener("192.168.0.116", 8080)
         .setThreadNum(1)
         .run();
}
