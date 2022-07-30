#include <executors/async_executor.hpp>
#include <thread>
#include <chrono>
#include <stop_token>

namespace ExecutorNs
{

class AsyncExecutor::AsyncExecutorImpl
{
public:
    AsyncExecutorImpl()
    {

    }

    ~AsyncExecutorImpl()=default;

public:
    void start(ITimedExecutor::TExecutable&& executable, std::chrono::milliseconds interval )
    {

        m_workerThread =
            std::make_unique<std::jthread>([runnable = std::move(executable),interval](std::stop_token stoken){

            while (!stoken.stop_requested())
            {
                runnable();
                std::this_thread::sleep_for(interval);
            }

            }
        );
    }
    void stop()
    {
        m_workerThread->request_stop();
    }
public:
    std::unique_ptr<std::jthread> m_workerThread;

};


AsyncExecutor::AsyncExecutor()
    :   m_pExecutorImpl{std::make_unique<AsyncExecutorImpl>()}
{
}

AsyncExecutor::~AsyncExecutor()=default;

void AsyncExecutor::start(TExecutable&& executable, std::chrono::milliseconds interval )
{
    m_pExecutorImpl->start(std::move(executable),interval);
}

void AsyncExecutor::stop()
{
    m_pExecutorImpl->stop();
}


};
