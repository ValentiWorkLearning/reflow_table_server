#pragma once

#include <executors/ih_executor.hpp>
#include <memory>

namespace ExecutorNs
{

class AsyncExecutor : public ITimedExecutor
{
public:
    AsyncExecutor();
    virtual ~AsyncExecutor();

public:
    void start(TExecutable&& executable, std::chrono::milliseconds interval )override;
    void stop()override;

private:
    class AsyncExecutorImpl;
    std::unique_ptr<AsyncExecutorImpl> m_pExecutorImpl;
};

};
