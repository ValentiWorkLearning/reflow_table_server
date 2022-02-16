#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "presets_fixture.hpp"

TEST_F(PresetsTesting, EmptyPresetsHolderByDefault)
{
    EXPECT_TRUE(m_presetsHolder->isEmpty());
}