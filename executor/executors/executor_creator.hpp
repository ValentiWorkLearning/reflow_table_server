#pragma once

#include <executors/ih_executor.hpp>
#include <memory>

namespace ExecutorNs
{

ITimedExecutor::Ptr createSyncExecutor();
ITimedExecutor::Ptr createAsyncExecutor();

}
