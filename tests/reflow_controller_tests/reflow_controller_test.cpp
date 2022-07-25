#include "reflow_controller_test.hpp"


TEST_F(ReflowControllerTesting, CreateEmptySuite)
{
    EXPECT_EQ(m_pReflowController->getActiveReflowPresetId(),std::nullopt);
}

TEST_F(ReflowControllerTesting, SelectActivePreset)
{
    m_pReflowController->postCommand(Reflow::Commands::SelectPreset{.presetId = m_activePresetId});

    EXPECT_EQ(m_pReflowController->getActiveReflowPresetId().value(),m_activePresetId);
}

