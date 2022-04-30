#pragma once

#include <commands/commands_list.hpp>
#include <mutex>
#include <optional>
#include <queue>

namespace Reflow::Controller
{

class RequestsQueue
{
public:
    std::optional<Reflow::Commands::TCommandVariant> getOperation()
    {
        std::unique_lock lock(queueLock);
        if (requests.empty())
            return std::nullopt;
        else
        {
            auto&& result = std::move(requests.front());
            requests.pop();
            return result;
        }
    }

    void pushOperation(Reflow::Commands::TCommandVariant commands)
    {
        std::unique_lock lock(queueLock);
        requests.push(commands);
    }

private:
    std::mutex queueLock;
    std::queue<Reflow::Commands::TCommandVariant> requests;
};
}