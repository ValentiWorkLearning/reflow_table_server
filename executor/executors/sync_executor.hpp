#pragma once

#include <executors/ih_executor.hpp>
#include <memory>

namespace ExecutorNs
{

class SyncExecutor : public ITimedExecutor
{
public:
    SyncExecutor();
    virtual ~SyncExecutor();

public:
    void start(TExecutable&& executable, std::chrono::milliseconds interval )override;
    void stop()override;

private:
    class SyncExecutorImpl;
    std::unique_ptr<SyncExecutorImpl> m_pExecutorImpl;
};

};

