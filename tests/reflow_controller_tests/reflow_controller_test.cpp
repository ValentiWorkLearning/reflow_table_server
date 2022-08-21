#include "reflow_controller_test.hpp"

TEST_F(ReflowControllerTesting, CreateEmptySuite)
{
    EXPECT_EQ(m_pReflowController->getActiveReflowPresetId(), std::nullopt);
}

TEST_F(ReflowControllerTesting, SelectActivePreset)
{
    m_pReflowController->postCommand(Reflow::Commands::SelectPreset{.presetId = m_activePresetId});

    m_pExecutor->toggleExecutable();

    EXPECT_EQ(m_pReflowController->getActiveReflowPresetId().value(), m_activePresetId);
}


TEST_F(ReflowControllerTesting, StartReflowProcessWithout)
{
    EXPECT_FALSE(m_pReflowController->isRunning());

    m_pReflowController->postCommand(Reflow::Commands::StartReflow{});
    m_pExecutor->toggleExecutable();

    EXPECT_FALSE(m_pReflowController->isRunning());
}

TEST_F(ReflowControllerTesting, StartReflowProcess)
{
    EXPECT_FALSE(m_pReflowController->isRunning());
    m_pReflowController->postCommand(Reflow::Commands::SelectPreset{.presetId = m_activePresetId});

    m_pReflowController->postCommand(Reflow::Commands::StartReflow{});
    m_pExecutor->toggleExecutable();

    EXPECT_TRUE(m_pReflowController->isRunning());
}