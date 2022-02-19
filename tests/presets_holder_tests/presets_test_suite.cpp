#include "presets_fixture.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

TEST_F(PresetsTesting, EmptyPresetsHolderByDefault)
{
    EXPECT_TRUE(m_presetsHolder->isEmpty());
}

TEST_F(PresetsTesting, AddedNewPreset)
{
    constexpr std::string_view kPresetName = "DefaultPreset";

    auto presetId = m_presetsHolder->addNewPreset(kPresetName);

    constexpr std::size_t kPresetsCount = 1;
    EXPECT_EQ(m_presetsHolder->getPresetById(presetId)->presetName(), kPresetName);
    EXPECT_EQ(m_presetsHolder->presetsCount(), kPresetsCount);
    EXPECT_EQ(presetId, std::hash<std::string_view>()(kPresetName));
}