#pragma once
#include <executors/ih_executor.hpp>
#include <common/meta_utils.hpp>

namespace TestingNs
{
class TestFakeExecutor : public ExecutorNs::ITimedExecutor
{

public:

    ~TestFakeExecutor() override = default;

public:
    void start(TExecutable&& executable, std::chrono::milliseconds interval)override
    {
        m_executable = std::move(executable);
        Meta::UnuseVar(interval);
    }

    void stop()
    {
    }

public:

    void toggleExecutable()
    {
        m_executable();
    }

private:
    TExecutable m_executable;

};
}