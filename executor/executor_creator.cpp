#include <executors/executor_creator.hpp>
#include <executors/async_executor.hpp>
#include <executors/sync_executor.hpp>

namespace ExecutorNs
{

ITimedExecutor::Ptr createSyncExecutor()
{
    return std::make_shared<SyncExecutor>();
}
ITimedExecutor::Ptr createAsyncExecutor()
{
    return std::make_shared<AsyncExecutor>();
}

}
