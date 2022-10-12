#include <executors/sync_executor.hpp>
#include <spdlog/spdlog.h>

namespace ExecutorNs
{

class SyncExecutor::SyncExecutorImpl
{
public:
    SyncExecutorImpl() = default;
    ~SyncExecutorImpl()=default;

public:
    void start(ITimedExecutor::TExecutable&& executable, std::chrono::milliseconds interval )
    {
        auto executableLocal = std::move(executable);
        executableLocal();
        spdlog::info("[SyncExecutorImpl] start");
    }
    void stop()
    {

    }

private:

};


SyncExecutor::SyncExecutor()
    :   m_pExecutorImpl{std::make_unique<SyncExecutorImpl>()}
{
}

SyncExecutor::~SyncExecutor()=default;

void SyncExecutor::start(TExecutable&& executable, std::chrono::milliseconds interval )
{
    m_pExecutorImpl->start(std::move(executable),interval);
}

void SyncExecutor::stop()
{
    m_pExecutorImpl->stop();
}



};
