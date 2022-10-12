#pragma once

#include <functional>
#include <cstdint>
#include <chrono>
#include <memory>

namespace ExecutorNs
{

class ITimedExecutor
{
public:
    virtual ~ITimedExecutor() = default;

public:
    using TExecutable = std::function<void()>;
    using Ptr = std::shared_ptr<ITimedExecutor>;

public:
    virtual void start(TExecutable&& executable, std::chrono::milliseconds interval ) = 0;
    virtual void stop() = 0;
};

};
